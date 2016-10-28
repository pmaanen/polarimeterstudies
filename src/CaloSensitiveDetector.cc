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
#include "global.hh"
#include "G4EventManager.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4Transform3D.hh>
#include <G4VisManager.hh>
#include <G4UnitsTable.hh>
#include "G4AutoLock.hh"
CaloSensitiveDetector::CaloSensitiveDetector(const G4String& name):JediSensitiveDetector_impl(name) {
	Analysis::Instance()->RegisterCaloSD(this);
	vect=std::unique_ptr<std::vector<calorhit_t>>(new std::vector<calorhit_t>);
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
	vect->clear();
	if(gVerbose>3)
		G4cout<<fName<<": "<<"CaloSensitiveDetector::EndOfEvent"<<G4endl;
	for(const auto &iHit : fHitMap){
		calorhit_t hit;
		hit.edep=iHit.second/CLHEP::MeV;
		hit.detid=iHit.first;
		if(gVerbose>3){
			G4int i=0;
			G4cout<<fName<<" hit "<<i++<<": "<<G4BestUnit(hit.edep,"Energy")<<" in det no. "<<hit.detid<<G4endl;
		}
		vect->push_back(hit);
	}
	fHitMap.clear();
}

G4bool CaloSensitiveDetector::ProcessHits(G4Step* aStep,
		G4TouchableHistory*) {
	if(gVerbose>4)
			G4cout<<fName<<": "<<"CaloSensitiveDetector::ProcessHits"<<G4endl;
	if(!Analysis::Instance()->isEnabled())
		return false;
	// energy deposit
	G4double edep = aStep->GetTotalEnergyDeposit();

	if (edep==0.)
		return false;

	auto preStep = aStep->GetPreStepPoint();
	auto th = (G4TouchableHistory*)(preStep->GetTouchable());
	auto index=th->GetReplicaNumber();

	if(fHitMap.count(index)==0)
		fHitMap[index]=edep;
	else
		fHitMap[index]+=edep;
	return true;
}

CaloSensitiveDetector::~CaloSensitiveDetector() {
	Analysis::Instance()->UnRegisterCaloSD(this);
}

