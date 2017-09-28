/*
 * JediBreakupProcess.cc
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#include <JediBreakupProcess.hh>
#include <JediBreakupModel.hh>
#include <G4ParticleChange.hh>
#include <G4ParticleDefinition.hh>
#include <G4HadronicInteraction.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4Element.hh>
#include <G4StableIsotopes.hh>
#include <G4TrackStatus.hh>


JediBreakupProcess::JediBreakupProcess():G4HadronicProcess( "dcbreakup" ), fModel( nullptr ),theTotalResult( nullptr ), isInitialized( false ) {

	theTotalResult = new G4ParticleChange();
}

JediBreakupProcess::~JediBreakupProcess() {
	delete theTotalResult;
}

G4VParticleChange* JediBreakupProcess::PostStepDoIt(const G4Track& track,
		const G4Step&) {
	//G4cout<<"JediBreakupProcess::PostStepDoIt\n";


	theTotalResult->Clear();
	G4TrackStatus  trackStatus( track.GetTrackStatus() );
	if ( trackStatus != fAlive && trackStatus != fSuspend )
	{
		theTotalResult->Clear();
		theTotalResult->Initialize( track );
		return theTotalResult;
	}

	if(track.GetTouchable()->GetVolume()->GetName()!="Target")	{
		theTotalResult->Clear();
		theTotalResult->Initialize( track );
		return theTotalResult;
	}

	if ( ! isInitialized )
	{
		CalculateTargetNucleus( track.GetMaterial() );
		isInitialized = true;
	}

	G4HadProjectile    projectile( track );
	G4HadFinalState *  result( fModel->ApplyYourself( projectile,
			targetNucleus ) );
	FillTotalResult( result, track );

	return theTotalResult;
}

G4bool JediBreakupProcess::IsApplicable(const G4ParticleDefinition& particle) {

	G4bool res=false;
	if(particle.GetPDGEncoding()==G4Deuteron::DeuteronDefinition()->GetPDGEncoding())
		res=true;
	return res;
}

void JediBreakupProcess::RegisterModel(JediBreakupModel* model) {


	fModel = model;
	//interaction = dynamic_cast< G4HadronicInteraction * >( fModel );
	/*
	if ( ! interaction )
		throw JediException( IncompatibleProductionModel );
	 */
	G4HadronicProcess::RegisterMe( dynamic_cast< G4HadronicInteraction * >( fModel ) );
}

void JediBreakupProcess::CalculateTargetNucleus(const G4Material* material) {
	G4int  numberOfElements( material->GetNumberOfElements() );
	if ( numberOfElements > 1 )
	{
		G4cout <<"WARNING: Number of elements in target "
				"material is more than 1.\n              Only the first "
				"element will be chosen for target nucleus" << G4endl;
	}

	const G4Element *  element( material->GetElement( 0 ) );
	G4double           ZZ( element->GetZ() );
	G4int              Z( G4int( ZZ + 0.5 ) );

	G4StableIsotopes  stableIsotopes;
	G4int             index( stableIsotopes.GetFirstIsotope( Z ) );
	G4double          AA( stableIsotopes.GetIsotopeNucleonCount( index ) );

	targetNucleus.SetParameters( AA, ZZ );


}

void JediBreakupProcess::FillTotalResult(G4HadFinalState* hadFinalState,
		const G4Track& track) {

	theTotalResult->Clear();
	G4int  numberOfSecondaries( hadFinalState->GetNumberOfSecondaries() );

	theTotalResult->Initialize( track );
	theTotalResult->SetSecondaryWeightByProcess( true );
	theTotalResult->SetNumberOfSecondaries( numberOfSecondaries );
	theTotalResult->ProposeTrackStatus( fStopAndKill );
	for ( G4int  i( 0 ); i < 2; ++i )
	{
		G4double   time( hadFinalState->GetSecondary( i )->GetTime() );
		if ( time < 0 )
			time = track.GetGlobalTime();

		G4Track *  newTrack( new G4Track(
				hadFinalState->GetSecondary( i )->GetParticle(),
				time, track.GetPosition() ) );

		G4double   newWeight( track.GetWeight() *
				hadFinalState->GetSecondary( i )->GetWeight() );
		newTrack->SetWeight( newWeight );
		newTrack->SetTouchableHandle( track.GetTouchableHandle() );
		theTotalResult->AddSecondary( newTrack );
	}
	hadFinalState->Clear();


}
