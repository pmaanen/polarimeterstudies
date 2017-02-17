/*
 * DCBreakupEventGenerator.hh
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#define INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
class G4ParticleGun;
class TF1;
class DCBreakupEventGenerator : public PhaseSpaceGenerator {
public:
	DCBreakupEventGenerator();
	virtual ~DCBreakupEventGenerator()=default;

protected:
	virtual void Initialize();
	virtual genevent_t Generate();
	virtual void BeginOfRun(){};

	virtual void DefineCommands();
	Double_t fMomentum_cms,fBeamPolarization;
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization); fRunInitialized=false;};
	std::unique_ptr<TF2> fThetaEx;
	std::unique_ptr<TF1> fPhi;

};
#endif /* INCLUDE_DCBREAKUPEVENTGENERATOR_HH_ */
