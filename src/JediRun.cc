/*
 * JediRun.cc
 *
 *  Created on: Mar 6, 2016
 *      Author: maanen
 */

#include <JediRun.hh>
#include "Analysis.hh"
#include <G4Threading.hh>
JediRun::JediRun() {
	G4cout<<"JediRun::JediRun(): "<<fEvents.size()<<" events."<<G4endl;
}

JediRun::~JediRun() {

}

void JediRun::RecordEvent(const G4Event* evt) {
	G4Run::RecordEvent(evt);
}

void JediRun::Merge(const G4Run* aRun) {
	auto an=Analysis::Instance();
	auto events=an->getSimEvents();
	auto genevents=an->getGenEvents();
	G4cout<<"Thread "<<G4Threading::G4GetThreadId()<<" has "<<events->size()<<" events.";
	for(auto iEvent : *events){
		fEvents.push_back(iEvent);
	}
	for(auto iEvent : *genevents){
		fGenEvents.push_back(iEvent);
	}
	G4Run::Merge(aRun);
}
