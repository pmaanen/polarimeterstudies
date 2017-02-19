/*
 * JediSteppingAction.cc
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 */

#include <DetectorConstructionFactory.hh>
#include <JediSteppingAction.hh>
#include "JediTrackInfo.hh"
#include "JediPhysicsManager.hh"
#include <G4RunManager.hh>
#include "global.hh"
#include "SingleCrystal.hh"
JediSteppingAction::JediSteppingAction(std::shared_ptr<JediPhysicsManager> physicsManager):fPhysicsManager(physicsManager) {}

void JediSteppingAction::UserSteppingAction(const G4Step* step) {
	G4Track *         track( step->GetTrack() );
	JediTrackInfo *  trackInfo( static_cast< JediTrackInfo * >(
			track->GetUserInformation() ) );

	G4StepPoint *         postStepPoint( step->GetPostStepPoint() );
	G4StepStatus          stepStatus( postStepPoint->GetStepStatus() );
	const G4VTouchable *  touchable( postStepPoint->GetTouchable() );
	G4VPhysicalVolume *   volume( touchable->GetVolume() );

	if(!trackInfo)
		return;

	if ( volume && volume->GetLogicalVolume()->GetName() == "Target" )
	{
		if ( ! trackInfo->IsStudiedProcessActivated() )
		{
			if(gVerbose>3)
				G4cout<<"JediSteppingAction::UserSteppingAction "
				<<"entering "
				<<volume->GetLogicalVolume()->GetName()
				<<": Activating process"
				<<G4endl;
			fPhysicsManager->SetMaxIL(track->GetMomentumDirection());
			fPhysicsManager->ResampleTrackLengthInTarget( track, postStepPoint );
			trackInfo->ActivateStudiedProcess();
		}

		if ( stepStatus != fGeomBoundary )
		{
			if ( trackInfo->NeedsTrackLengthResampling() ){
				fPhysicsManager->SetMaxIL(track->GetMomentumDirection());
				fPhysicsManager->ResampleTrackLengthInTarget(
						track, postStepPoint );
			}
			else{
				trackInfo->AddTrackLengthInTarget( step->GetStepLength() );
				if(gVerbose>3)
					G4cout<<"JediSteppingAction::UserSteppingAction "
					<<"Adding "<< step->GetStepLength()/CLHEP::mm
					<<" to TrackLengthInTarget"
					<<G4endl;
			}
		}
	}

	G4StepPoint *  preStepPoint( step->GetPreStepPoint() );
	touchable = preStepPoint->GetTouchable();
	volume = touchable->GetVolume();

	if ( volume && volume->GetLogicalVolume()->GetName() == "Target" )
	{
		if ( stepStatus == fGeomBoundary ){
			if(gVerbose>3)
				G4cout<<"JediSteppingAction::UserSteppingAction "
				<<"leaving "
				<<volume->GetLogicalVolume()->GetName()
				<<": Deactivating process"
				<<G4endl;
			trackInfo->ActivateStudiedProcess( false );
		}
	}
}
