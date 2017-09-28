/*
 * JediPhysicsManager.cc
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 */

#include "JediPhysicsManager.hh"
#include "JediPolarimeter.hh"
#include "JediTrackInfo.hh"
#include <G4GenericMessenger.hh>
#include <G4Track.hh>
#include <G4StepPoint.hh>
#include <G4VSolid.hh>

#include <Randomize.hh>
#include <limits>
#include <algorithm>
JediPhysicsManager::JediPhysicsManager():basicMaxIL( DBL_MAX ),
maxILCorrection( 0 ), proposedMaxIL( DBL_MAX ),
numberOfTriggeredStudiedInteractions( 0 ),
onlyBeamParticleCanTriggerStudiedProcess( false ), targetSolid( nullptr ) {
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(
			this,"/PolarimeterStudies/biasing/",""));
}

void JediPhysicsManager::ResampleTrackLengthInTarget(const G4Track* track,
		const G4StepPoint* stepPoint) {
	/* BEWARE: all callers must ensure that:
	 * 1) track (or stepPoint if not NULL) is inside target volume:
	 *    in this case we can use already calculated targetTransform
	 * 2) track info object is of type CexmcIncidentParticleTrackInfo*:
	 *    in this case we can use static_cast<> for trackInfo */
	JediTrackInfo *  trackInfo(
			static_cast< JediTrackInfo * >(
					track->GetUserInformation() ) );

	if ( ! trackInfo )
		return;

	if(!targetSolid)
		return;

	G4ThreeVector  position;
	G4ThreeVector  direction;

	if ( stepPoint )
	{
		position = targetTransform.TransformPoint( stepPoint->GetPosition() );
		direction = targetTransform.TransformAxis(
				stepPoint->GetMomentumDirection() );
	}
	else
	{
		position = targetTransform.TransformPoint( track->GetPosition() );
		direction = targetTransform.TransformAxis(
				track->GetMomentumDirection() );
	}

	G4double  distanceInTarget( targetSolid->DistanceToOut( position,
			direction ) );

	trackInfo->ResetCurrentTrackLengthInTarget();
	trackInfo->SetFinalTrackLengthInTarget( G4UniformRand() *
			std::max( distanceInTarget, proposedMaxIL ) );

	trackInfo->SetNeedsTrackLengthResampling( false );
}

void JediPhysicsManager::GeometryHasChanged(const JediPolarimeter* setup) {
	if(!setup->GetTarget()){
		G4Exception("JediPhysicsManager::GeometryHasChanged","",JustWarning,"Target solid not found. Setting interaction length to infinite.");
		proposedMaxIL=DBL_MAX;
		basicMaxIL=DBL_MAX;
		return;
	}
	targetSolid = setup->GetTarget()->GetSolid();
	targetTransform = setup->GetTargetTransform().Inverse();
	if(JediConfigurationManager::Instance()->GetVerbose()>3)
		G4cout<<"JediPhysicsManager::GeometryHasChanged "
		<<targetSolid<<" "
		<<targetSolid->GetName()
		<<G4endl;
	SetMaxIL( G4ThreeVector(0,0,1) );
}

void JediPhysicsManager::CalculateBasicMaxIL(const G4ThreeVector& direction) {
	/* basicMaxIL is double distance from the point (0, 0, 0) to the edge of the
	 * target solid along the specified direction */
	basicMaxIL = targetSolid->DistanceToOut( G4ThreeVector(),
			targetTransform.TransformAxis( direction ) ) * 2;
}
