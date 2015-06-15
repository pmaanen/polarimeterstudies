/*
 * DCElasticTimeDependentGenerator.hh
 *
 *  Created on: 12.06.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCELASTICTIMEDEPENDENTGENERATOR_HH_
#define INCLUDE_DCELASTICTIMEDEPENDENTGENERATOR_HH_

#include <DCElasticEventGenerator.hh>
class time_functions;
class DCElasticTimeDependentGenerator: public DCElasticEventGenerator {
public:
	DCElasticTimeDependentGenerator(G4ParticleGun* pgun=0);
	virtual ~DCElasticTimeDependentGenerator();


	virtual PrimaryEvent Generate();
	virtual void Generate(G4Event* E);

private:
	TF1* polarization_time_dependence;
	TF1* intensity_time_dependence;

	time_functions* fct_helper;
	G4double t_min,t_max,tau;
	void Initialize();


};

class time_functions{
public:
	Double_t polarization_time_dependence(Double_t *x,Double_t *par){
		return x[0];
	}
	Double_t intensity_time_dependence(Double_t* x,Double_t *par){
		return exp(-x[0]/400.);
	}

};

#endif /* INCLUDE_DCELASTICTIMEDEPENDENTGENERATOR_HH_ */
