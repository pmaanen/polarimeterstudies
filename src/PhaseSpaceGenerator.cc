/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */


#include "G4ParticleGun.hh"
#include <PhaseSpaceGenerator.hh>

G4ThreadLocal TF2* PhaseSpaceGenerator::SigmaFunc = 0;

PhaseSpaceGenerator::PhaseSpaceGenerator(G4ParticleGun* gun) {

	beamEnergy=235.*CLHEP::MeV;
	Initialized=false;

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;
	DefineCommands();

	pGun=gun;
	Analysis* an=Analysis::Instance();
	myTupleId.push_back(an->CreateNtuple("MCTruth","MCTruth"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"event"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"pid"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"px"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"py"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"pz"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vx"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vy"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vz"));
	an->FinishNtuple(myTupleId[0]);
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
