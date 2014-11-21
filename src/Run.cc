/*
 * Run.cc
 *
 *  Created on: 21.11.2014
 *      Author: pmaanen
 */

#include <Run.hh>
#include "Analysis.hh"
Run::Run() {
	// TODO Auto-generated constructor stub

}

void Run::RecordEvent(const G4Event* evt) {
	Analysis::GetInstance()->EndOfEvent(evt);
}

void Run::Merge(const G4Run*) {
}
