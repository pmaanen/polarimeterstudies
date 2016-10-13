/*
 * DCBreakupEventGenerator.hh
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#define INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
class DeuteronCarbonBreakupModel;
class G4ParticleGun;
class DCBreakupEventGenerator : public PhaseSpaceGenerator {
public:
	DCBreakupEventGenerator(G4ParticleGun* pgun=0);
	virtual ~DCBreakupEventGenerator();

protected:
	virtual void Initialize();
	virtual genevent_t Generate();
	virtual void Generate(G4Event* E);
	virtual void BeginOfRun(){};

	virtual void DefineCommands();
	Double_t fMomentum_cms,fBeamPolarization;
	std::unique_ptr<DeuteronCarbonBreakupModel> fScatteringModel;
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization); fRunInitialized=false;};

};
#endif /* INCLUDE_DCBREAKUPEVENTGENERATOR_HH_ */
