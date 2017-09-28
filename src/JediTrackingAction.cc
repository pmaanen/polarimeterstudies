/*
 * JediTrackingAction.cpp
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 */

#include <DetectorConstruction.hh>
#include <JediTrackingAction.hh>
#include "JediPhysicsManager.hh"
#include "JediTrackInfo.hh"
#include <G4RunManager.hh>
#include <G4Deuteron.hh>
JediTrackingAction::JediTrackingAction(JediPhysicsManager* physicsManager):fPhysicsManager(physicsManager)
{}

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

			if ( volume && volume->GetLogicalVolume()->GetName() == "Target" )
			{
				fPhysicsManager->SetMaxIL(track->GetMomentumDirection());
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

			if ( volume && volume->GetLogicalVolume()->GetName() == "Target" )
			{
				fPhysicsManager->SetMaxIL(track->GetMomentumDirection());
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
