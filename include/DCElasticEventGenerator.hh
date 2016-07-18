/*
 * DCElasticEventGenerator.hh
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#ifndef DCELASTICEVENTGENERATOR_HH_
#define DCELASTICEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
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
class DeuteronCarbonElasticScatteringModel;
class DCElasticEventGenerator : public PhaseSpaceGenerator {
public:
	DCElasticEventGenerator(G4ParticleGun* pgun=0);
	virtual ~DCElasticEventGenerator();
	virtual void Initialize();
	virtual genevent_t Generate();
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization);	fRunInitialized=false;};

protected:

	Double_t fMomentumCMS,fBeamPolarization;
	DeuteronCarbonElasticScatteringModel* fScatteringModel;
	virtual void DefineCommands();
	//Returns a the TF2 for hit and miss.
};
#endif /* DCELASTICEVENTGENERATOR_HH_ */
