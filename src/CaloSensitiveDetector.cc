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
CaloSensitiveDetector::CaloSensitiveDetector(G4String name):G4MultiFunctionalDetector(name) {
	G4VPrimitiveScorer* scorer = new G4PSEnergyDeposit("edep");
	this->RegisterPrimitive(scorer);
	Analysis::GetInstance()->BookObject<TNtuple>(name,name,"event:copyNo:edep");
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent* HC) {
	G4MultiFunctionalDetector::EndOfEvent(HC);
	G4int nPrim = primitives.size();

	/*
	G4int collId=SDMan->GetCollectionID("MyDetector/eDep");
	G4THitsMap<G4double>* evtMap=(G4THitsMap<G4double>*)HCE->GetHC(collId);
	std::map<G4int,G4double*>::iterator itr = evtMap->GetMap()->begin();
		if((itr->first)==0)
			PMT1_Energy=(*(itr->second))/(1*CLHEP::MeV);
		else if((itr->first)==1)
			PMT2_Energy=(*(itr->second))/(1*CLHEP::MeV);
		else{
			G4cerr<<"Output: Error: Unknown detector id "<<G4endl;
			G4Exception("[Output]", "EndOfEvent()", FatalException,
					" ERROR: Unknown detector id.");
		}
	}
	 */

	for(G4int iPrim=0;iPrim<nPrim;iPrim++){
		TNtuple* mytuple;
		try{
			mytuple=Analysis::GetInstance()->GetObject<TNtuple>(this->GetName());
		}
		catch(myG4Exception& exc){
			G4cout <<exc.getExceptionCode()<<" catched in primitive loop"<<G4endl;
			continue;
		}
		G4int collID=this->GetCollectionID(iPrim);
		G4THitsMap<G4double>* evtMap=(G4THitsMap<G4double>*)HC->GetHC(collID);
		std::map<G4int,G4double*>::iterator itr = evtMap->GetMap()->begin();
		for(; itr != evtMap->GetMap()->end(); itr++){
			try{
				mytuple->Fill(G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID(),itr->first,(*(itr->second))/(1*CLHEP::MeV));
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
