/*
 * BeamGenerator.hh
 *
 *  Created on: 19.02.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_BEAMGENERATOR_HH_
#define INCLUDE_BEAMGENERATOR_HH_


#include <EventGenerator.hh>
#include "G4ThreeVector.hh"
class BeamGenerator: public EventGenerator {
public:
	BeamGenerator(G4ParticleGun* gun=0);
	virtual ~BeamGenerator();

	virtual void Generate(G4Event*);
	virtual PrimaryEvent Generate();
	virtual void Initialize(){};

private:
	G4ThreeVector fPosition, fSpotsize;
	G4double fXPrime,fYPrime;
	G4ParticleGun* fGun;
};

#endif /* INCLUDE_BEAMGENERATOR_HH_ */
