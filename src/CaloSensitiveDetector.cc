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
#include <JediConfigurationManager.hh>
#include <JediException.hh>
#include "G4AutoLock.hh"
#include "G4SDManager.hh"
CaloSensitiveDetector::CaloSensitiveDetector(const G4String& name,G4int depth):JediVSensitiveDetector(name),CalorimeterCollection(nullptr){
	fHits=std::unique_ptr<std::vector<calorhit_t>>(new std::vector<calorhit_t>);
	fDepth=depth;
	G4String HCname = name + "_HC";
	collectionName.insert(HCname);
	G4cout << collectionName.size() << "   CalorimeterSD name:  " << name << " collection Name: "
			<< HCname << G4endl;
	if(JediConfigurationManager::Instance()->GetVerbose()>2)
		G4cout<<GetName()<<": "<<"CaloSensitiveDetector::CaloSensitiveDetector"<<G4endl;

	fType=SDtype::kCalorimeter;
}

void CaloSensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
	fHits->clear();
	if(JediConfigurationManager::Instance()->GetVerbose()>3)
		G4cout<<GetName()<<": "<<"CaloSensitiveDetector::EndOfEvent"<<G4endl;
	for(const auto &iHit : fHitMap){
		calorhit_t hit;
		hit.edep=iHit.second/CLHEP::MeV;
		hit.detid=iHit.first;
		if(JediConfigurationManager::Instance()->GetVerbose()>3){
			G4int i=0;
			G4cout<<GetName()<<" hit "<<i++<<": "<<G4BestUnit(hit.edep,"Energy")<<" in det no. "<<hit.detid<<G4endl;
		}
		fHits->push_back(hit);
	}
	fHitMap.clear();
	fHitId.clear();

	if ( JediConfigurationManager::Instance()->GetVerbose()>2 ) {
	     G4int nofHits = CalorimeterCollection->entries();
	     G4cout << G4endl
	            << "-------->Hits Collection: in this event they are " << nofHits
	            << " hits in the calorimeter crystal: " << G4endl;
	     for ( G4int i=0; i<nofHits; i++ ) (*CalorimeterCollection)[i]->Print();
	  }
}

G4bool CaloSensitiveDetector::ProcessHits(G4Step* aStep,
		G4TouchableHistory*) {
	if(JediConfigurationManager::Instance()->GetVerbose()>4)
		G4cout<<GetName()<<": "<<"CaloSensitiveDetector::ProcessHits"<<G4endl;
	if(!Analysis::Instance()->isEnabled())
		return false;
	// energy deposit
	G4double edep = aStep->GetTotalEnergyDeposit();

	if (edep==0.)
		return false;

	auto index=GetIndex(aStep);

	if(fHitMap.count(index)==0)
		fHitMap[index]=edep;
	else
		fHitMap[index]+=edep;

	if(fHitId.count(index)==0){
		if(JediConfigurationManager::Instance()->GetVerbose()>4)
			G4cout<<GetName()<<": "<<"CaloSensitiveDetector::ProcessHits creating new hit."<<G4endl;
		auto CalorimeterHit = new JediCalorimeterHit;
		CalorimeterHit->AddEnergy(edep);
		CalorimeterHit->SetPos(aStep->GetPreStepPoint()->GetPosition());
		CalorimeterHit->SetId(index);
		fHitId[index] =
				CalorimeterCollection->insert(CalorimeterHit) -1;
	}
	else{
		(*CalorimeterCollection)[fHitId[index]]->AddEnergy(edep);
	}

	return true;
}

void CaloSensitiveDetector::WriteHitsToFile(TTree* aTree,
		const G4Run* aRun) const {

	if(JediConfigurationManager::Instance()->GetVerbose()>2)
		G4cout<<"CaloSensitiveDetector::WriteHitsToFile "<<GetName()<<G4endl;

	auto SimEvents=dynamic_cast<const JediRun*>(aRun)->getSimEvents();
	const std::vector<calorhit_t> *hitPointer=nullptr;
	auto branch=aTree->Branch(GetName(),&hitPointer);

	for(const auto &evt : SimEvents){
		hitPointer=&evt.calorimeter.at(GetName());
		branch->Fill();
	}
}

void CaloSensitiveDetector::CopyHitsToRun(simevent_t* anEvent) const {
	anEvent->calorimeter[GetName()]=*fHits.get();
}

void CaloSensitiveDetector::Initialize(G4HCofThisEvent* HCE) {
	if(JediConfigurationManager::Instance()->GetVerbose()>3)
		G4cout<<GetName()<<": "<<"CaloSensitiveDetector::Initialize"<<G4endl;
	CalorimeterCollection=new JediCalorimeterHitsCollection(SensitiveDetectorName,collectionName[0]);




	G4cout << "CalorimeterSD::Initialize:  " << SensitiveDetectorName << "   "
			<< collectionName[0] << G4endl;
	G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

	HCE->AddHitsCollection(HCID, CalorimeterCollection);
}

void CaloSensitiveDetector::CopyHitsToRun(SimEvent* anEvent) const {
	// storing the hits in ROOT file
	G4int NbHits = CalorimeterCollection->entries();
	std::vector<JediCalorimeterHit> hitsVector;
	if(JediConfigurationManager::Instance()->GetVerbose()>3)
	{
		G4cout << "\n-------->Storing hits in the ROOT file: in this event there are " << NbHits
				<< " hits in the calorimeter: " << G4endl;
		for (G4int i=0;i<NbHits;i++) (*CalorimeterCollection)[i]->Print();
	}

	for (G4int i=0;i<NbHits;i++)
		hitsVector.push_back(*(*CalorimeterCollection)[i]);

	anEvent->calorimeter[GetName()]=hitsVector;
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
