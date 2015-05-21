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
#include "G4Threading.hh"
CosmicMuonGenerator::CosmicMuonGenerator(G4ParticleGun* pgun):EventGenerator(pgun) {

	functions.Put(new function_helper);
	angle.Put(new TF1("cos_squared",functions.Get(),&CosmicMuonGenerator::function_helper::angle,0,3.1415,0,"function_helper","angle"));
	momentumAmp.Put(new TF1("energy",functions.Get(),&CosmicMuonGenerator::function_helper::energy,0,20,0,"function_helper","energy"));

	position=pgun->GetParticlePosition();
}

CosmicMuonGenerator::~CosmicMuonGenerator() {}

void CosmicMuonGenerator::Generate(G4Event* E) {
	if(!runInitialized)
		Initialize();
	auto muon=Generate()[0];
	auto momentum=G4ThreeVector(muon.px,muon.py,muon.pz);


	auto vx=position.getX()+spotsize.getX()*(G4UniformRand()-0.5);
	auto vy=position.getY()+spotsize.getY()*(G4UniformRand()-0.5);
	auto vz=position.getZ()+spotsize.getZ()*(G4UniformRand()-0.5);
	pGun->SetParticlePosition(G4ThreeVector(vx,vy,vz));

	Analysis* an=Analysis::Instance();
	an->FillH2(myHistoId[0],momentum.mag()/CLHEP::MeV,momentum.angle(G4ThreeVector(0,-1,0))/CLHEP::deg);
	an->FillNtupleIColumn(myTupleId[0],myTupleId[1],E->GetEventID());
	an->FillNtupleIColumn(myTupleId[0],myTupleId[2],muon.id);
	an->FillNtupleFColumn(myTupleId[0],myTupleId[3],momentum.getX());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[4],momentum.getY());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[5],momentum.getZ());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[6],pGun->GetParticlePosition().getX()/CLHEP::mm);
	an->FillNtupleFColumn(myTupleId[0],myTupleId[7],pGun->GetParticlePosition().getY()/CLHEP::mm);
	an->FillNtupleFColumn(myTupleId[0],myTupleId[8],pGun->GetParticlePosition().getZ()/CLHEP::mm);
	an->AddNtupleRow(myTupleId[0]);

	pGun->SetParticleMomentum(momentum) ;
	pGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(muon.id));
	pGun->GeneratePrimaryVertex(E);

}

PrimaryEvent CosmicMuonGenerator::Generate() {
	G4double yMom=1;
	G4double mom=0;
	G4ThreeVector momentum;
	G4double theta=0;
	G4double phi=0;
	G4ParticleDefinition* part=0;
	while(yMom>0){
		while(1){
			theta=G4UniformRand()*CLHEP::pi/4;
			if(angle.Get()->Eval(theta)>angle.Get()->GetMaximum(0,CLHEP::pi/2)*G4UniformRand())
				break;
		}
		phi=G4UniformRand()*2*CLHEP::pi;
		auto charge=G4UniformRand()-0.5;
		if(charge>0)
			part=G4MuonPlus::MuonPlusDefinition();
		else
			part=G4MuonMinus::MuonMinusDefinition();
		while(1){
			mom=G4UniformRand()*10;
			if(momentumAmp.Get()->Eval(mom)>momentumAmp.Get()->GetMaximum(0,10)*G4UniformRand())
				break;
		}
		mom*=CLHEP::GeV;
		momentum=G4ThreeVector(mom*sin(theta)*cos(phi),mom*(-cos(theta)),mom*(sin(theta)*sin(phi)));
		yMom=momentum.getY();
	}
	PrimaryEvent res;
	res.push_back(PrimaryParticle(part->GetPDGEncoding(),momentum.getX(),momentum.getY(),momentum.getZ()));
	return res;
}

void CosmicMuonGenerator::Initialize() {
	myTupleId.clear();
	myHistoId.clear();
	Analysis* an=Analysis::Instance();
	myTupleId.push_back(an->CreateNtuple("Cosmics","Comics"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"event"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"pid"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"px"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"py"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"pz"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vx"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vy"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vz"));
	an->FinishNtuple(myTupleId[0]);
	myHistoId.push_back(an->GetH2Id("thetap"));
	position=pGun->GetParticlePosition();
	spotsize=G4ThreeVector(3*CLHEP::cm,3*CLHEP::cm,10*CLHEP::cm);
	runInitialized=true;
}
