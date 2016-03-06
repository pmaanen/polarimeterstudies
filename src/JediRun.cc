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
	// TODO Auto-generated constructor stub

}

JediRun::~JediRun() {
	// TODO Auto-generated destructor stub
}

void JediRun::RecordEvent(const G4Event* evt) {
	G4Run::RecordEvent(evt);
}

void JediRun::Merge(const G4Run* aRun) {
	auto an=Analysis::Instance();
	auto events=an->getEvents();
	for(auto iEvent : events){
		fEvents.push_back(iEvent);
	}
	G4Run::Merge(aRun);
}
