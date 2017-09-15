/*
 * CosmicMuonGenerator.cpp
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <CosmicMuonGenerator.hh>
#include <G4ParticleTable.hh>
#include <JediClasses.hh>
#include  "G4MuonMinus.hh"
#include  "G4MuonPlus.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "Analysis.hh"
#include "G4Event.hh"
#include "G4Threading.hh"
#include "G4GenericMessenger.hh"
#include "TMath.h"

using namespace CLHEP;
CosmicMuonGenerator::CosmicMuonGenerator():EventGenerator("cosmics"),fPosition(0,0,0),fSpotsize(0,0,0) {

	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this, "/PolarimeterStudies/muon/", "muon generator control"));

	fMessenger->DeclarePropertyWithUnit("spotsize","mm", fSpotsize, "spotsize of muon gun");

	fMessenger->DeclarePropertyWithUnit("position","mm", fPosition, "position of muon gun");

	fHelperFunctions=std::unique_ptr<cosmic_functions>(new cosmic_functions);

	fTheta=std::unique_ptr<TF1>(new TF1("Theta",fHelperFunctions.get(),&cosmic_functions::angle,0,90*TMath::DegToRad(),0));
	fMomentum=std::unique_ptr<TF1>(new TF1("Momentum",fHelperFunctions.get(),&cosmic_functions::momentum,0,20,0,0));
}


genvertex_t CosmicMuonGenerator::Generate() {
	auto phi=G4UniformRand()*2*CLHEP::pi*CLHEP::rad;

	G4ParticleDefinition* part=0;
	auto charge=G4UniformRand()-0.5;
	if(charge>0)
		part=G4MuonPlus::MuonPlusDefinition();
	else
		part=G4MuonMinus::MuonMinusDefinition();

	G4double momentumAmp=fMomentum->GetRandom()*CLHEP::GeV;
	G4double theta=fTheta->GetRandom(0,90*TMath::DegToRad());
	auto momentum=G4ThreeVector(momentumAmp*sin(theta)*cos(phi),momentumAmp*(-cos(theta)),momentumAmp*(sin(theta)*sin(phi)));

	auto vx=fPosition.getX()+fSpotsize.getX()*(G4UniformRand()-0.5);
	auto vy=fPosition.getY()+fSpotsize.getY()*(G4UniformRand()-0.5);
	auto vz=fPosition.getZ()+fSpotsize.getZ()*(G4UniformRand()-0.5);
	genvertex_t res(0,vx,vy,vz);
	Double_t mass=part->GetPDGMass()/CLHEP::GeV;
	Double_t e=sqrt(momentumAmp*momentumAmp+mass*mass);
	res.particles.push_back(particle_t(part->GetPDGEncoding(),momentum.getX(),momentum.getY(),momentum.getZ(),e*GeV/MeV));
	return res;
}

void CosmicMuonGenerator::Initialize() {}
