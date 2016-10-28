#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SDManager.hh"
#include "DetectorHit.hh"
#include "DetectorConstruction.hh"
//ROOT Stuff
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "G4RootFileManager.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4THitsMap.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include "DetId.hh"
#include "Math/Vector3D.h"
#include "tools/wroot/file"
#include <algorithm>
#include "hit.hh"
#include "global.hh"
//
#include "JediRun.hh"
#include <G4Threading.hh>
#include <G4MTRunManager.hh>
#include "TrackerSensitiveDetector.hh"
#include "CaloSensitiveDetector.hh"
#include "G4AutoLock.hh"
#include "PrimaryGeneratorAction.hh"
//namespace { G4Mutex AnalysisMutex = G4MUTEX_INITIALIZER; }
G4String Analysis::fGeneratorName=G4String("gen");
Analysis* Analysis::fgMasterInstance = nullptr;
G4ThreadLocal Analysis* Analysis::fgInstance = nullptr;
Analysis::Analysis(G4bool isMaster):fEnabled(false),fFileName("")
{
	if ( ( isMaster && fgMasterInstance ) || ( fgInstance ) ) {
		G4ExceptionDescription description;
		description
		<< "      "
		<< "Analysis already exists."
		<< "Cannot create another instance.";
		G4Exception("Analysis::Analysis()",
				"Analysis_F001", FatalException, description);
	}
	if ( isMaster ) fgMasterInstance = this;
	fgInstance = this;
	fAnalysisMessenger=new G4GenericMessenger(this,"/analysis/","analysis control");
	fAnalysisMessenger->DeclareProperty("setFileName",Analysis::fFileName,"set file name");
	fAnalysisMessenger->DeclareMethod("Enable",&Analysis::enable,"enable analysis");
	fAnalysisMessenger->DeclareMethod("Disable",&Analysis::disable,"disable analysis");

	fGenEvents=new std::vector<genevent_t>();
	fSimEvents=new std::vector<simevent_t>();
}

void Analysis::BeginOfRun() {
	fSimEvents->clear();
	fGenEvents->clear();
}
void Analysis::EndOfRun(const G4Run* run) {
	if(!fEnabled) return;
	if(!G4Threading::IsWorkerThread()){
		if(gVerbose>1)
			G4cout<<"Creating file and trees..."<<G4endl;
		G4String fileName("");
		//Precedence: 1) Messenger 2) Command line 3) Default (run_i.root)
		if(fFileName==""){
			if(gConfig.count("general.output_file"))
				fileName=gConfig["general.output_file"].as<std::string>();
			else{
				std::stringstream fname;
				fname<<"run_"<<run->GetRunID()<<".root";
				if(gVerbose>2)
					G4cout<<"run_"<<run->GetRunID()<<G4endl;
				fileName=fname.str().c_str();
			}
		}else{
			fileName=fFileName;
		}
		TFile OutFile(fileName,"RECREATE");
		TTree SimTree("sim","simulated data");
		TTree GenTree("gen","generated data");
		TTree InfoTree("info","run information");
		auto myRun=static_cast<const JediRun*> (run);


		// Write events from SD to file
		auto GenEvents=myRun->getGenEvents();
		genevent_t* thisEvent=nullptr;
		if(gVerbose>2)
			G4cout<<"generator name:"<<fGeneratorName<<G4endl;
		fOutBranches["gen"]=GenTree.Branch("events","genevent_t",&thisEvent);
		for(auto iEvent : GenEvents){
			thisEvent=&iEvent;
			GenTree.Fill();
		}


		auto SimEvents=&myRun->getSimEvents();
		auto first=(*SimEvents)[0];

		for(auto detector : first.calorimeter){
			if ( std::find(fCaloSDNames.begin(),fCaloSDNames.end(),G4String(detector.first)) == fCaloSDNames.end()){
				if(gVerbose>2)
					G4cout<<"Creating branch for "<<detector.first<<G4endl;
				fCaloHits[detector.first]=nullptr;
				fCaloSDNames.push_back(detector.first);
				fOutBranches[detector.first]=SimTree.Branch(detector.first.c_str(),"std::vector<calorhit_t>",&fCaloHits[detector.first]);
			}
		}
		for(auto detector : first.tracker){
			if ( std::find(fTrackerSDNames.begin(),fTrackerSDNames.end(),G4String(detector.first)) == fTrackerSDNames.end()){
				if(gVerbose>2)
					G4cout<<"Creating branch for "<<detector.first<<G4endl;
				fTrackerHits[detector.first]=nullptr;
				fTrackerSDNames.push_back(detector.first);
				fOutBranches[detector.first]=SimTree.Branch(detector.first.c_str(),"std::vector<trackerhit_t>",&fTrackerHits[detector.first]);
			}
		}

		for(auto evt : *SimEvents){
			for(auto detector : evt.calorimeter){
				fCaloHits[detector.first]=new std::vector<calorhit_t>(detector.second);
			}
			for(auto detector : evt.tracker){
				fTrackerHits[detector.first]=new std::vector<trackerhit_t>(detector.second);
			}
			SimTree.Fill();

			for(auto pointer : fCaloHits){
				if(pointer.second)
					delete pointer.second;
				pointer.second=nullptr;
			}
			for(auto pointer : fTrackerHits){
				if(pointer.second)
					delete pointer.second;
				pointer.second=nullptr;
			}
		}

		//
		if(gVerbose>2)
			G4cout<<"Writing Tree to file..."<<G4endl;
		OutFile.Write();
		fCaloHits.clear();
		fTrackerHits.clear();
		fOutBranches.clear();
		fCaloSDNames.clear();
		fTrackerSDNames.clear();
	}

}

void Analysis::RegisterTrackerSD(TrackerSensitiveDetector* sd) {
	if(gVerbose>3)
		G4cout<<"Analysis::RegisterTrackerSD: "<<sd->GetName()<<G4endl;
	if(std::find(fTrackerSD.begin(),fTrackerSD.end(),sd)==fTrackerSD.end())
		fTrackerSD.push_back(sd);
	return;
}

void Analysis::RegisterCaloSD(CaloSensitiveDetector* sd) {
	if(gVerbose>3)
		G4cout<<"Analysis::RegisterCaloSD: "<<sd->GetName()<<G4endl;
	if(std::find(fCaloSD.begin(),fCaloSD.end(),sd)==fCaloSD.end())
		fCaloSD.push_back(sd);
	return;
}

void Analysis::UnRegisterTrackerSD(TrackerSensitiveDetector* sd) {
	if(gVerbose>3)
		G4cout<<"Analysis::UnRegisterTrackerSD: "<<sd->GetName()<<G4endl;
	auto pos=std::find(fTrackerSD.begin(),fTrackerSD.end(),sd);
	if(pos!=fTrackerSD.end())
		fTrackerSD.erase(pos);
}

void Analysis::UnRegisterCaloSD(CaloSensitiveDetector* sd) {
	if(gVerbose>3)
		G4cout<<"Analysis::UnRegisterCaloSD: "<<sd->GetName()<<G4endl;
	auto pos=std::find(fCaloSD.begin(),fCaloSD.end(),sd);
	if(pos!=fCaloSD.end())
		fCaloSD.erase(pos);
}


void Analysis::BeginOfEvent() {}

void Analysis::EndOfEvent(const G4Event* evt) {
	simevent_t thisSimEvent;
	thisSimEvent.eventid=evt->GetEventID();
	for(auto iSD:fCaloSD){
		thisSimEvent.calorimeter[iSD->GetName()]=*iSD->getVect();
	}
	for(auto iSD:fTrackerSD){
		thisSimEvent.tracker[iSD->GetName()]=*iSD->getVect();
	}
	fSimEvents->push_back(thisSimEvent);
	genevent_t thisGenEvent;
	thisGenEvent.eventid=evt->GetEventID();
	auto gen=static_cast<const PrimaryGeneratorAction*>(G4MTRunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	if(gen==nullptr){
		G4Exception("Analysis::EndOfEvent","",FatalException,"Could not find PrimaryGeneratorAction or pointer is invalid!");
	}
	if(evt->GetEventID()==0){
		if(gVerbose>3)
			G4cout<<"Analysis:: Generator Name is now: "<<gen->getGeneratorName()<<G4endl;
		fGeneratorName=gen->getGeneratorName();
	}
	thisGenEvent=gen->getGenEvent();
	fGenEvents->push_back(thisGenEvent);
}
