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
	DCElasticTimeDependentGenerator();
	virtual ~DCElasticTimeDependentGenerator()=default;


	virtual genvertex_t Generate();
	virtual void DefineCommands();

	void setMax(G4double max) {
		fTmax = max;
		fRunInitialized=false;
	}

	void setMin(G4double min) {
		fTmin = min;
		fRunInitialized=false;
	}

	void setTau(G4double tau) {
		this->fTau = tau;
		fRunInitialized=false;
	}

private:

	G4double fTmin,fTmax,fTau,fTCur,fM;
	void Initialize();


};

class time_functions{
public:
	static G4double polarization_time_dependence(G4double t,G4double m){
		return m*t*2./3.;
	}
	static G4double intensity_time_dependence(G4double t,G4double tau){
		return exp(-t/tau);
	}
};

#endif /* INCLUDE_DCELASTICTIMEDEPENDENTGENERATOR_HH_ */
