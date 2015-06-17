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
	G4double t_min,t_max,tau,t_cur;
	void Initialize();


};

class time_functions{
	time_functions(Double_t max,Double_t min,Double_t t):t_max(max),t_min(min),tau(t){};
public:
	Double_t polarization_time_dependence(Double_t *x,Double_t *par){
		return x[0]/(t_max)*2/3;
	}
	Double_t intensity_time_dependence(Double_t* x,Double_t *par){
		return exp(-x[0]/tau)/tau;
	}

private:
	Double_t t_max,t_min,tau;

};

#endif /* INCLUDE_DCELASTICTIMEDEPENDENTGENERATOR_HH_ */
