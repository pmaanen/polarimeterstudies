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
	fTmax=100;
	fTau=25;
	fTCur=0;
	fTimeFunctions=new time_functions(fTmax,fTmin,fTau);
	fPolarizationTimeDependence=new TF1("polarization_time_dependence",fTimeFunctions,&time_functions::polarization_time_dependence,fTmin,fTmax,1,"time_functions","polarization_time_dependence");
	fIntensityTimeDependence=new TF1("intensity_time_dependence",fTimeFunctions,&time_functions::intensity_time_dependence,fTmin,fTmax,1,"time_functions","intensity_time_dependence");
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

	while(true){
		//delta_t=t_cur+(t_max-t_cur)*G4UniformRand();
		fTCur=(fTmax-fTmin)*G4UniformRand();
		G4double acc=G4UniformRand()*fIntensityTimeDependence->GetMaximum(fTmin,fTmax);
		//G4cout<<"t_cur="<<t_cur<<" intensity_time_dependence->Eval(t_cur)="<<intensity_time_dependence->Eval(t_cur/CLHEP::s)<<" acc="<<acc<<G4endl;
		if(fIntensityTimeDependence->Eval(fTCur/CLHEP::s)>acc)
			break;
	}
	//t_cur+=delta_t;
	setBeamPolarization(fPolarizationTimeDependence->Eval(fTCur));
	auto event=PrimaryEvent(DCElasticEventGenerator::Generate());
	event.t=fTCur;
	return event;
}

void DCElasticTimeDependentGenerator::Generate(G4Event* E) {
	auto event=PrimaryEvent(Generate());
	fParticleGun->SetParticleTime(event.t*CLHEP::s);
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
		an->FillNtupleIColumn(fTupleId[0],fTupleId[1],E->GetEventID());
		an->FillNtupleIColumn(fTupleId[0],fTupleId[2],iPart->id);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[3],iPart->px);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[4],iPart->py);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[5],iPart->pz);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[6],fParticleGun->GetParticlePosition().getX()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[7],fParticleGun->GetParticlePosition().getY()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[8],fParticleGun->GetParticlePosition().getZ()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[9],fParticleGun->GetParticleTime()/CLHEP::s);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[10],this->fBeamPolarization);
		an->AddNtupleRow(fTupleId[0]);
	}
	return;
}

void DCElasticTimeDependentGenerator::DefineCommands() {

	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dCtime/", "time dependent event generator control");

	fMessenger->DeclareMethod("polarization", &DCElasticTimeDependentGenerator::setBeamPolarization, "beam polarization");

	fMessenger->DeclareMethod("energy", &DCElasticTimeDependentGenerator::setBeamEnergy, "beam energy");

	fMessenger->DeclareMethod("tmin", &DCElasticTimeDependentGenerator::setMin, "minimum time");

	fMessenger->DeclareMethod("tmax", &DCElasticTimeDependentGenerator::setMax, "maximum time");

	fMessenger->DeclareMethod("tau", &DCElasticTimeDependentGenerator::setTau, "Tau");



}

void DCElasticTimeDependentGenerator::Initialize() {
	DCElasticEventGenerator::Initialize();
	fIntensityTimeDependence->SetParameter(0,fTau/CLHEP::s);
	fPolarizationTimeDependence->SetParameter(0,fTmax);
	fInitialized=true;
	return;
}
