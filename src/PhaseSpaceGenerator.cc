/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */


#include "G4ParticleGun.hh"
#include <PhaseSpaceGenerator.hh>
PhaseSpaceGenerator::PhaseSpaceGenerator(G4String name):EventGenerator(name),fXPrime(0),fYPrime(0),fTiltX(0),fTiltY(0),fBeamposition(0,0,0),fBeamsize(0,0,0),fName(name) {

	if(gConfig.count("generator.beam_energy")){
		fBeamEnergy=gConfig["generator.beam_energy"].as<double>()*CLHEP::MeV;
	}
	else
		fBeamEnergy=270*CLHEP::MeV;

	if(gConfig.count("generator.thetamin")){
		fThetaMin=gConfig["generator.thetamin"].as<double>()*CLHEP::deg;
	}
	else
		fThetaMin=3*CLHEP::deg;

	if(gConfig.count("generator.thetamax")){
		fThetaMax=gConfig["generator.thetamax"].as<double>()*CLHEP::deg;
	}
	else
		fThetaMax=30*CLHEP::deg;

	DefineCommands();
	fInitialized=false;
}

void PhaseSpaceGenerator::DefineCommands() {

	G4String dir=G4String("/PolarimeterStudies/")+G4String(fName)+G4String("/");
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this, dir, "elastic event generator control"));
	fMessenger->DeclareMethod("beamposition",&PhaseSpaceGenerator::setBeamposition,"position of beam centroid.");
	fMessenger->DeclareMethod("beamsize",&PhaseSpaceGenerator::setBeamsize,"beam size.");
	fMessenger->DeclareMethod("energy", &PhaseSpaceGenerator::setBeamEnergy, "beam energy");
	fMessenger->DeclareMethodWithUnit("thetamin","deg",&PhaseSpaceGenerator::setThetaMin,"min angle");
	fMessenger->DeclareMethodWithUnit("thetamax","deg",&PhaseSpaceGenerator::setThetaMin,"max angle");

	return;
}
