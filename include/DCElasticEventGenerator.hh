/*
 * DCElasticEventGenerator.hh
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#ifndef DCELASTICEVENTGENERATOR_HH_
#define DCELASTICEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
#include "DeuteronCarbonElasticScatteringModel.hh"
#include <G4ThreeVector.hh>
#include "TF2.h"
#include "TGenPhaseSpace.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "globals.hh"
#include <TMath.h>
#include <utility>
#include <vector>

#include <G4UnitsTable.hh>
class G4ParticleDefinition;
class G4GenericMessenger;
class DCElasticEventGenerator : public PhaseSpaceGenerator {
public:
	DCElasticEventGenerator();
	virtual ~DCElasticEventGenerator()=default;
	virtual void Initialize();
	virtual genevent_t Generate();
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization);	fInitialized=false;};

protected:

	Double_t fMomentumCMS,fBeamPolarization;
	std::unique_ptr<JediElasticModel> fScatteringModel;
	std::unique_ptr<TF1> fQ;
	std::unique_ptr<TF1> fPhi;
	virtual void DefineCommands();
	Double_t fQmin,fQmax;
	//Returns a the TF2 for hit and miss.
};
#endif /* DCELASTICEVENTGENERATOR_HH_ */
