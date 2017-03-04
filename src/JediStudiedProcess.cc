/*
 * JediStudiesProcess.cc
 *
 *  Created on: 17.01.2017
 *      Author: pmaanen
 */

#include <JediStudiedProcess.hh>
#include "JediTrackInfo.hh"
#include <limits>
#include <G4Track.hh>
#include <G4Deuteron.hh>
#include <JediCommon.hh>
JediStudiedProcess::JediStudiedProcess():G4WrapperProcess("dcelastic-faststim", fUserDefined) {}

G4double JediStudiedProcess::PostStepGetPhysicalInteractionLength(
		const G4Track& track, G4double , G4ForceCondition* condition) {

	*condition = NotForced;

	JediTrackInfo *  theTrackInfo( static_cast< JediTrackInfo * >(
			track.GetUserInformation() ) );

	if ( ! theTrackInfo ||
			*track.GetDefinition()!=*G4Deuteron::Deuteron() ){
		if(gVerbose>3)
			G4cout<<"JediStudiedProcess::PostStepGetPhysicalInteractionLength: "
			<<theTrackInfo<<" "
			<<track.GetDefinition()->GetParticleName()
			<<"!="
			<<G4Deuteron::Deuteron()
		<<G4endl;
		return DBL_MAX;
	}
	if ( ! theTrackInfo->IsStudiedProcessActivated() )
		return DBL_MAX;

	if(! theTrackInfo->IsStudiedProcessAllowed())
		return DBL_MAX;

	//elastic model vomits when ke is too low.
	if(track.GetKineticEnergy()<30*CLHEP::MeV || track.GetKineticEnergy()>300*CLHEP::MeV)
		return DBL_MAX;

	if(gVerbose>3)
		G4cout<<"JediStudiedProcess::PostStepGetPhysicalInteractionLength: "
		<<theTrackInfo->GetFinalTrackLengthInTarget()-theTrackInfo->GetCurrentTrackLengthInTarget()
		<<G4endl;
	return theTrackInfo->GetFinalTrackLengthInTarget() -
			theTrackInfo->GetCurrentTrackLengthInTarget();
}

G4VParticleChange* JediStudiedProcess::PostStepDoIt(const G4Track& track,
		const G4Step& step) {

	G4VParticleChange *  particleChange( pRegProcess->PostStepDoIt( track,step ) );

	auto theTrackInfo=dynamic_cast<JediTrackInfo*>(track.GetUserInformation());
	theTrackInfo->AllowStudiedProcess(false);
	if(gVerbose>2)
		G4cout<<"JediStudiedProcess::PostStepDoIt "
		<<track.GetPosition()<<" "
		<<track.GetMomentumDirection()<<" "
		<<track.GetKineticEnergy()/CLHEP::MeV
		<<G4endl;
	return particleChange;
}
