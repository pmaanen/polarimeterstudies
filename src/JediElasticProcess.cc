/*
 * JediElasticScatteringProcess.cc
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#include <JediElasticProcess.hh>
#include <JediElasticModel.hh>
#include <G4ParticleChange.hh>
#include <G4ParticleDefinition.hh>
#include <G4HadronicInteraction.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4Element.hh>
#include <G4StableIsotopes.hh>
#include <G4IonTable.hh>
#include <G4TrackStatus.hh>
#include <TGenPhaseSpace.h>
#include <TLorentzVector.h>
#include "global.hh"
#include "G4Threading.hh"
JediElasticProcess::JediElasticProcess():G4HadronicProcess( "dcelastic" ), fModel( nullptr ), theTotalResult( nullptr ) {
	theTotalResult = new G4ParticleChange();
	if(!G4Threading::IsWorkerThread())
		G4cout<<"JediElasticProcess=x:y:z:px:py:pz:ekin:theta:phi"<<G4endl;
}

JediElasticProcess::~JediElasticProcess() {
	delete theTotalResult;
}

G4VParticleChange* JediElasticProcess::PostStepDoIt(const G4Track& track,
		const G4Step&) {

	G4TrackStatus  trackStatus( track.GetTrackStatus() );

	if ( trackStatus != fAlive && trackStatus != fSuspend )
	{
		theTotalResult->Clear();
		theTotalResult->Initialize( track );

		return theTotalResult;
	}
	targetNucleus.SetParameters( 12, 6 );
	G4HadProjectile    projectile( track );
	fModel->setBeamPolarization(track.GetPolarization().y());
	G4HadFinalState *  result( fModel->ApplyYourself( projectile,
			targetNucleus ) );


	result->SetTrafoToLab(projectile.GetTrafoToLab());
	FillTotalResult( result, track );

	return theTotalResult;
}

G4bool JediElasticProcess::IsApplicable(const G4ParticleDefinition& particle) {
	G4bool res=false;
	if(particle.GetPDGEncoding()==G4Deuteron::DeuteronDefinition()->GetPDGEncoding())
		res=true;
	return res;
}

void JediElasticProcess::RegisterModel(JediElasticModel* model) {
	fModel = model;
	//interaction = dynamic_cast< G4HadronicInteraction * >( fModel );
	/*
		if ( ! interaction )
			throw JediException( IncompatibleProductionModel );
	 */
	G4HadronicProcess::RegisterMe( dynamic_cast< G4HadronicInteraction * >( fModel ) );
}

void JediElasticProcess::CalculateTargetNucleus(const G4Material* material) {
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

void JediElasticProcess::FillTotalResult(G4HadFinalState* hadFinalState,
		const G4Track& track) {

	G4int  numberOfSecondaries( hadFinalState->GetNumberOfSecondaries() );
	G4ThreeVector it(0.,0.,1.);
	theTotalResult->Clear();
	theTotalResult->Initialize( track );
	theTotalResult->SetSecondaryWeightByProcess( true );
	theTotalResult->ProposeLocalEnergyDeposit(
			hadFinalState->GetLocalEnergyDeposit() );
	theTotalResult->SetNumberOfSecondaries( numberOfSecondaries );
	theTotalResult->ProposeTrackStatus( fAlive );
	if ( hadFinalState->GetStatusChange() == stopAndKill )
		theTotalResult->ProposeTrackStatus( fStopAndKill );

	G4double ekin= hadFinalState->GetEnergyChange();
	G4double mass = track.GetParticleDefinition()->GetPDGMass();
	G4double etot = ekin + mass;
	G4double newP = std::sqrt(ekin*(ekin + 2*mass));
	G4ThreeVector newPV = newP*hadFinalState->GetMomentumChange();
	G4LorentzVector newP4(etot, newPV);
	newP4 *= hadFinalState->GetTrafoToLab();
	theTotalResult->ProposeMomentumDirection(newP4.vect().unit());
	ekin = newP4.e() - mass;
	if(ekin<0.0)
		ekin=0.0;
	theTotalResult->ProposeEnergy(ekin);
	theTotalResult->ProposeMomentumDirection(newP4.vect().unit());

	if(gVerbose>2) {
		G4cout <<"JediElasticProcess="
				<<track.GetPosition().x()/CLHEP::mm<<" "
				<<track.GetPosition().y()/CLHEP::mm<<" "
				<<track.GetPosition().z()/CLHEP::mm<<" "
				<<track.GetMomentum().x()/CLHEP::GeV<<" "
				<<track.GetMomentum().y()/CLHEP::GeV<<" "
				<<track.GetMomentum().z()/CLHEP::GeV<<" "
				<<theTotalResult->GetEnergy()
				<< " " << theTotalResult->GetMomentumDirection()->theta()/CLHEP::deg
				<< " " << theTotalResult->GetMomentumDirection()->phi()/CLHEP::deg
				<< G4endl;
	}

	for ( G4int  i( 0 ); i < numberOfSecondaries; ++i )
	{
		if(!hadFinalState->GetSecondary(i))
			G4Exception("JediElasticProcess::FillTotalResult","",FatalException,"Number of secondaries not correct!");
		G4double   time( hadFinalState->GetSecondary( i )->GetTime() );
		if ( time < 0 )
			time = track.GetGlobalTime();

		G4LorentzVector secMom=hadFinalState->GetSecondary(i)->GetParticle()->Get4Momentum();
		secMom*=hadFinalState->GetTrafoToLab();
		hadFinalState->GetSecondary(i)->GetParticle()->Set4Momentum(secMom);

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
