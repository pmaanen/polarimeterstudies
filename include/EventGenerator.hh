/*
 * EventGenerator.hh
 *
 *  Created on: 27.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_EVENTGENERATOR_HH_
#define INCLUDE_EVENTGENERATOR_HH_
class G4ParticleGun;
class G4Event;
#include "G4Types.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include <ostream>
#include <vector>
#include <memory>
#include "hit.hh"
class G4GenericMessenger;

class EventGenerator {
public:
	EventGenerator();
	virtual ~EventGenerator()=default;

	virtual genevent_t Generate()=0;
	virtual void Initialize()=0;

protected:
	G4bool fRunInitialized;
	std::unique_ptr<G4GenericMessenger> fMessenger;
};

#endif /* INCLUDE_EVENTGENERATOR_HH_ */
