/*
 * JediRun.cc
 *
 *  Created on: Mar 6, 2016
 *      Author: maanen
 */

#include <JediRun.hh>
#include "Analysis.hh"
#include <G4Threading.hh>
#include <JediConfigurationManager.hh>
#include <memory>

JediRun::JediRun():G4Run() {}

void JediRun::RecordEvent(const G4Event* evt) {
	G4Run::RecordEvent(evt);
}

void JediRun::Merge(const G4Run* aRun) {
	auto an=Analysis::Instance();
	auto simEvents=an->getSimEvents();
	auto genEvents=an->getGenEvents();
	auto newSimevent=an->getNewSimEvents();
	if(JediConfigurationManager::Instance()->GetVerbose()>2)
		G4cout<<"JediRun::Merge: Thread "<<G4Threading::G4GetThreadId()<<" has "<<simEvents->size()<<" sim events.";
	for(auto iEvent : *simEvents){
		fSimEvents.push_back(iEvent);
	}
	if(JediConfigurationManager::Instance()->GetVerbose()>2)
			G4cout<<"JediRun::Merge: Thread "<<G4Threading::G4GetThreadId()<<" has "<<genEvents->size()<<" gen events.";
	for(auto iEvent : *genEvents){
		fGenEvents.push_back(iEvent);
	}

	for(auto iEvent : *newSimevent){
		fNewSimEvents.push_back(iEvent);
	}
	G4Run::Merge(aRun);
}
