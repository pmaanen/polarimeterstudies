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
	fGun=gun;

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
	fGun->SetParticlePosition(G4ThreeVector(event.vx,event.vy,event.vz));
	auto part=event.particles[0];
	fGun->SetParticleMomentumDirection(G4ThreeVector(part.px,part.py,part.pz));
	fGun->GeneratePrimaryVertex(E);
}

PrimaryEvent BeamGenerator::Generate() {
	G4double x=fPosition.x(),y=fPosition.y(),z=fPosition.z();
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


	PrimaryEvent res(0,x,y,z);
	res.particles.push_back(PrimaryParticle(0,direction.getX(),direction.getY(),direction.getZ()));
	return res;

}
