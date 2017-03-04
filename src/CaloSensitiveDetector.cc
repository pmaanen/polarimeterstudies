/*
 * CaloSensitiveDetector.cc
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#include <CaloSensitiveDetector.hh>
#include "JediRun.hh"
#include "G4String.hh"
#include "Analysis.hh"
#include <G4Event.hh>
#include "TNtuple.h"
#include "G4EventManager.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4Transform3D.hh>
#include <G4VisManager.hh>
#include <G4UnitsTable.hh>
#include <JediCommon.hh>
#include <JediException.hh>
#include "G4AutoLock.hh"
CaloSensitiveDetector::CaloSensitiveDetector(const G4String& name,G4int depth):JediSensitiveDetector_impl(name) {
	fHits=std::unique_ptr<std::vector<calorhit_t>>(new std::vector<calorhit_t>);
	fDepth=depth;
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
	fHits->clear();
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
		fHits->push_back(hit);
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
	auto index=GetIndex(aStep);

	th->GetReplicaNumber();

	if(fHitMap.count(index)==0)
		fHitMap[index]=edep;
	else
		fHitMap[index]+=edep;
	return true;
}

void CaloSensitiveDetector::WriteHitsToFile(TTree& aTree,
		const G4Run* aRun) const {

	if(gVerbose>2)
		G4cout<<"CaloSensitiveDetector::WriteHitsToFile "<<fName<<G4endl;

	auto SimEvents=dynamic_cast<const JediRun*>(aRun)->getSimEvents();
	const std::vector<calorhit_t> *hitPointer=nullptr;
	auto branch=aTree.Branch(fName,&hitPointer);

	for(const auto &evt : SimEvents){
		hitPointer=&evt.calorimeter.at(fName);
		branch->Fill();
	}
}

void CaloSensitiveDetector::CopyHitsToRun(simevent_t& anEvent) const {
	anEvent.calorimeter[GetName()]=*fHits.get();
}

G4int CaloSensitiveDetector::GetIndex(G4Step* aStep) {
	G4int index=0;
	auto touchable=aStep->GetPreStepPoint()->GetTouchable();
	if(touchable)
		index=touchable->GetCopyNumber(fDepth);
	else{
		G4ExceptionDescription ed;
		ed<<"Could not find touchable for detector "<<GetName();
		G4Exception(" CaloSensitiveDetector::GetIndex","",FatalException,ed);
	}
	return index;
}
