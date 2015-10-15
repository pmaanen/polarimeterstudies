/*
 * DCElasticTimeDependentGenerator.cc
 *
 *  Created on: 12.06.2015
 *      Author: pmaanen
 */

#include <DCElasticTimeDependentGenerator.hh>
#include "G4ParticleGun.hh"
#include "TF1.h"
#include "Analysis.hh"
DCElasticTimeDependentGenerator::DCElasticTimeDependentGenerator(G4ParticleGun* pgun):DCElasticEventGenerator(pgun) {
	fTmin=0;
	t_max=100;
	fTau=25;
	fTcur=0;
	fTimeFunctions=new time_functions(t_max,fTmin,fTau);
	fPolarizationTimeDependence=new TF1("polarization_time_dependence",fTimeFunctions,&time_functions::polarization_time_dependence,fTmin,t_max,1,"time_functions","polarization_time_dependence");
	fIntensityTimeDependence=new TF1("intensity_time_dependence",fTimeFunctions,&time_functions::intensity_time_dependence,fTmin,t_max,1,"time_functions","intensity_time_dependence");
	DefineCommands();
	fInitialized=false;
}

DCElasticTimeDependentGenerator::~DCElasticTimeDependentGenerator() {
	delete fPolarizationTimeDependence;
	delete fIntensityTimeDependence;
}

PrimaryEvent DCElasticTimeDependentGenerator::Generate() {
	if(!fInitialized)
		Initialize();
	G4double delta_t;
	while(true){
		//delta_t=t_cur+(t_max-t_cur)*G4UniformRand();
		fTcur=(t_max-fTmin)*G4UniformRand();
		G4double acc=G4UniformRand()*fIntensityTimeDependence->GetMaximum(fTmin,t_max);
		//G4cout<<"t_cur="<<t_cur<<" intensity_time_dependence->Eval(t_cur)="<<intensity_time_dependence->Eval(t_cur/CLHEP::s)<<" acc="<<acc<<G4endl;
		if(fIntensityTimeDependence->Eval(fTcur/CLHEP::s)>acc)
			break;
	}
	//t_cur+=delta_t;
	setBeamPolarization(fPolarizationTimeDependence->Eval(fTcur));
	auto event=PrimaryEvent(DCElasticEventGenerator::Generate());
	event.t=fTcur;
	return event;
}

void DCElasticTimeDependentGenerator::Generate(G4Event* E) {
	auto event=PrimaryEvent(Generate());
	pGun->SetParticleTime(event.t*CLHEP::s);
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		//TODO Write Truth
		pGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		pGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		pGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
		an->FillNtupleIColumn(myTupleId[0],myTupleId[1],E->GetEventID());
		an->FillNtupleIColumn(myTupleId[0],myTupleId[2],iPart->id);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[3],iPart->px);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[4],iPart->py);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[5],iPart->pz);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[6],pGun->GetParticlePosition().getX()/CLHEP::mm);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[7],pGun->GetParticlePosition().getY()/CLHEP::mm);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[8],pGun->GetParticlePosition().getZ()/CLHEP::mm);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[9],pGun->GetParticleTime()/CLHEP::s);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[10],this->fBeamPolarization);
		an->AddNtupleRow(myTupleId[0]);
	}
	return;
}

void DCElasticTimeDependentGenerator::DefineCommands() {

	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dCtime/", "time dependent event generator control");
	G4GenericMessenger::Command& polCmd
	= fMessenger->DeclareMethod("polarization", &DCElasticTimeDependentGenerator::setBeamPolarization, "beam polarization");

	G4GenericMessenger::Command& eneCmd
	= fMessenger->DeclareMethod("energy", &DCElasticTimeDependentGenerator::setBeamEnergy, "beam energy");

	G4GenericMessenger::Command& tminCmd
	= fMessenger->DeclareMethod("tmin", &DCElasticTimeDependentGenerator::setMin, "minimum time");

	G4GenericMessenger::Command& tmaxCmd
	= fMessenger->DeclareMethod("tmax", &DCElasticTimeDependentGenerator::setMax, "maximum time");

	G4GenericMessenger::Command& tauCmd
	= fMessenger->DeclareMethod("tau", &DCElasticTimeDependentGenerator::setTau, "Tau");



}

void DCElasticTimeDependentGenerator::Initialize() {
	DCElasticEventGenerator::Initialize();
	fIntensityTimeDependence->SetParameter(0,fTau/CLHEP::s);
	fPolarizationTimeDependence->SetParameter(0,t_max);
	fInitialized=true;
	return;
}
