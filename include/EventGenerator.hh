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

#include <ostream>
#include <vector>

struct PrimaryParticle {
	G4int id;
	G4double px;
	G4double py;
	G4double pz;

	PrimaryParticle(G4int id,G4double px,G4double py,G4double pz):id(id),px(px),py(py),pz(pz){};

	friend std::ostream& operator<< (std::ostream& stream, const PrimaryParticle& particle){
		stream<<particle.id<<" "<<particle.px<<" "<<particle.py<<" "<<particle.pz;
		return stream;
	};
};
typedef std::vector<PrimaryParticle> PrimaryEvent;

class G4ParticleGun;
class EventGenerator {
public:
	EventGenerator(G4ParticleGun* pgun=0);
	virtual ~EventGenerator();

	virtual void Generate(G4Event*)=0;
	virtual void Initialize()=0;

protected:
	std::vector<G4int> myTupleId;
	std::vector<G4int> myHistoId;


	G4ParticleGun* pGun;
	G4bool runInitialized;
};

#endif /* INCLUDE_EVENTGENERATOR_HH_ */
