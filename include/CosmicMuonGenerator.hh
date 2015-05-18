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
class CosmicMuonGenerator: public EventGenerator{
public:
	CosmicMuonGenerator(G4ParticleGun* pgun);
	virtual ~CosmicMuonGenerator();


	virtual void Generate(G4Event* E);
	virtual void Initialize();

	class function_helper{
	public:
		Double_t angle(Double_t *x, Double_t *par){return cos(x[0])*cos(x[0]);};
		Double_t energy(Double_t *x, Double_t *par){if(x[0]<3.35) return std::pow(3.35,-2.7); else return std::pow(x[0],-2.7);};
		Double_t particle(Double_t *x, Double_t *par){return 1;};
	};

	G4Cache<CosmicMuonGenerator::function_helper*> functions;
	G4Cache<TF1*> angle,momentumAmp,particle;
};

#endif /* INCLUDE_COSMICMUONGENERATOR_HH_ */
