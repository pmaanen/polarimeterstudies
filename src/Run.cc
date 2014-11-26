/*
 * Run.cc
 *
 *  Created on: 21.11.2014
 *      Author: pmaanen
 */

#include <Run.hh>
#include "Analysis.hh"
#include "G4Threading.hh"
Run::Run():G4Run() {
	return;
	if(!G4Threading::IsWorkerThread()){
		G4cout<<"Run::Run() has been called for master."<<G4endl;
	}
	else{
		G4cout<<"Run::Run() has been called for slave "<<G4Threading::G4GetThreadId()<<G4endl;
	}


}

void Run::RecordEvent(const G4Event* evt) {
	return;
	if(!G4Threading::IsWorkerThread()){
		G4cout<<"Run::RecordEvent() has been called for master."<<G4endl;
	}
	else{
		G4cout<<"Run::RecordEvent() has been called for slave "<<G4Threading::G4GetThreadId()<<G4endl;
	}

}

void Run::Merge(const G4Run*) {
}
