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
class G4GenericMessenger;
struct PrimaryParticle {
	G4int id;
	G4double px,py,pz;

	PrimaryParticle(G4int iid,G4double ppx,G4double ppy,G4double ppz):id(iid),px(ppx),py(ppy),pz(ppz){};

	friend std::ostream& operator<< (std::ostream& stream, const PrimaryParticle& particle){
		stream<<particle.id<<" "<<particle.px/CLHEP::GeV<<" "<<particle.py/CLHEP::GeV<<" "<<particle.pz/CLHEP::GeV;
		return stream;
	};


};

struct PrimaryEvent{
	G4double t,vx,vy,vz;
	std::vector<PrimaryParticle> particles;

	PrimaryEvent(G4int tt=0,G4double vvx=0,G4double vvy=0,G4double vvz=0):t(tt),vx(vvx),vy(vvy),vz(vvz){};

	std::vector<PrimaryParticle>::iterator begin(){return particles.begin();}
	std::vector<PrimaryParticle>::iterator end(){return particles.end();}
	friend std::ostream& operator<< (std::ostream& stream, const PrimaryEvent& event){
		for(auto ipart: event.particles){
			stream<<ipart<<event.vx/CLHEP::mm<<" "<<event.vy/CLHEP::mm<<" "<<event.vz/CLHEP::mm<<" "<<event.t/CLHEP::s;
		}
		return stream;
	};
};

class G4ParticleGun;
class EventGenerator {
public:
	EventGenerator(G4ParticleGun* pgun=0);
	virtual ~EventGenerator();

	virtual void Generate(G4Event*)=0;
	virtual PrimaryEvent Generate()=0;
	virtual void Initialize()=0;

protected:
	std::vector<G4int> myTupleId;
	std::vector<G4int> myHistoId;


	G4ParticleGun* pGun;
	G4bool runInitialized;

	G4GenericMessenger* fMessenger;
};

#endif /* INCLUDE_EVENTGENERATOR_HH_ */
