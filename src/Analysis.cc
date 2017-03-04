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
#include "JediRun.hh"
#include <G4Threading.hh>
#include <G4MTRunManager.hh>
#include <JediCommon.hh>
#include "TrackerSensitiveDetector.hh"
#include "CaloSensitiveDetector.hh"
#include "G4AutoLock.hh"
#include "PrimaryGeneratorAction.hh"
namespace { G4Mutex AnalysisMutex = G4MUTEX_INITIALIZER; }
G4String Analysis::fGeneratorName=G4String("gen");
Analysis::Analysis():fEnabled(false),fFileName("")
{
	fAnalysisMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,"/analysis/","analysis control"));
	fAnalysisMessenger->DeclareProperty("setFileName",Analysis::fFileName,"set file name");
	fAnalysisMessenger->DeclareMethod("Enable",&Analysis::enable,"enable analysis");
	fAnalysisMessenger->DeclareMethod("Disable",&Analysis::disable,"disable analysis");

	fGenEvents=new std::vector<genevent_t>();
	fSimEvents=new std::vector<simevent_t>();
}

void Analysis::BeginOfRun() {
	fSimEvents->clear();
	fGenEvents->clear();
	for(auto iGen:fGenerators)
		iGen->BeginOfRun();
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

		std::map<G4String,const std::vector<calorhit_t> *> calohitPointer;
		std::map<G4String,const std::vector<trackerhit_t> *> trackerhitPointer;


		for(const auto& iSD:fSD){
			if(gVerbose>2)
				G4cout<<"Creating branch for "<<iSD->GetName()<<G4endl;
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

		// Write events from generator to file
		std::map<G4String,const std::vector<genvertex_t> *> genVertexPointer;

		auto GenEvents=myRun->getGenEvents();
		if(gVerbose>2){
			G4cout<<"GenEvents has "<<GenEvents.size()<<" entries. "<<G4endl;
		if(GenEvents.size()){
			G4cout<<"The following generators are in the first event: "<<G4endl;
			for(auto iGen:GenEvents[0].generators)
				G4cout<<iGen.first<<" ";
			G4cout<<G4endl;
		}
		}
		if(!GenEvents.size())
			G4Exception("Analysis::EndOfRun","",FatalException,"GenEvents are empty!");
		std::vector<G4String> generatorNames;
		for(auto iGen:GenEvents[0].generators)
			generatorNames.push_back(iGen.first);


		for(const auto& iGen:generatorNames){
			if(gVerbose>2)
				G4cout<<"Creating branch for "<<iGen<<G4endl;
			genVertexPointer[iGen]=nullptr;
			GenTree.Branch(iGen,&genVertexPointer[iGen]);
		}

		for(const auto &evt : GenEvents){
			for(const auto& iGen:generatorNames){
				genVertexPointer[iGen]=&evt.generators.at(iGen);
			}

			GenTree.Fill();
		}
		OutFile.Write();
	}
}

void Analysis::RegisterMe(JediSensitiveDetector* sd) {
	if(gVerbose>2)
		G4cout<<"Analysis::RegisterSD: "<<sd->GetName()<<G4endl;
	if(std::find(fSD.begin(),fSD.end(),sd)==fSD.end())
		fSD.push_back(sd);
	return;
}

void Analysis::UnRegisterMe(JediSensitiveDetector* sd) {
	if(gVerbose>2)
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
	for(const auto iGen: fGenerators){
		iGen->CopyVerticesToRun(thisGenEvent);
		iGen->EndOfEvent();
	}
	fGenEvents->push_back(thisGenEvent);


}

void Analysis::RegisterMe(GenEventProducer* pd) {
	if(gVerbose>2)
		G4cout<<"Analysis::RegisterMe: "<<pd->getName()<<G4endl;
	if(std::find(fGenerators.begin(),fGenerators.end(),pd)==fGenerators.end())
		fGenerators.push_back(pd);
	return;
}

void Analysis::UnRegisterMe(GenEventProducer*pd) {
	if(gVerbose>2)
		G4cout<<"Analysis::UnRegisterMe: "<<pd->getName()<<G4endl;
	auto pos=std::find(fGenerators.begin(),fGenerators.end(),pd);
	if(pos!=fGenerators.end())
		fGenerators.erase(pos);
}
