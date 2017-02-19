#include <DetectorConstructionFactory.hh>
#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SDManager.hh"
#include "DetectorHit.hh"
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
	if(gVerbose>2)
		for(const auto idet: fSD)
			G4cout<<G4Threading::G4GetThreadId()<<" Analysis::EndOfRun "<<idet->GetName()<<G4endl;
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
		//TTree InfoTree("info","run information");
		auto myRun=static_cast<const JediRun*> (run);

		std::map<G4String,TBranch*> OutBranches;
		// Write events from generator to file
		auto GenEvents=myRun->getGenEvents();
		genevent_t* thisEvent=nullptr;
		if(gVerbose>2)
			G4cout<<"generator name:"<<fGeneratorName<<G4endl;
		OutBranches["gen"]=GenTree.Branch("events","genevent_t",&thisEvent);
		for(auto iEvent : GenEvents){
			thisEvent=&iEvent;
			GenTree.Fill();
		}
		for(const auto idet: fSD)
			idet->WriteHitsToFile(SimTree,myRun);

		OutFile.Write();
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


void Analysis::RegisterSD(JediSensitiveDetector* sd) {
	if(gVerbose>3)
		G4cout<<"Analysis::RegisterSD: "<<sd->GetName()<<G4endl;
	if(std::find(fSD.begin(),fSD.end(),sd)==fSD.end())
		fSD.push_back(sd);
	return;
}

void Analysis::UnRegisterSD(JediSensitiveDetector* sd) {
	if(gVerbose>3)
		G4cout<<"Analysis::UnRegisterSD: "<<sd->GetName()<<G4endl;
	auto pos=std::find(fSD.begin(),fSD.end(),sd);
	if(pos!=fSD.end())
		fSD.erase(pos);
}

void Analysis::EndOfEvent(const G4Event* evt) {
	simevent_t thisSimEvent;
	thisSimEvent.eventid=evt->GetEventID();
	for(const auto iSD : fSD){
		iSD->CopyHitsToRun(thisSimEvent);
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
