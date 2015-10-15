//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file SensitiveDetector.cc
/// \brief Implementation of the SensitiveDetector class

#include "TrackerSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "Analysis.hh"
#include "EventAction.hh"
#include "TNtuple.h"
#include "CLHEP/Units/SystemOfUnits.h"
using namespace CLHEP;

#define NDEBUG
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSensitiveDetector::TrackerSensitiveDetector(const G4String& name,
		const G4String& /*hitsCollectionName*/)
: G4VSensitiveDetector(name),
  fHitsCollection(NULL),
  fTupleId()
{
	collectionName.insert(name);
	fRunInitialized=false;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSensitiveDetector::~TrackerSensitiveDetector() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
	// Create hits collection
	fHitsCollection
	= new DetectorHitsCollection(SensitiveDetectorName, collectionName[0]);

	// Add this collection in hce

	G4int hcID
	= G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hce->AddHitsCollection( hcID, fHitsCollection );
	Analysis* an=Analysis::Instance();
	if(!fRunInitialized and an->isEnabled()){
		fTupleId.clear();
		fTupleId.push_back(an->CreateNtuple(this->GetName(),this->GetName()));
		fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"event"));
		fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"trackId"));
		fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"particleId"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"edep"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"x"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"y"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"z"));
		fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"time"));
		an->FinishNtuple(fTupleId[0]);
		fRunInitialized=true;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TrackerSensitiveDetector::ProcessHits(G4Step* aStep, 
		G4TouchableHistory*)
{  
	// energy deposit
	G4double edep = aStep->GetTotalEnergyDeposit();

	//if (edep==0.) return false;

	DetectorHit* newHit = new DetectorHit();

	newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
	newHit->SetEdep(edep);
	newHit->SetTruePos (aStep->GetPreStepPoint()->GetPosition());
	G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
	G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();


	newHit->SetDetId(theTouchable->GetCopyNumber(2));
	newHit->SetTof(preStepPoint->GetGlobalTime()/CLHEP::s);
	G4ThreeVector worldPosition = preStepPoint->GetPosition();

	newHit->SetPos(worldPosition);
	newHit->SetParticleId(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() );
	/*
	if(particleNames.count(aStep->GetTrack()->GetParticleDefinition()->GetParticleName())==0)
		particleNames[aStep->GetTrack()->GetParticleDefinition()->GetParticleName()]=1;
	 */

	fHitsCollection->insert( newHit );
	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
	Analysis* an=Analysis::Instance();
	if(an->isEnabled()){
		static G4int HCID = -1;
		if(HCID<0)
		{
			HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
		}
		HCE->AddHitsCollection(HCID,fHitsCollection);
		G4int nHits=fHitsCollection->entries();
		for(G4int ii=0;ii<nHits;ii++){
			an->FillNtupleIColumn(fTupleId[0],fTupleId[1],G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
			an->FillNtupleIColumn(fTupleId[0],fTupleId[2],(*fHitsCollection)[ii]->GetTrackID());
			an->FillNtupleIColumn(fTupleId[0],fTupleId[3],(*fHitsCollection)[ii]->GetParticleId());
			an->FillNtupleFColumn(fTupleId[0],fTupleId[4],(*fHitsCollection)[ii]->GetEdep());
			an->FillNtupleFColumn(fTupleId[0],fTupleId[5],(*fHitsCollection)[ii]->GetPos().x()/CLHEP::mm);
			an->FillNtupleFColumn(fTupleId[0],fTupleId[6],(*fHitsCollection)[ii]->GetPos().y()/CLHEP::mm);
			an->FillNtupleFColumn(fTupleId[0],fTupleId[7],(*fHitsCollection)[ii]->GetPos().z()/CLHEP::mm);
			an->FillNtupleFColumn(fTupleId[0],fTupleId[8],(*fHitsCollection)[ii]->GetTof());
			an->AddNtupleRow(fTupleId[0]);
		}
	}
	/*
	for(auto iPart=particleNames.begin();iPart!=particleNames.end();++iPart){
		G4cout<<iPart->first<<"(s) found in event. "<<G4endl;
	}
	 */
	return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
