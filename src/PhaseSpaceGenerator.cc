/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */


#include "G4ParticleGun.hh"
#include <PhaseSpaceGenerator.hh>
PhaseSpaceGenerator::PhaseSpaceGenerator(G4ParticleGun* gun, G4String name):fXPrime(0),fYPrime(0),fTiltX(0),fTiltY(0),fBeamspot(0,0,0),fSpotsize(0,0,0),fName(name) {

	if(gConfig.count("generator.beam_energy")){
		fBeamEnergy=gConfig["generator.beam_energy"].as<double>()*CLHEP::MeV;
	}
	else
		fBeamEnergy=270*CLHEP::MeV;

	if(gConfig.count("generator.thetamin")){
		fThetaMin=gConfig["generator.thetamin"].as<double>()*CLHEP::deg;
	}
	else
		fThetaMin=0*CLHEP::deg;

	if(gConfig.count("generator.thetamax")){
		fThetaMax=gConfig["generator.thetamax"].as<double>()*CLHEP::deg;
	}
	else
		fThetaMax=180*CLHEP::deg;

	DefineCommands();
	fInitialized=false;
	fParticleGun=gun;
}

PhaseSpaceGenerator::~PhaseSpaceGenerator() {}

void PhaseSpaceGenerator::DefineCommands() {



	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dcelastic/", "elastic event generator control");
	fMessenger->DeclarePropertyWithUnit("beamspot","mm",PhaseSpaceGenerator::fBeamspot,"position of beam centroid.");
	fMessenger->DeclarePropertyWithUnit("beamsize","mm",PhaseSpaceGenerator::fSpotsize,"beam size.");
	fMessenger->DeclareMethod("energy", &PhaseSpaceGenerator::setBeamEnergy, "beam energy");
	fMessenger->DeclarePropertyWithUnit("thetamin","deg",PhaseSpaceGenerator::fThetaMin,"min angle");
	fMessenger->DeclarePropertyWithUnit("thetamax","deg",PhaseSpaceGenerator::fThetaMax,"max angle");




}

void PhaseSpaceGenerator::Generate(G4Event* E) {


	auto event=genevent_t(Generate());
	fParticleGun->SetParticlePosition(G4ThreeVector(event.x,event.y,event.z));
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
	}
	return;
}
