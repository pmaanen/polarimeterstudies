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
	virtual void Initialize();
	virtual genevent_t Generate();
	virtual void Generate(G4Event* E);
	virtual void BeginOfRun(){};

	virtual void DefineCommands();
	Double_t fMomentum_cms,fBeamPolarization;
	deuteron_breakup_model* fScatteringModel;
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization); fRunInitialized=false;};

};

class deuteron_breakup_model{
public:
	Double_t sigma(Double_t theta, Double_t phi, Double_t Ex);
	deuteron_breakup_model(Double_t beamEnergy, Double_t beamPolarization):fBeamEnergy(beamEnergy),fBeamPolarization(beamPolarization){
		for(int theta=1;theta<30;theta++){
			for(int iEkin=1;iEkin<11;iEkin++){
				//G4cout<<"sigma("<<theta<<"°,"<<iEkin*10<<"MeV)="<<SigmaUnpol(beamEnergy,theta,iEkin*10)<<G4endl;
			}

		}


	};

	Double_t SigmaUnpol(Double_t, Double_t, Double_t);

	void setBeamEnergy(Double_t beamEnergy) {
		fBeamEnergy = beamEnergy;
	}

	void setBeamPolarization(Double_t beamPolarization) {
		fBeamPolarization = beamPolarization;
	}

private:
	Double_t fBeamEnergy,fBeamPolarization;
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
