/*
 * DCBreakupEventGenerator.hh
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#define INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
class deuteron_breakup_model;
class G4ParticleGun;
class DCBreakupEventGenerator : public PhaseSpaceGenerator {
public:
	DCBreakupEventGenerator(G4ParticleGun* pgun=0);
	virtual ~DCBreakupEventGenerator();

protected:
	virtual TF2* BuildFunction();
	virtual void Initialize();
	virtual PrimaryEvent Generate();
	virtual void Generate(G4Event* E);
	virtual void beginOfRun(){};

	virtual void DefineCommands();
	Double_t momentum_cms,beamPolarization;
	deuteron_breakup_model* scattering_model;
	TF2* cross_section;

	void setBeamPolarization(G4double xBeamPolarization) {beamPolarization = Double_t(xBeamPolarization);
	if(cross_section)
		cross_section->SetParameter(2,beamPolarization);
	}
};


class deuteron_breakup_model{
public:
	Double_t sigma(Double_t *x, Double_t *par);
	deuteron_breakup_model();

private:
	Double_t SigmaUnpol(Double_t, Double_t, Double_t);
	Double_t c1(Double_t theta, Double_t E);
	Double_t c2(Double_t theta, Double_t E);
	Double_t c3(Double_t theta);
	Double_t c4(Double_t theta);
	Double_t c5(Double_t theta);

	Double_t Ay(Double_t, Double_t, Double_t);
	Double_t d1(Double_t theta, Double_t E);
	Double_t d2(Double_t theta, Double_t E);

};
#endif /* INCLUDE_DCBREAKUPEVENTGENERATOR_HH_ */
