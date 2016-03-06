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
namespace { G4Mutex AnalysisMutex = G4MUTEX_INITIALIZER; }

Analysis* Analysis::fgMasterInstance = nullptr;
G4ThreadLocal Analysis* Analysis::fgInstance = nullptr;
Analysis::Analysis(G4bool isMaster):fEnabled(false),fOutFile(nullptr),fOutTree(nullptr),fFileName("")
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
}

TTree* Analysis::GetTree() {
	return fOutTree;
}

void Analysis::BeginOfRun() {fEvents.clear();}
void Analysis::EndOfRun(const G4Run* run) {
	if(!fEnabled) return;
	if(!G4Threading::IsWorkerThread()){
		G4cout<<"Creating file and trees..."<<G4endl;
		if(fFileName==""){
			std::stringstream fname;
			fname<<"run_"<<run->GetRunID()<<".root";
			fOutFile=new TFile(fname.str().c_str(),"RECREATE");
		}else{
			fOutFile=new TFile(fFileName,"RECREATE");
		}
		fOutTree=new TTree("sim","simulated events");

		auto myRun=static_cast<const JediRun*> (run);
		auto events=myRun->getEvents();
		auto first=events[0];

		for(auto detector : first.calorimeter){

			if ( std::find(fCaloSDNames.begin(),fCaloSDNames.end(),G4String(detector.first)) == fCaloSDNames.end()){
				G4cout<<"Creating branch for "<<detector.first<<G4endl;
				fCaloHits[detector.first]=nullptr;
				fCaloSDNames.push_back(detector.first);
				fOutBranches[detector.first]=fOutTree->Branch(detector.first.c_str(),"std::vector<calorhit_t>",&fCaloHits[detector.first]);
			}
		}
		for(auto detector : first.tracker){
			if ( std::find(fTrackerSDNames.begin(),fTrackerSDNames.end(),G4String(detector.first)) == fTrackerSDNames.end()){
				G4cout<<"Creating branch for "<<detector.first<<G4endl;
				fTrackerHits[detector.first]=nullptr;
				fTrackerSDNames.push_back(detector.first);
				fOutBranches[detector.first]=fOutTree->Branch(detector.first.c_str(),"std::vector<trackerhit_t>",&fTrackerHits[detector.first]);
			}
		}
		for(auto evt : events){
			for(auto detector : evt.calorimeter){
				fCaloHits[detector.first]=new std::vector<calorhit_t>(detector.second);
			}
			for(auto detector : evt.tracker){
				fTrackerHits[detector.first]=new std::vector<trackerhit_t>(detector.second);
			}
			fOutTree->Fill();
			for(auto pointer : fCaloHits){
				delete pointer.second;
				pointer.second=nullptr;
			}
			for(auto pointer : fTrackerHits){
				delete pointer.second;
				pointer.second=nullptr;
			}
		}
		G4cout<<"Writing Tree to file..."<<G4endl;
		fOutFile->Write();
		fOutFile->Close();

		if(fOutFile!=nullptr){
			delete fOutFile;
			fOutFile=nullptr;
		}
		fOutBranches.clear();
		fCaloSDNames.clear();
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
	event_t thisEvent;
	thisEvent.eventid=evt->GetEventID();
	for(auto iSD:fCaloSD){
		thisEvent.calorimeter[iSD->GetName()]=*iSD->getVect();
	}
	for(auto iSD:fTrackerSD){
		thisEvent.tracker[iSD->GetName()]=*iSD->getVect();
	}
	fEvents.push_back(thisEvent);
}
