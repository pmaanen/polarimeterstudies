/*
 * E22EventGenerator.hh
 *
 *  Created on: 10.11.2016
 *      Author: pmaanen
 */
#ifndef INCLUDE_E22EVENTGENERATOR_HH_
#define INCLUDE_E22EVENTGENERATOR_HH_
/*
#include "EventGenerator.hh"
#include "VertexGeneratorU.hh"
#include "VertexGeneratorO.hh"
#include "DCElasticEventGenerator.hh"

#include <TF1.h>
class G4ParticleGun;
class E22EventGenerator: public EventGenerator {
public:
	E22EventGenerator(G4ParticleGun* gun=0);
	virtual ~E22EventGenerator()=default;

	virtual void Generate(G4Event*){};
	virtual genevent_t Generate(){genevent_t temp; return temp;};
	virtual void Initialize(){};

private:
	DCElasticEventGenerator fGenerator;
	VertexGeneratorO* fVertexGenXY;
	VertexGeneratorU* fVertexGenZ;

	TF1 fZvsE;
	TF1 fZvsXY;
};
*/
#endif /* INCLUDE_E22EVENTGENERATOR_HH_ */
