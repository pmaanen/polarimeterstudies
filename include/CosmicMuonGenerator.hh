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
#include "hit.hh"
#include "G4ThreeVector.hh"
class G4GenericMessenger;
class function_helper;
class CosmicMuonGenerator: public EventGenerator{
public:
	CosmicMuonGenerator(G4ParticleGun* pgun=0);
	virtual ~CosmicMuonGenerator();


	virtual void Generate(G4Event* E);
	virtual genevent_t Generate();
	virtual void Initialize();
	G4ThreeVector fPosition, fSpotsize;


	G4GenericMessenger* fMessenger;
};
class cosmic_functions{
	public:
		static G4double angle(G4double theta){return cos(theta)*cos(theta);};
		static G4double momentum(G4double momAmp){if(momAmp/CLHEP::GeV<3.35) return std::pow(3.35,-2.7); else return std::pow(momAmp/CLHEP::GeV,-2.7);};
	};

#endif /* INCLUDE_COSMICMUONGENERATOR_HH_ */
