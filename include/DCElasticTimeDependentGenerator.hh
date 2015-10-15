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
	virtual void DefineCommands();

	void setMax(G4double max) {
		fTmax = max;
		fInitialized=false;
	}

	void setMin(G4double min) {
		fTmin = min;
		fInitialized=false;
	}

	void setTau(G4double tau) {
		this->fTau = tau;
		fInitialized=false;
	}

private:
	TF1* fPolarizationTimeDependence;
	TF1* fIntensityTimeDependence;

	time_functions* fTimeFunctions;
	G4double fTmin,fTmax,fTau,fTcur;
	void Initialize();


};

class time_functions{
public:
	time_functions(Double_t max,Double_t min,Double_t t){};
	Double_t polarization_time_dependence(Double_t *x,Double_t *par){
		return x[0]/(par[0])*2/3;
	}
	Double_t intensity_time_dependence(Double_t* x,Double_t *par){
		return exp(-x[0]/par[0]);
	}
};

#endif /* INCLUDE_DCELASTICTIMEDEPENDENTGENERATOR_HH_ */
