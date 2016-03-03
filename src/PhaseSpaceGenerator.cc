/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */


#include "G4ParticleGun.hh"
#include <PhaseSpaceGenerator.hh>

PhaseSpaceGenerator::PhaseSpaceGenerator(G4ParticleGun* gun):fXPrime(0),fYPrime(0),fTiltX(0),fTiltY(0),fBeamspot(0,0,0),fSpotsize(0,0,0) {

	fBeamEnergy=235.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=5*CLHEP::deg;
	fThetaMax=20*CLHEP::deg;
	DefineCommands();

	fParticleGun=gun;
}

PhaseSpaceGenerator::~PhaseSpaceGenerator() {}

void PhaseSpaceGenerator::DefineCommands() {}

void PhaseSpaceGenerator::Generate(G4Event* E) {


	auto event=PrimaryEvent(Generate());
	fParticleGun->SetParticlePosition(G4ThreeVector(event.vx,event.vy,event.vz));
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
	}
	return;
}
