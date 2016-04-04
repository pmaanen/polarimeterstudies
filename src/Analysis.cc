#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SDManager.hh"
#include "DetectorHit.hh"
#include "DetectorConstruction.hh"
#include "AnalysisMessenger.hh"
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
//
#include "JediRun.hh"
#include <G4Threading.hh>
#include <G4MTRunManager.hh>
#include "TrackerSensitiveDetector.hh"
#include "CaloSensitiveDetector.hh"
#include "G4AutoLock.hh"
#include "PrimaryGeneratorAction.hh"
namespace { G4Mutex AnalysisMutex = G4MUTEX_INITIALIZER; }
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
		G4cout<<"Creating file and trees..."<<G4endl;
		G4String fileName("");
		if(fFileName==""){
			std::stringstream fname;
			fname<<"run_"<<run->GetRunID()<<".root";
			fileName=fname.str().c_str();
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
		G4cout<<"generator name:"<<fGeneratorName<<G4endl;
		fOutBranches["gen"]=GenTree.Branch(fGeneratorName,"genevent_t",&thisEvent);
		for(auto iEvent : GenEvents){
			thisEvent=&iEvent;
			GenTree.Fill();
		}


		auto SimEvents=&myRun->getEvents();
		auto first=(*SimEvents)[0];

		for(auto detector : first.calorimeter){

			if ( std::find(fCaloSDNames.begin(),fCaloSDNames.end(),G4String(detector.first)) == fCaloSDNames.end()){
				G4cout<<"Creating branch for "<<detector.first<<G4endl;
				fCaloHits[detector.first]=nullptr;
				fCaloSDNames.push_back(detector.first);
				fOutBranches[detector.first]=SimTree.Branch(detector.first.c_str(),"std::vector<calorhit_t>",&fCaloHits[detector.first]);
			}
		}
		for(auto detector : first.tracker){
			if ( std::find(fTrackerSDNames.begin(),fTrackerSDNames.end(),G4String(detector.first)) == fTrackerSDNames.end()){
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
				delete pointer.second;
				pointer.second=nullptr;
			}
			for(auto pointer : fTrackerHits){
				delete pointer.second;
				pointer.second=nullptr;
			}

		}

		//
		G4cout<<"Writing Tree to file..."<<G4endl;
		OutFile.Write();
		fOutBranches.clear();
		fCaloSDNames.clear();
		fTrackerSDNames.clear();
	}

}

void Analysis::RegisterTrackerSD(TrackerSensitiveDetector* sd) {
	fTrackerSD.push_back(sd);
	return;
}

void Analysis::RegisterCaloSD(CaloSensitiveDetector* sd) {
	fCaloSD.push_back(sd);
	return;
}

void Analysis::BeginOfEvent() {}

void Analysis::EndOfEvent(const G4Event* evt) {
	simevent_t thisEvent;
	thisEvent.eventid=evt->GetEventID();
	for(auto iSD:fCaloSD){
		thisEvent.calorimeter[iSD->GetName()]=*iSD->getVect();
	}
	for(auto iSD:fTrackerSD){
		thisEvent.tracker[iSD->GetName()]=*iSD->getVect();
	}
	fSimEvents->push_back(thisEvent);
	genevent_t thisGenEvent;
	thisGenEvent.eventid=evt->GetEventID();
	auto gen=static_cast<const PrimaryGeneratorAction*>(G4MTRunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	if(gen==nullptr){
		G4cout<<"Error! Could not convert primary generator!"<<G4endl;
		return;
	}
	if(evt->GetEventID()==0){
		G4cout<<"Setting generator name to "<<gen->getGeneratorName()<<G4endl;
		fGeneratorName=gen->getGeneratorName();
	}
	thisGenEvent=gen->getGenEvent();
	fGenEvents->push_back(thisGenEvent);
}
