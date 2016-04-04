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

	fFunctions=new function_helper;
	fAngle=new TF1("cos_squared",fFunctions,&function_helper::angle,0,3.1415,0,"function_helper","angle");
	fMomentumAmp=new TF1("energy",fFunctions,&function_helper::energy,0,20,0,"function_helper","energy");
	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/muon/", "muon generator control");

	fMessenger->DeclarePropertyWithUnit("spotsize","mm", fSpotsize, "spotsize of muon gun");

	fMessenger->DeclarePropertyWithUnit("position","mm", fPosition, "position of muon gun");
}

CosmicMuonGenerator::~CosmicMuonGenerator() {
	delete fMessenger;
}

void CosmicMuonGenerator::Generate(G4Event* E) {
	if(!fRunInitialized)
		Initialize();
	auto event=Generate();
	auto muon=event.particles[0];
	auto momentum=G4ThreeVector(muon.px,muon.py,muon.pz);

	fParticleGun->SetParticlePosition(G4ThreeVector(event.x,event.y,event.z));

	Analysis* an=Analysis::Instance();
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
			if(fAngle->Eval(theta)>fAngle->GetMaximum(0,CLHEP::pi/2)*G4UniformRand())
				break;
		}
		phi=G4UniformRand()*2*CLHEP::pi;
		auto charge=G4UniformRand()-0.5;
		if(charge>0)
			part=G4MuonPlus::MuonPlusDefinition();
		else
			part=G4MuonMinus::MuonMinusDefinition();
		while(1){
			mom=G4UniformRand()*20;
			if(fMomentumAmp->Eval(mom)>fMomentumAmp->GetMaximum(0,20)*G4UniformRand())
				break;
		}
		mom*=CLHEP::GeV;
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

void CosmicMuonGenerator::Initialize() {
	fTupleId.clear();
	Analysis* an=Analysis::Instance();
	fRunInitialized=true;
}
