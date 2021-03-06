/*
 * DCElasticTimeDependentGenerator.cc
 *
 *  Created on: 12.06.2015
 *      Author: pmaanen
 */

#include <DCElasticTimeDependentGenerator.hh>
#include <G4Event.hh>
#include "G4ParticleGun.hh"
#include "TF1.h"
#include "Analysis.hh"
DCElasticTimeDependentGenerator::DCElasticTimeDependentGenerator():DCElasticEventGenerator() {
	fTmin=0;
	fTmax=100;
	fTau=25;
	fTCur=0;
	DefineCommands();
	fInitialized=false;
}

genvertex_t DCElasticTimeDependentGenerator::Generate() {
	if(!fRunInitialized)
		Initialize();

	while(true){
		//delta_t=t_cur+(t_max-t_cur)*G4UniformRand();
		fTCur=(fTmax-fTmin)*G4UniformRand();
		//TODO
		G4double acc=G4UniformRand()*time_functions::intensity_time_dependence(fTmin,fTau);
		//G4cout<<"t_cur="<<t_cur<<" intensity_time_dependence->Eval(t_cur)="<<intensity_time_dependence->Eval(t_cur/CLHEP::s)<<" acc="<<acc<<G4endl;
		if(time_functions::intensity_time_dependence(fTCur,fTau)>acc)
			break;
	}
	//t_cur+=delta_t;
	//TODO
	setBeamPolarization(time_functions::polarization_time_dependence(fTCur,fM));
	auto event=DCElasticEventGenerator::Generate();
	event.time=fTCur;

	return event;
}

void DCElasticTimeDependentGenerator::DefineCommands() {

	fMessenger->DeclareMethod("polarization", &DCElasticTimeDependentGenerator::setBeamPolarization, "beam polarization");

	fMessenger->DeclareMethod("energy", &DCElasticTimeDependentGenerator::setBeamEnergy, "beam energy");

	fMessenger->DeclareMethod("tmin", &DCElasticTimeDependentGenerator::setMin, "minimum time");

	fMessenger->DeclareMethod("tmax", &DCElasticTimeDependentGenerator::setMax, "maximum time");

	fMessenger->DeclareMethod("tau", &DCElasticTimeDependentGenerator::setTau, "Tau");



}

void DCElasticTimeDependentGenerator::Initialize() {
	DCElasticEventGenerator::Initialize();
	fRunInitialized=true;
	return;
}
