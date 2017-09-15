/*
 * CosmicMuonGenerator.h
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_COSMICMUONGENERATOR_HH_
#define INCLUDE_COSMICMUONGENERATOR_HH_

#include "PrimaryGeneratorAction.hh"
#include "EventGenerator.hh"
#include "GenEventProducer.hh"
#include <G4ThreeVector.hh>
#include <JediClasses.hh>
#include <TF1.h>
#include <cmath>
#include <memory>
class G4GenericMessenger;
class cosmic_functions;
class CosmicMuonGenerator: public EventGenerator{
public:
	CosmicMuonGenerator();
	virtual ~CosmicMuonGenerator()=default;

	virtual genvertex_t Generate();
	virtual void Initialize();
	G4ThreeVector fPosition, fSpotsize;

	std::unique_ptr<TF1> fTheta;
	std::unique_ptr<TF1> fMomentum;
	std::unique_ptr<cosmic_functions> fHelperFunctions;

};
class cosmic_functions{
	public:
		Double_t angle(Double_t* x,Double_t*){return cos(x[0])*cos(x[0]);};
		Double_t momentum(Double_t* x,Double_t*){if(x[0]<3.35) return std::pow(3.35,-2.7); else return std::pow(x[0],-2.7);};
	};

#endif /* INCLUDE_COSMICMUONGENERATOR_HH_ */
