/*
 * CosmicMuonGenerator.cpp
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <CosmicMuonGenerator.hh>
#include <G4ParticleTable.hh>
#include  "G4MuonMinus.hh"
#include  "G4MuonPlus.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "Analysis.hh"
#include "G4Event.hh"
#include "G4Threading.hh"
#include "G4GenericMessenger.hh"
#include "hit.hh"
CosmicMuonGenerator::CosmicMuonGenerator(G4ParticleGun* pgun):EventGenerator(pgun),fPosition(0,0,0),fSpotsize(0,0,0) {

	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this, "/PolarimeterStudies/muon/", "muon generator control"));

	fMessenger->DeclarePropertyWithUnit("spotsize","mm", fSpotsize, "spotsize of muon gun");

	fMessenger->DeclarePropertyWithUnit("position","mm", fPosition, "position of muon gun");
}

CosmicMuonGenerator::~CosmicMuonGenerator() {}

void CosmicMuonGenerator::Generate(G4Event* E) {
	if(!fRunInitialized)
		Initialize();
	auto event=Generate();
	auto muon=event.particles[0];
	auto momentum=G4ThreeVector(muon.px,muon.py,muon.pz);

	fParticleGun->SetParticlePosition(G4ThreeVector(event.x,event.y,event.z));

	fParticleGun->SetParticleMomentum(momentum) ;
	fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(muon.id));
	fParticleGun->GeneratePrimaryVertex(E);

}

genevent_t CosmicMuonGenerator::Generate() {
	G4double yMom=1;
	G4double mom=0;
	G4ThreeVector momentum;
	G4double theta=0;
	G4double phi=0;
	G4ParticleDefinition* part=0;
	while(yMom>0){
		while(1){
			theta=G4UniformRand()*CLHEP::pi/2;
			auto acc=cosmic_functions::angle(0)*G4UniformRand();
			if(cosmic_functions::angle(theta)>acc)
				break;
		}
		phi=G4UniformRand()*2*CLHEP::pi*CLHEP::rad;
		auto charge=G4UniformRand()-0.5;
		if(charge>0)
			part=G4MuonPlus::MuonPlusDefinition();
		else
			part=G4MuonMinus::MuonMinusDefinition();
		while(1){
			mom=G4UniformRand()*20*CLHEP::GeV;
			auto acc=cosmic_functions::momentum(0)*G4UniformRand();
			if(cosmic_functions::momentum(mom)>acc)
				break;
		}
		momentum=G4ThreeVector(mom*sin(theta)*cos(phi),mom*(-cos(theta)),mom*(sin(theta)*sin(phi)));
		yMom=momentum.getY();
	}
	auto vx=fPosition.getX()+fSpotsize.getX()*(G4UniformRand()-0.5);
	auto vy=fPosition.getY()+fSpotsize.getY()*(G4UniformRand()-0.5);
	auto vz=fPosition.getZ()+fSpotsize.getZ()*(G4UniformRand()-0.5);
	genevent_t res(0,0,vx,vy,vz);
	Double_t mass=part->GetPDGMass()/CLHEP::GeV;
	Double_t e=sqrt(mom*mom+mass*mass);
	res.particles.push_back(particle_t(part->GetPDGEncoding(),momentum.getX(),momentum.getY(),momentum.getZ(),e));
	return res;
}

void CosmicMuonGenerator::Initialize() {}
