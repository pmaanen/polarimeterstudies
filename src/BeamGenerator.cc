/*
 * BeamGenerator.cc
 *
 *  Created on: 19.02.2016
 *      Author: pmaanen
 */

#include <BeamGenerator.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4ParticleGun.hh>

BeamGenerator::BeamGenerator(G4ParticleGun* gun) {
	fXPrime=fYPrime=0;
	fPosition=fSpotsize=G4ThreeVector(0,0,0);
	fParticleGun=gun;

	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/beam/", "beam event generator control");

	fMessenger->DeclarePropertyWithUnit("spotsize","mm", fSpotsize, "spotsize of gun");
	fMessenger->DeclarePropertyWithUnit("position","mm", fPosition, "position of gun");
	fMessenger->DeclarePropertyWithUnit("xp","rad",fXPrime,"x prime");
	fMessenger->DeclarePropertyWithUnit("yp","rad",fYPrime,"y prime");

}

BeamGenerator::~BeamGenerator() {
	delete fMessenger;
}

void BeamGenerator::Generate(G4Event* E) {
	auto event=Generate();
	fParticleGun->SetParticlePosition(G4ThreeVector(event.x,event.y,event.z));
	auto part=event.particles[0];
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(part.px,part.py,part.pz));
	fParticleGun->GeneratePrimaryVertex(E);
}

genevent_t BeamGenerator::Generate() {
	auto position=fParticleGun->GetParticlePosition();
	G4double x=position.x(),y=position.y(),z=position.z();
	if(fSpotsize.x()>0)
		x+=G4RandGauss::shoot(0,fSpotsize.x());
	if(fSpotsize.y()>0)
		y+=G4RandGauss::shoot(0,fSpotsize.y());
	if(fSpotsize.z()>0)
		z+=G4RandGauss::shoot(0,fSpotsize.z());
	G4ThreeVector direction(0,0,1);
	if(fXPrime>0)
		direction.rotateX(G4RandGauss::shoot(0,fXPrime));
	if(fYPrime>0)
		direction.rotateY(G4RandGauss::shoot(0,fYPrime));

	genevent_t res(0,0,x/CLHEP::mm,y/CLHEP::mm,z/CLHEP::mm);
	auto id=0;
	G4double e=0;
	G4double mom=0;
	if(fParticleGun){
		mom=fParticleGun->GetParticleMomentum()/CLHEP::GeV;
		if(mom<1e-5){
			auto mass=fParticleGun->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV;
			auto e=fParticleGun->GetParticleEnergy()/CLHEP::GeV+mass;
			mom=sqrt(e*e-mass*mass);
		}
		id=fParticleGun->GetParticleDefinition()->GetPDGEncoding();
		e=fParticleGun->GetParticleEnergy()/CLHEP::GeV;
	}
	res.particles.push_back(particle_t(id,mom*direction.getX(),mom*direction.getY(),mom*direction.getZ(),0));
	return res;

}
