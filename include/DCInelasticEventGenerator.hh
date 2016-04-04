/*
 * DCInelasticEventGenerator.hh
 *
 *  Created on: 18.03.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCINELASTICEVENTGENERATOR_HH_
#define INCLUDE_DCINELASTICEVENTGENERATOR_HH_

#include <PhaseSpaceGenerator.hh>
class inelastic_scattering_model;
class DCInelasticEventGenerator: public PhaseSpaceGenerator {
public:
	DCInelasticEventGenerator(G4ParticleGun* pgun=0);
	//virtual ~DCInelasticEventGenerator();
	virtual void Initialize();
	virtual void Generate(G4Event* E);
	virtual genevent_t Generate();
private:
	Double_t fBeamPolarization;
	inelastic_scattering_model* fScatteringModel;
};


class inelastic_scattering_model{
public:
	inelastic_scattering_model(Double_t beamPolarization):fBeamPolarization(beamPolarization){};
	Double_t Sigma(Double_t q, Double_t Ex){ return SigmaUnpol(q,Ex);};
	Double_t SigmaUnpol(Double_t q, Double_t Ex){return pow(10,f1(Ex)+f2(Ex)*(q-0.25)+f3(Ex)*(q-0.25)*(q-0.25));};

	void setBeamPolarization(Double_t beamPolarization) {fBeamPolarization = beamPolarization;}

private:

	Double_t fBeamPolarization;

	double f1(Double_t Ex){return 0.81403-8.5131e-4*Ex;};
	double f2(Double_t Ex){return -3.1246-0.068376*Ex+0.004052*Ex*Ex;};
	double f3(Double_t Ex){return -4.8934+0.028654*Ex;};

	//Parameters for Ay. Conversion from theta_cm to theta_lab is missing, but this does not affect us very much.
	double Ay(Double_t theta, Double_t Ex);
	double g1(Double_t Ex);
	double g2(Double_t Ex);
	double g3(Double_t Ex);
};
#endif /* INCLUDE_DCINELASTICEVENTGENERATOR_HH_ */
