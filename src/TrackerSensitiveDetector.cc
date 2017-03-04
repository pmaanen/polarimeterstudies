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
#include "JediRun.hh"
#include "Analysis.hh"
#include "EventAction.hh"
#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4ThreeVector.hh>
#include <G4EventManager.hh>
#include <G4Event.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>
#include <G4AutoLock.hh>
#include <Randomize.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <JediCommon.hh>



using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSensitiveDetector::TrackerSensitiveDetector(const G4String& name)
: JediSensitiveDetector_impl(name)
{
	fHits=std::unique_ptr<std::vector<trackerhit_t>>(new std::vector<trackerhit_t>);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TrackerSensitiveDetector::ProcessHits(G4Step* aStep, 
		G4TouchableHistory*)
{  
	if(!Analysis::Instance()->isEnabled())
		return false;
	// energy deposit
	G4double edep = aStep->GetTotalEnergyDeposit();

	if (edep==0.)
		return false;

	DetectorHit newHit;

	newHit.SetTrackID  (aStep->GetTrack()->GetTrackID());
	newHit.SetEdep(edep);
	newHit.SetEkin(aStep->GetTrack()->GetKineticEnergy());
	newHit.SetPos (aStep->GetPreStepPoint()->GetPosition());
	G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
	G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();


	//newHit.SetDetId(theTouchable->GetCopyNumber());
	newHit.SetTof(preStepPoint->GetGlobalTime()/CLHEP::s);
	G4ThreeVector worldPosition = preStepPoint->GetPosition();

	newHit.SetParticleId(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() );
	fHitBuffer.push_back( newHit );
	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
	Analysis* an=Analysis::Instance();
	if(an->isEnabled()){
		fHits->clear();
		for(const auto &iHit : fHitBuffer){
			trackerhit_t hit;
			hit.edep=iHit.GetEdep()/CLHEP::MeV;
			hit.x=iHit.GetPos().getX();
			hit.y=iHit.GetPos().getY();
			hit.z=iHit.GetPos().getZ();
			hit.px=iHit.getMom().getX()/CLHEP::GeV;
			hit.py=iHit.getMom().getY()/CLHEP::GeV;
			hit.pz=iHit.getMom().getZ()/CLHEP::GeV;
			hit.ekin=iHit.GetEtot()/CLHEP::MeV;
			hit.tof=iHit.GetTof();
			hit.pid=iHit.GetParticleId();
			hit.trid=iHit.GetTrackID();
			fHits->push_back(hit);
		}
		/*
	for(auto iPart=particleNames.begin();iPart!=particleNames.end();++iPart){
		G4cout<<iPart->first<<"(s) found in event. "<<G4endl;
	}
		 */
	}
	fHitBuffer.clear();
	return;
}
void TrackerSensitiveDetector::WriteHitsToFile(TTree& aTree,
		const G4Run* aRun) const {

	if(gVerbose>2)
			G4cout<<"TrackerSensitiveDetector::WriteHitsToFile "<<fName<<G4endl;

	auto SimEvents=&dynamic_cast<const JediRun*>(aRun)->getSimEvents();
	const std::vector<trackerhit_t> *hitPointer=nullptr;
	auto myBranch=aTree.Branch(fName,&hitPointer);

	for(const auto &evt : *SimEvents){
		hitPointer=&evt.tracker.at(fName);
		myBranch->Fill();
	}
	}

void TrackerSensitiveDetector::CopyHitsToRun(simevent_t& anEvent) const {
	anEvent.tracker[GetName()]=*fHits.get();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
