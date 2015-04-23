/*
 * CaloSensitiveDetector.cc
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#include <CaloSensitiveDetector.hh>
#include "G4String.hh"
#include "Analysis.hh"
#include "G4Exception.hh"
#include "TNtuple.h"
#include "G4EventManager.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
CaloSensitiveDetector::CaloSensitiveDetector(G4String name):G4MultiFunctionalDetector(name),myTupleId(0) {
	G4VPrimitiveScorer* scorer = new G4PSEnergyDeposit("edep");
	this->RegisterPrimitive(scorer);
	Analysis* an=Analysis::Instance();
	myTupleId.push_back(an->CreateNtuple(name,name));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"event"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"detid"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"edep"));
	an->FinishNtuple(myTupleId[0]);
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent* HC) {
	G4MultiFunctionalDetector::EndOfEvent(HC);
	G4int nPrim = primitives.size();
	Analysis* an=Analysis::Instance();
	for(G4int iPrim=0;iPrim<nPrim;iPrim++){
		G4int collID=this->GetCollectionID(iPrim);
		G4THitsMap<G4double>* evtMap=(G4THitsMap<G4double>*)HC->GetHC(collID);
		std::map<G4int,G4double*>::iterator itr = evtMap->GetMap()->begin();
		for(; itr != evtMap->GetMap()->end(); itr++){
			try{
				an->FillNtupleIColumn(myTupleId[0],myTupleId[1],G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
				an->FillNtupleIColumn(myTupleId[0],myTupleId[2],itr->first);
				an->FillNtupleFColumn(myTupleId[0],myTupleId[3],*(itr->second)/CLHEP::MeV);
				an->AddNtupleRow(myTupleId[0]);
			}

			catch(myG4Exception& exc){
				G4cout <<exc.getExceptionCode()<<" catched in hit loop"<<G4endl;
				continue;
			}
		}
	}
}

void CaloSensitiveDetector::Initialize(G4HCofThisEvent* HC) {
	G4MultiFunctionalDetector::Initialize(HC);
}
