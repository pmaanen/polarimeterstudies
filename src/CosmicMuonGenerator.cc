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
CosmicMuonGenerator::CosmicMuonGenerator(G4ParticleGun* pgun) {
	angle=TF1("cos_squared","cos(x)*cos(x)",0,3.1415/2);
	energy=TF1("powerlaw","(x<3.35)*TMath::Power(3.35,-2.7)+(x>3.35)*TMath::Power(x,-2.7)",0,100);
	particle=TF1("fiftyfifty","1",-1,1);
	this->pgun=pgun;
}

CosmicMuonGenerator::~CosmicMuonGenerator() {}

void CosmicMuonGenerator::Generate(G4Event* E) {

	G4double theta=angle.GetRandom()*CLHEP::rad;
	G4double phi=G4Random::getTheGenerator()->flat()*2*CLHEP::pi;
	G4ParticleDefinition* part=0;
	if(particle.GetRandom()>0)
		part=G4MuonPlus::MuonPlusDefinition();
	else
		part=G4MuonMinus::MuonMinusDefinition();


	auto mom=energy.GetRandom()*CLHEP::GeV;

	//Analysis::Instance()->FillH2(myHistoId[0],mom,theta);
	pgun->SetParticleMomentum(mom);
	pgun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi),-cos(theta),sin(theta)*sin(phi))) ;
	pgun->SetParticleDefinition(part);
	pgun->GeneratePrimaryVertex(E);

}

void CosmicMuonGenerator::beginOfRun() {
	myTupleId.clear();
	myHistoId.clear();
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
	myHistoId.push_back(an->CreateH2("thetap","Theta vs p",1000,0,10,90,0,90,"GeV","deg"));
}
