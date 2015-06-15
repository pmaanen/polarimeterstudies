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

	fct_helper=new time_functions;
	polarization_time_dependence=new TF1("polarization_time_dependence",fct_helper,&time_functions::polarization_time_dependence,0,1000,0,"time_functions","polarization_time_dependence");
	intensity_time_dependence=new TF1("intensity_time_dependence",fct_helper,&time_functions::intensity_time_dependence,0,1000,0,"time_functions","intensity_time_dependence");
	t_min=0;
	t_max=1000;
	tau=1;
	Initialized=false;
}

DCElasticTimeDependentGenerator::~DCElasticTimeDependentGenerator() {
	delete polarization_time_dependence;
	delete intensity_time_dependence;
}

PrimaryEvent DCElasticTimeDependentGenerator::Generate() {
	if(!Initialized)
		Initialize();
	G4double cur_time=0;
	while(true){
		cur_time=t_min+(t_max-t_min)*G4UniformRand();
		G4double acc=G4UniformRand()*intensity_time_dependence->GetMaximum(0,1000);
		if(intensity_time_dependence->Eval(cur_time)>acc)
			break;
	}
	setBeamPolarization(polarization_time_dependence->Eval(cur_time)/t_max);
	auto event=PrimaryEvent(DCElasticEventGenerator::Generate());
	event.t=cur_time;
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
		an->AddNtupleRow(myTupleId[0]);

	}
	return;
}

void DCElasticTimeDependentGenerator::Initialize() {
	DCElasticEventGenerator::Initialize();
	Initialized=true;
	return;
}
