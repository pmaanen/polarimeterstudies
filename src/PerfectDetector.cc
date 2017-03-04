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
#include <JediClasses.hh>
#include "Analysis.hh"
#include <JediCommon.hh>
#include <G4VProcess.hh>
#include <algorithm>
PerfectDetector::PerfectDetector(const G4String& name):TrackerSensitiveDetector(name) {
}

G4bool PerfectDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
	if(!Analysis::Instance()->isEnabled())
		return false;
	// energy deposit
	if(std::find(fTrackIds.begin(), fTrackIds.end(), aStep->GetTrack()->GetTrackID() ) != fTrackIds.end())
		return false;
	fTrackIds.push_back(aStep->GetTrack()->GetTrackID());
	/*
	if(aStep->GetTrack()->GetCreatorProcess()){
			auto processName=aStep->GetTrack()->GetCreatorProcess()->GetProcessName();
			auto particleName=aStep->GetTrack()->GetParticleDefinition()->GetParticleName();
			auto id=aStep->GetTrack()->GetTrackID();
			G4cout<<"Track #"<<id<<" "<<particleName<<" "<<processName<<G4endl;
	}
	 */

	DetectorHit newHit;
	newHit.SetTrackID(aStep->GetTrack()->GetTrackID());
	newHit.SetEdep(-1);
	//newHit.SetDetId(history->GetCopyNumber());
	newHit.SetEkin(aStep->GetPreStepPoint()->GetKineticEnergy());
	newHit.SetPos (aStep->GetPreStepPoint()->GetPosition());
	G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
	G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
	newHit.SetTof(preStepPoint->GetGlobalTime()/CLHEP::ns);
	newHit.SetParticleId(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() );
	newHit.setMom(aStep->GetTrack()->GetMomentum());
	fHitBuffer.push_back( newHit );
	return true;
}

void PerfectDetector::EndOfEvent(G4HCofThisEvent* hitCollection) {
	TrackerSensitiveDetector::EndOfEvent(hitCollection);
	fTrackIds.clear();
}
