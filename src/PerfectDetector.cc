/*
 * PerfectDetector.cc
 *
 *  Created on: Mar 7, 2016
 *      Author: maanen
 */

#include "PerfectDetector.hh"
#include "G4VSensitiveDetector.hh"

#include "DetectorHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class SensitiveDetectorMessenger;
#include <G4String.hh>
#include <map>
#include "TTree.h"
#include <vector>
#include "hit.hh"
#include "Analysis.hh"
PerfectDetector::PerfectDetector(const G4String& name,
		const G4String& hitsCollectionName):TrackerSensitiveDetector(name,hitsCollectionName) {
}

G4bool PerfectDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* history) {
	if(!Analysis::Instance()->isEnabled())
		return false;
	// energy deposit
	G4double edep = aStep->GetTotalEnergyDeposit();

	if (edep==0.)
		return false;

	DetectorHit* newHit = new DetectorHit();

	newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
	newHit->SetEdep(edep);
	newHit->SetEtot(aStep->GetTrack()->GetKineticEnergy());
	newHit->SetPos (aStep->GetPreStepPoint()->GetPosition());
	G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
	G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
	newHit->SetTof(preStepPoint->GetGlobalTime()/CLHEP::s);
	newHit->SetParticleId(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() );
	newHit->setMom(aStep->GetTrack()->GetMomentum());
	fHitsCollection->insert( newHit );
	aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	return true;
}
