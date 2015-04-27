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
class CosmicMuonGenerator: public EventGenerator{
public:
	CosmicMuonGenerator(G4ParticleGun* pgun);
	virtual ~CosmicMuonGenerator();


	virtual void Generate(G4Event* E);
	virtual void beginOfRun();

	TF1 angle,energy,particle;
	G4ParticleGun* pgun;



};

#endif /* INCLUDE_COSMICMUONGENERATOR_HH_ */
