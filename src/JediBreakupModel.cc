/*
 * DeuteronCarbonBreakupModel.cc
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#include <JediBreakupModel.hh>
#include <cmath>
JediBreakupModel::JediBreakupModel() : G4HadronicInteraction( "dcbreakup" ) {
	// TODO Auto-generated constructor stub

}

JediBreakupModel::~JediBreakupModel() {
	// TODO Auto-generated destructor stub
}

G4HadFinalState* JediBreakupModel::ApplyYourself(const G4HadProjectile& aTrack,
		G4Nucleus& targetNucleus) {



	theParticleChange.Clear();
	theParticleChange.ClearSecondaries();
	theParticleChange.SetStatusChange( stopAndKill );
	theParticleChange.SetEnergyChange( 0.0 );

	auto dmom=aTrack.Get4Momentum();

	auto pmom=dmom.vect().mag()/2;
	auto pmass=G4Proton::ProtonDefinition()->GetPDGMass();
	auto ep=sqrt(pmom*pmom+pmass*pmass);

	G4double phi=2*CLHEP::pi*G4UniformRand()*CLHEP::rad;
	G4LorentzVector pMom(0,0,pmom,ep);
	pMom.setTheta(CLHEP::pi*G4UniformRand()*CLHEP::rad);
	pMom.setPhi(phi);
	G4LorentzVector nMom=dmom-pMom;
	G4DynamicParticle *  secProton( new G4DynamicParticle(
			G4Proton::ProtonDefinition(),
			pMom) );
	theParticleChange.AddSecondary( secProton );
	G4DynamicParticle *  secNeutron( new G4DynamicParticle(
			G4Neutron::NeutronDefinition(),
			nMom) );
	theParticleChange.AddSecondary( secNeutron );
	return &theParticleChange;
}
