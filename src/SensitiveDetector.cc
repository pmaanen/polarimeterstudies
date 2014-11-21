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

#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "TrackerId.hh"
#include "SensitiveDetectorMessenger.hh"
#include "Analysis.hh"
#include "EventAction.hh"
#include "TNtuple.h"
#include "CLHEP/Units/SystemOfUnits.h"
using namespace CLHEP;

#define NDEBUG
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SensitiveDetector::SensitiveDetector(const G4String& name,
		const G4String& hitsCollectionName)
: G4VSensitiveDetector(name),
  fHitsCollection(NULL),
  reso(0*mm)
{
	collectionName.insert(name);
	sensMessenger=new SensitiveDetectorMessenger(this);
	Analysis::GetInstance()->BookObject<TNtuple>(this->GetName(),this->GetName(),"event:edep:x:y:z");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SensitiveDetector::~SensitiveDetector() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void SensitiveDetector::SetResolution(G4double res){
	reso=res;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SensitiveDetector::Initialize(G4HCofThisEvent* hce)
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

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, 
		G4TouchableHistory*)
{  
	// energy deposit
	G4double edep = aStep->GetTotalEnergyDeposit();

	if (edep==0.) return false;

	DetectorHit* newHit = new DetectorHit();

	newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
	// tracker =1 dE=2 Calor=3
	G4int subdet=0; // 1=barrel 2=endcap
	G4int layer=0;
	newHit->SetEdep(edep);
	newHit->SetTruePos (aStep->GetPreStepPoint()->GetPosition());
	G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
	G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();


	//G4String gmother(theTouchable->GetVolume(2)->GetName());

	//G4String mother(theTouchable->GetVolume(1)->GetName());
	newHit->SetTof(preStepPoint->GetGlobalTime());

	TrackerId modId(subdet,layer);
	newHit->SetDetId(modId.rawId());
	G4ThreeVector worldPosition = preStepPoint->GetPosition();
	G4ThreeVector localPosition = theTouchable->GetHistory()->
			GetTopTransform().TransformPoint(worldPosition);



	// smearing

	G4double y=localPosition.y()+G4RandGauss::shoot(0,1)*reso;
	G4double z=localPosition.z()+G4RandGauss::shoot(0,1)*reso;
	G4double x=localPosition.x();

	G4ThreeVector smearedLocalPosition(x,y,z);
	G4ThreeVector smearedGlobalPosition(theTouchable->GetHistory()->
			GetTopTransform().Inverse().TransformPoint(smearedLocalPosition));
	newHit->SetPos(smearedGlobalPosition);
	newHit->SetParticleId(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() );
	fHitsCollection->insert( newHit );
	return true;
	if(smearedGlobalPosition.mag()<0.01)
		newHit->Print();

	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
	/*
	G4int nofHits = fHitsCollection->entries();
	if ( verboseLevel>1 ) {
		G4cout << "\n-------->Hits Collection: in this event they are " << nofHits
				<< " hits in the tracker chambers: " << G4endl;
	}

	for ( G4int i=0; i<nofHits; i++ ){

		G4double edep,x,y,z;
		G4int eventID=EventAction::GetInstance()->GetEventno();
		edep=(*fHitsCollection)[i]->GetEdep();
		x=(*fHitsCollection)[i]->GetPos().getX();
		y=(*fHitsCollection)[i]->GetPos().getY();
		z=(*fHitsCollection)[i]->GetPos().getZ();
			Analysis::GetInstance()->GetObject<TNtuple>(this->GetName())->Fill(eventID,edep/MeV,x/mm,y/mm,z/mm);

	}
	return;
	*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
