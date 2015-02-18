/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */

#include <PhaseSpaceGenerator.hh>

G4ThreadLocal TF2* PhaseSpaceGenerator::SigmaFunc = 0;

PhaseSpaceGenerator::PhaseSpaceGenerator() {

	beamEnergy=235.*CLHEP::MeV;
	Initialized=false;

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;
	DefineCommands();

}

PhaseSpaceGenerator::~PhaseSpaceGenerator() {}

void PhaseSpaceGenerator::DefineCommands() {

	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/dcElastic/",
			"DC event generator control");

	G4GenericMessenger::Command& enCmd
	= fMessenger->DeclareMethodWithUnit("energy","MeV",
			&PhaseSpaceGenerator::setBeamEnergy,
			"Set beam energy");
	enCmd.SetParameterName("energy",false,false);
	//enCmd.SetRange("energy>50. && energy<300.");
}
