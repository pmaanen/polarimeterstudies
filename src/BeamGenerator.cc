/*
 * BeamGenerator.cc
 *
 *  Created on: 19.02.2016
 *      Author: pmaanen
 */
#include <memory>
#include <BeamGenerator.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4ParticleGun.hh>


#include "VertexGeneratorU.hh"
#include "VertexGeneratorA.hh"
#include "VertexGeneratorO.hh"
BeamGenerator::BeamGenerator(G4ParticleGun* gun):fVertexGenerator(VertexGeneratorU::GetInstance()),fParticleGun(gun) {
	fXPrime=fYPrime=0;
	if(fParticleGun)
		fEnergy=fParticleGun->GetParticleEnergy();
	else
		fEnergy=100*CLHEP::MeV;
	fPosition=fSpotsize=G4ThreeVector(0,0,0);
	fParticleGun=gun;

	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this, "/PolarimeterStudies/beam/", "beam event generator control"));

	fMessenger->DeclarePropertyWithUnit("spotsize","mm", fSpotsize, "spotsize of gun");
	fMessenger->DeclarePropertyWithUnit("position","mm", fPosition, "position of gun");
	fMessenger->DeclarePropertyWithUnit("xp","rad",fXPrime,"x prime");
	fMessenger->DeclarePropertyWithUnit("yp","rad",fYPrime,"y prime");
	auto formCmd=fMessenger->DeclareMethod("form",&BeamGenerator::SetVertexGenerator,"shape of beam");
	formCmd.SetCandidates("uniform gaus parabola");
	fMessenger->DeclareMethodWithUnit("energy","MeV",&BeamGenerator::SetEnergy,"energy of beam");

}

genevent_t BeamGenerator::Generate() {
	fVertexGenerator->setBeamsize(fSpotsize.x()/CLHEP::mm,fSpotsize.y()/CLHEP::mm,fSpotsize.z()/CLHEP::mm);
	fVertexGenerator->setBeamposition(fPosition.x()/CLHEP::mm,fPosition.y()/CLHEP::mm,fPosition.z()/CLHEP::mm);

	auto pos=fVertexGenerator->generateVertex();

	G4ThreeVector direction(0,0,1);
	if(fXPrime>0)
		direction.rotateX(G4RandGauss::shoot(0,fXPrime));
	if(fYPrime>0)
		direction.rotateY(G4RandGauss::shoot(0,fYPrime));

	genevent_t res(0,0,pos.x(),pos.y(),pos.z());
	auto id=0;
	G4double e=0;
	G4double mom=0;
	if(fParticleGun){
		mom=fParticleGun->GetParticleMomentum()/CLHEP::GeV;
		if(mom<1e-5){
			auto mass=fParticleGun->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV;
			e=fParticleGun->GetParticleEnergy()/CLHEP::GeV+mass;
			mom=sqrt(e*e-mass*mass);
		}
		id=fParticleGun->GetParticleDefinition()->GetPDGEncoding();
		e=fParticleGun->GetParticleEnergy()/CLHEP::GeV;
	}
	res.particles.push_back(particle_t(id,mom*direction.getX(),mom*direction.getY(),mom*direction.getZ(),0));
	return res;

}

void BeamGenerator::SetVertexGenerator(G4String cmd) {
	if(cmd=="uniform")
		fVertexGenerator=VertexGeneratorU::GetInstance();
	else if(cmd=="gaus")
		fVertexGenerator=VertexGeneratorO::GetInstance();
	else if (cmd=="parabola")
		fVertexGenerator=VertexGeneratorA::GetInstance();
	else
		G4Exception("BeamGenerator::SetVertexGenerator","",FatalException,"Generator name not recognized");
}

void BeamGenerator::SetEnergy(G4double energy) {
	fEnergy=energy;
	fParticleGun->SetParticleEnergy(energy);
}
