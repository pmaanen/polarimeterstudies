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

#include "G4AutoLock.hh"
namespace { G4Mutex CaloSDMutex = G4MUTEX_INITIALIZER; }


CaloSensitiveDetector::CaloSensitiveDetector(G4String name,G4int depth):G4MultiFunctionalDetector(name),fName(name) {
	G4VPrimitiveScorer* scorer = new G4PSEnergyDeposit("edep",depth);
	this->RegisterPrimitive(scorer);
	Analysis::Instance()->RegisterCaloSD(this);
	vect=new std::vector<calorhit_t>;
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent* HC) {
	G4MultiFunctionalDetector::EndOfEvent(HC);
	G4int nPrim = primitives.size();
	Analysis* an=Analysis::Instance();
	G4AutoLock lock(&CaloSDMutex);
	if(an->isEnabled()){
		vect->clear();
		for(G4int iPrim=0;iPrim<nPrim;iPrim++){
			G4int collID=this->GetCollectionID(iPrim);
			auto evtMap=(G4THitsMap<G4double>*)HC->GetHC(collID);
			for(const auto& iHit:*(evtMap->GetMap())){
				try{
					calorhit_t hit;
					hit.detid=iHit.first;
					hit.edep=*(iHit.second)/CLHEP::MeV;
					vect->push_back(hit);
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

void CaloSensitiveDetector::BeginOfRun() {
	G4AutoLock lock(&CaloSDMutex);
	auto myTree=Analysis::Instance()->GetTree();
	myTree->Branch(fName,"std::vector<calorhit_t>",&vect);
}

void CaloSensitiveDetector::EndOfRun() {}
