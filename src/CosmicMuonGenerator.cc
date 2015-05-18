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

	//SigmaFunc=new TF2("xsec",func,&DCElasticEventGenerator::MyFunction::sigma,3.,30.,0.,360.,3,"MyFunction","sigma");
	angle.Put(new TF1("cos_squared",functions.Get(),&CosmicMuonGenerator::function_helper::angle,0,3.1415,0,"function_helper","angle"));
	momentumAmp.Put(new TF1("energy",functions.Get(),&CosmicMuonGenerator::function_helper::energy,0,20,0,"function_helper","energy"));
	particle.Put(new TF1("particle",functions.Get(),&CosmicMuonGenerator::function_helper::particle,-1,1,0,"function_helper","particle"));
}

CosmicMuonGenerator::~CosmicMuonGenerator() {}

void CosmicMuonGenerator::Generate(G4Event* E) {
	if(!runInitialized)
		Initialize();
	G4double yMom=1;
	G4double mom=0;
	G4ThreeVector momentum;
	G4double theta=0;
	G4double phi=0;
	G4ParticleDefinition* part=0;
	while(yMom>0){
		theta=angle.Get()->GetRandom()*CLHEP::rad;
		phi=G4Random::getTheGenerator()->flat()*2*CLHEP::pi;
		if(particle.Get()->GetRandom()>0)
			part=G4MuonPlus::MuonPlusDefinition();
		else
			part=G4MuonMinus::MuonMinusDefinition();


		mom=momentumAmp.Get()->GetRandom()*CLHEP::GeV;
		momentum=G4ThreeVector(sin(theta)*cos(phi),-cos(theta),sin(theta)*sin(phi));
		yMom=momentum.getY();
	}

	Analysis* an=Analysis::Instance();
	an->FillH2(myHistoId[0],mom/CLHEP::MeV,theta/CLHEP::deg);
	an->FillNtupleIColumn(myTupleId[0],myTupleId[1],E->GetEventID());
	an->FillNtupleIColumn(myTupleId[0],myTupleId[2],pGun->GetParticleDefinition()->GetPDGEncoding());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[3],momentum.getX());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[4],momentum.getY());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[5],momentum.getZ());
	an->FillNtupleFColumn(myTupleId[0],myTupleId[6],pGun->GetParticlePosition().getX()/CLHEP::mm);
	an->FillNtupleFColumn(myTupleId[0],myTupleId[7],pGun->GetParticlePosition().getY()/CLHEP::mm);
	an->FillNtupleFColumn(myTupleId[0],myTupleId[8],pGun->GetParticlePosition().getZ()/CLHEP::mm);
	an->AddNtupleRow(myTupleId[0]);


	pGun->SetParticleMomentum(mom);
	pGun->SetParticleMomentumDirection(momentum) ;
	pGun->SetParticleDefinition(part);
	pGun->GeneratePrimaryVertex(E);
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
	runInitialized=true;
}
