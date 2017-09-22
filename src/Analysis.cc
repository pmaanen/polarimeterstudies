#include <DetectorConstruction.hh>
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
#include "JediRun.hh"
#include <G4Threading.hh>
#include <G4MTRunManager.hh>
#include <JediClasses.hh>
#include <JediConfigurationManager.hh>
#include "TrackerSensitiveDetector.hh"
#include "CaloSensitiveDetector.hh"
#include "G4AutoLock.hh"
#include "PrimaryGeneratorAction.hh"
G4String Analysis::fGeneratorName=G4String("gen");


Analysis::Analysis():fEnabled(false),fFileName("")
{
	fAnalysisMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,"/analysis/","analysis control"));
	fAnalysisMessenger->DeclareProperty("setFileName",Analysis::fFileName,"set file name");
	fAnalysisMessenger->DeclareMethod("Enable",&Analysis::enable,"enable analysis");
	fAnalysisMessenger->DeclareMethod("Disable",&Analysis::disable,"disable analysis");
	fGenEvents=std::unique_ptr<std::vector<genevent_t>>(new std::vector<genevent_t>());
	fSimEvents=std::unique_ptr<std::vector<simevent_t>>(new std::vector<simevent_t>());
}

void Analysis::BeginOfRun() {
	fSimEvents->clear();
	fGenEvents->clear();
	for(auto iGen:fGenerators)
		iGen->BeginOfRun();
}


G4String Analysis::Filename(const G4Run* run){
	G4String fileName;
	if(fFileName==""){
		if(JediConfigurationManager::Instance()->GetMap().count("general.output_file"))
			fileName=JediConfigurationManager::Instance()->GetMap()["general.output_file"].as<std::string>();
		else{
			std::stringstream fname;
			fname<<"run_"<<run->GetRunID()<<".root";
			Log("run_"+G4String(run->GetRunID()),1);
			fileName=fname.str().c_str();
		}
	}else{
		fileName=fFileName;
	}
	return fileName;
}

void Analysis::EndOfRun(const G4Run* run) {
	if(!fEnabled) return;
	if(G4Threading::IsWorkerThread()) return;
	for(const auto idet: fSD)
		Log(G4String(G4Threading::G4GetThreadId())+" Analysis::EndOfRun "+idet->GetName(),2);
	Log("Creating file and trees...",1);

	TFile OutFile(Filename(run),"RECREATE");

	FillSimTree(run);
	FillGenTree(run);

	OutFile.Write();
}

void Analysis::RegisterMe(JediSensitiveDetector* sd) {
	Log("Analysis::RegisterSD: "+sd->GetName(),2);
	if(std::find(fSD.begin(),fSD.end(),sd)==fSD.end())
		fSD.push_back(sd);
	return;
}

void Analysis::UnRegisterMe(JediSensitiveDetector* sd) {
	Log("Analysis::UnRegisterSD: "+sd->GetName(),2);
	fSD.erase(std::remove(fSD.begin(),fSD.end(),sd),fSD.end());
	return;
}

void Analysis::EndOfEvent(const G4Event* evt) {
	simevent_t thisSimEvent;
	thisSimEvent.eventid=evt->GetEventID();
	for(const auto iSD : fSD){
		iSD->CopyHitsToRun(&thisSimEvent);
	}
	fSimEvents->push_back(thisSimEvent);
	genevent_t thisGenEvent;
	thisGenEvent.eventid=evt->GetEventID();
	for(const auto iGen: fGenerators){
		iGen->CopyVerticesToRun(thisGenEvent);
		iGen->EndOfEvent();
	}
	fGenEvents->push_back(thisGenEvent);
}


void Analysis::FillSimTree(const G4Run* aRun) {
    TTree SimTree("sim","simulated data");
	auto myRun=static_cast<const JediRun*> (aRun);
	std::map<G4String,const std::vector<calorhit_t> *> calohitPointer;
	std::map<G4String,const std::vector<trackerhit_t> *> trackerhitPointer;
	for(const auto& iSD:fSD){
		Log("Creating branch for "+iSD->GetName(),2);
		if(iSD->GetType()==SDtype::kCalorimeter){
			calohitPointer[iSD->GetName()]=nullptr;
			SimTree.Branch(iSD->GetName(),&calohitPointer[iSD->GetName()]);
		}
		else if(iSD->GetType()==SDtype::kPerfect or iSD->GetType()==SDtype::kPerfect){
			trackerhitPointer[iSD->GetName()]=nullptr;
			SimTree.Branch(iSD->GetName(),&trackerhitPointer[iSD->GetName()]);
		}
	}
	auto SimEvents=myRun->getSimEvents();
	for(const auto &evt : SimEvents){
		for(const auto& iSD:fSD){
			if(iSD->GetType()==SDtype::kCalorimeter){
				calohitPointer[iSD->GetName()]=&evt.calorimeter.at(iSD->GetName());
			}
			else if(iSD->GetType()==SDtype::kPerfect or iSD->GetType()==SDtype::kPerfect){
				trackerhitPointer[iSD->GetName()]=&evt.tracker.at(iSD->GetName());
			}
		}
		SimTree.Fill();
	}
}

void Analysis::FillGenTree(TTree& aTree, const G4Run* aRun) {
    TTree GenTree("gen","generated data");
	auto myRun=static_cast<const JediRun*> (aRun);
	std::map<G4String,const std::vector<genvertex_t> *> genVertexPointer;
	auto GenEvents=myRun->getGenEvents();
	Log("GenEvents has "+G4String(GenEvents.size())+" entries. ",2);
	if(!GenEvents.size())
		G4Exception("Analysis::EndOfRun","",FatalException,"GenEvents are empty!");
	std::vector<G4String> generatorNames;
	for(auto iGen:GenEvents[0].generators)
		generatorNames.push_back(iGen.first);


	for(const auto& iGen:generatorNames){
		Log("Creating branch for "+iGen,2);
		genVertexPointer[iGen]=nullptr;
		GenTree.Branch(iGen,&genVertexPointer[iGen]);
	}

	for(const auto &evt : GenEvents){
		for(const auto& iGen:generatorNames){
			genVertexPointer[iGen]=&evt.generators.at(iGen);
		}
		GenTree.Fill();
	}
}


void Analysis::RegisterMe(GenEventProducer* pd) {
	Log("Analysis::RegisterMe: "+pd->getName(),2);
	if(std::find(fGenerators.begin(),fGenerators.end(),pd)==fGenerators.end())
		fGenerators.push_back(pd);
	return;
}

void Analysis::UnRegisterMe(GenEventProducer*pd) {
	Log("Analysis::UnRegisterMe: "+pd->getName(),2);
	fGenerators.erase(std::remove(fGenerators.begin(),fGenerators.end(),pd),fGenerators.end());
	return;
}
