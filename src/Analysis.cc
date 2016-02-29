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
#include <G4Threading.hh>
#include "TrackerSensitiveDetector.hh"
#include "CaloSensitiveDetector.hh"
#include "G4AutoLock.hh"
namespace { G4Mutex AnalysisMutex = G4MUTEX_INITIALIZER; }




Analysis* Analysis::fgMasterInstance = 0;
G4ThreadLocal Analysis* Analysis::fgInstance = 0;

Analysis::Analysis(G4bool isMaster):fEnabled(false),fOutFile(0),fMyWorkerId(-1)
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
	fMyWorkerId=G4Threading::G4GetThreadId();
	fgInstance = this;
	fAnalysisMessenger=new G4GenericMessenger(this,"/analysis/","analysis control");
	fAnalysisMessenger->DeclareProperty("setFileName",Analysis::fFileName,"set file name");
	fAnalysisMessenger->DeclareMethod("Enable",&Analysis::enable,"enable analysis");
	fAnalysisMessenger->DeclareMethod("Disable",&Analysis::disable,"disable analysis");
}

TTree* Analysis::MakeTree(G4String name, G4String desc) {
	auto res=new TTree(name,desc);
	fOutTrees.push_back(res);
	return res;
}

void Analysis::BeginOfRun() {

	G4AutoLock lock(&AnalysisMutex);
	std::ostringstream name;
	G4String base;
	G4String extension;
	if ( fFileName.find(".") != std::string::npos ) {
		extension = fFileName.substr(fFileName.find("."));
		base = fFileName.substr(0, fFileName.find("."));
	}
	name<<base<<"_t"<<G4Threading::G4GetThreadId()<<extension;
	if(fOutFile==0){
		fOutFile=new TFile(name.str().c_str(),"RECREATE");
	}
	else{
		G4cout<<"fOutFile already set!"<<G4endl;
		return;
	}
	for(auto iSD:fCaloSD){
		iSD->BeginOfRun();
	}
	for(auto iSD:fTrackerSD){
		iSD->BeginOfRun();
	}
}

void Analysis::EndOfRun() {
	G4AutoLock lock(&AnalysisMutex);

	for(auto iTree: fOutTrees){
		//iTree->Write();
	}
	fOutFile->Write();
	fOutFile->Close();
	for(auto iSD:fCaloSD){
		iSD->EndOfRun();
	}
	for(auto iSD:fTrackerSD){
		iSD->EndOfRun();
	}
	//delete fOutFile;
}

void Analysis::RegisterTrackerSD(TrackerSensitiveDetector* sd) {
	fTrackerSD.push_back(sd);
	return;
}

void Analysis::RegisterCaloSD(CaloSensitiveDetector* sd) {
	fCaloSD.push_back(sd);
	return;
}
