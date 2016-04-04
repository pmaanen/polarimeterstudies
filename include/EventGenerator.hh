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
#include "hit.hh"
class G4GenericMessenger;

class G4ParticleGun;
class EventGenerator {
public:
	EventGenerator(G4ParticleGun* pgun=0);
	virtual ~EventGenerator();

	virtual void Generate(G4Event*)=0;
	virtual genevent_t Generate()=0;
	virtual void Initialize()=0;

protected:
	std::vector<G4int> fTupleId;
	std::vector<G4int> fHistoId;


	G4ParticleGun* fParticleGun;
	G4bool fRunInitialized;

	G4GenericMessenger* fMessenger;
};

#endif /* INCLUDE_EVENTGENERATOR_HH_ */
