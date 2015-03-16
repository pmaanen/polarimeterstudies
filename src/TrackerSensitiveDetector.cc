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
  myTupleId()
{
	collectionName.insert(name);
	Analysis* an=Analysis::Instance();
	myTupleId.push_back(an->CreateNtuple(name,name));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"event"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"trackId"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"particleId"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"edep"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"x"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"y"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"z"));
	an->FinishNtuple(myTupleId[0]);
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

	newHit->SetTof(preStepPoint->GetGlobalTime());
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
	static G4int HCID = -1;
	if(HCID<0)
	{
		HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	}
	HCE->AddHitsCollection(HCID,fHitsCollection);
	G4int nHits=fHitsCollection->entries();
	for(G4int ii=0;ii<nHits;ii++){
		an->FillNtupleIColumn(myTupleId[0],myTupleId[1],G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
		an->FillNtupleIColumn(myTupleId[0],myTupleId[2],(*fHitsCollection)[ii]->GetTrackID());
		an->FillNtupleIColumn(myTupleId[0],myTupleId[3],(*fHitsCollection)[ii]->GetParticleId());
		an->FillNtupleFColumn(myTupleId[0],myTupleId[4],(*fHitsCollection)[ii]->GetEdep());
		an->FillNtupleFColumn(myTupleId[0],myTupleId[5],(*fHitsCollection)[ii]->GetPos().x());
		an->FillNtupleFColumn(myTupleId[0],myTupleId[6],(*fHitsCollection)[ii]->GetPos().y());
		an->FillNtupleFColumn(myTupleId[0],myTupleId[7],(*fHitsCollection)[ii]->GetPos().z());
		an->AddNtupleRow(myTupleId[0]);
	}
	/*
	for(auto iPart=particleNames.begin();iPart!=particleNames.end();++iPart){
		G4cout<<iPart->first<<"(s) found in event. "<<G4endl;
	}
	*/
	return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
