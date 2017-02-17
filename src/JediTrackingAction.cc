/*
 * JediTrackingAction.cpp
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 */

#include <DetectorConstructionFactory.hh>
#include <JediTrackingAction.hh>
#include "JediPhysicsManager.hh"
#include "JediTrackInfo.hh"
#include <G4RunManager.hh>
#include <G4Deuteron.hh>
JediTrackingAction::JediTrackingAction(std::shared_ptr<JediPhysicsManager> physicsManager):targetVolume( nullptr ), fPhysicsManager(physicsManager)
{
	G4RunManager *      runManager( G4RunManager::GetRunManager() );
	const JediPolarimeter *  setup( static_cast< const JediPolarimeter * >(
			runManager->GetUserDetectorConstruction() ) );
	if(setup && setup->GetTarget())
		targetVolume = setup->GetTarget();
	else
		G4Exception("JediSteppingAction::JediSteppingAction","",FatalException,"Detector Construction not found.");
}

void JediTrackingAction::PreUserTrackingAction(const G4Track* track) {

	JediTrackInfo *  trackInfo( static_cast< JediTrackInfo * >(
			track->GetUserInformation() ) );

	if ( trackInfo )
		return;

	G4Track *  theTrack( const_cast< G4Track * >( track ) );


	if ( *track->GetDefinition() == *G4Deuteron::Deuteron() )
	{
		if ( track->GetParentID() == 0 )
		{
			trackInfo = new JediTrackInfo();
			theTrack->SetUserInformation( trackInfo );
			G4VPhysicalVolume *  volume( track->GetVolume() );

			if ( volume && volume->GetLogicalVolume() == targetVolume )
			{
				fPhysicsManager->ResampleTrackLengthInTarget( track );
				trackInfo->ActivateStudiedProcess();
			}
			return;
		}
		else if ( false /* physicsManager->OnlyBeamParticleCanTriggerStudiedProcess() */ )
			return;
		else{
			trackInfo = new JediTrackInfo();
			theTrack->SetUserInformation( trackInfo );
			G4VPhysicalVolume *  volume( track->GetVolume() );

			if ( volume && volume->GetLogicalVolume() == targetVolume )
			{
				fPhysicsManager->ResampleTrackLengthInTarget( track );
				trackInfo->ActivateStudiedProcess();
			}
			return;
		}
	}

	if ( ! trackInfo )
		return;

	if ( ! track->GetUserInformation() )
		theTrack->SetUserInformation( trackInfo );

}
