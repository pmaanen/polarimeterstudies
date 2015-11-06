/*
 * CosmicMuonGenerator.h
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_COSMICMUONGENERATOR_HH_
#define INCLUDE_COSMICMUONGENERATOR_HH_

#include <PrimaryGeneratorAction.hh>
#include "TF1.h"
#include "EventGenerator.hh"
#include <cmath>
#include "G4ThreeVector.hh"
class G4GenericMessenger;
class function_helper;
class CosmicMuonGenerator: public EventGenerator{
public:
	CosmicMuonGenerator(G4ParticleGun* pgun=0);
	virtual ~CosmicMuonGenerator();


	virtual void Generate(G4Event* E);
	virtual PrimaryEvent Generate();
	virtual void Initialize();

	function_helper* fFunctions;
	TF1* fAngle,*fMomentumAmp;
	G4ThreeVector fPosition, fSpotsize;
	G4double fThetaMax;

	G4GenericMessenger* fMessenger;
};
class function_helper{
	public:
		Double_t angle(Double_t *x, Double_t *){return cos(x[0])*cos(x[0]);};
		Double_t energy(Double_t *x, Double_t *){if(x[0]<3.35) return std::pow(3.35,-2.7); else return std::pow(x[0],-2.7);};
	};

#endif /* INCLUDE_COSMICMUONGENERATOR_HH_ */
