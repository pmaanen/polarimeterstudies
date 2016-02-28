/*
 * CaloSensitiveDetector.cc
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#include <CaloSensitiveDetector.hh>
#include "G4String.hh"
#include "Analysis.hh"
#include <G4Event.hh>
#include "G4Exception.hh"
#include "TNtuple.h"
#include "G4EventManager.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4Transform3D.hh>
#include <G4VisManager.hh>
CaloSensitiveDetector::CaloSensitiveDetector(G4String name,G4int depth):G4MultiFunctionalDetector(name),fTupleId(),fName(name),fRunInitialized(false) {
	G4VPrimitiveScorer* scorer = new G4PSEnergyDeposit("edep",depth);
	this->RegisterPrimitive(scorer);
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent* HC) {
	G4MultiFunctionalDetector::EndOfEvent(HC);
	G4int nPrim = primitives.size();
	Analysis* an=Analysis::Instance();
	if(an->isEnabled()){
		for(G4int iPrim=0;iPrim<nPrim;iPrim++){
			G4int collID=this->GetCollectionID(iPrim);
			auto evtMap=(G4THitsMap<G4double>*)HC->GetHC(collID);
			for(const auto& iHit:*(evtMap->GetMap())){
				try{
					an->FillNtupleIColumn(fTupleId[0],fTupleId[1],G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
					an->FillNtupleIColumn(fTupleId[0],fTupleId[2],iHit.first);
					an->FillNtupleFColumn(fTupleId[0],fTupleId[3],*(iHit.second)/CLHEP::MeV);
					an->FillNtupleFColumn(fTupleId[0],fTupleId[4],0);
					an->AddNtupleRow(fTupleId[0]);
				}
				catch(myG4Exception& exc){
					G4cout <<exc.getExceptionCode()<<" caught in hit loop"<<G4endl;
					continue;
				}
			}
		}
	}
}

void CaloSensitiveDetector::Initialize(G4HCofThisEvent* HC) {
	G4MultiFunctionalDetector::Initialize(HC);

	Analysis* an=Analysis::Instance();
	if(an->isEnabled() and !fRunInitialized){
		fTupleId.push_back(an->CreateNtuple(fName,fName));
		fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"event"));
		fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"detid"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"edep"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"time"));
		an->FinishNtuple(fTupleId[0]);
		fRunInitialized=true;
	}
}

G4bool CaloSensitiveDetector::ProcessHits(G4Step* step,
		G4TouchableHistory* history) {
	/*
	auto touchable=step->GetPreStepPoint()->GetTouchableHandle();
	if(touchable){
		auto physVol=touchable->GetVolume();
		G4VisAttributes attribs(G4Colour(1.,0.,0.));
		attribs.SetForceSolid(true);
		G4RotationMatrix rot;
		if(physVol){
			physVol->GetLogicalVolume();
			this->
			if(physVol->GetRotation())//If a rotation is defined use it
				rot=*(physVol->GetRotation());
			G4Transform3D trans(rot,physVol->GetTranslation());//Create transform
			G4VisManager::GetConcreteInstance()->Draw(*physVol,attribs,trans);//Draw it
			G4cout<<"Drawing..."<<physVol->GetName()<<G4endl;
		}
	}
	 */
	return G4MultiFunctionalDetector::ProcessHits(step,history);
}
