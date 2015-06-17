/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */


#include "G4ParticleGun.hh"
#include <PhaseSpaceGenerator.hh>

PhaseSpaceGenerator::PhaseSpaceGenerator(G4ParticleGun* gun) {

	beamEnergy=235.*CLHEP::MeV;
	Initialized=false;

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;
	DefineCommands();

	pGun=gun;
}

PhaseSpaceGenerator::~PhaseSpaceGenerator() {}

void PhaseSpaceGenerator::DefineCommands() {}
