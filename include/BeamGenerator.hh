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
#include "VVertexGenerator.hh"
class BeamGenerator: public EventGenerator {
public:
	BeamGenerator(G4ParticleGun* gun);
	virtual ~BeamGenerator()=default;
	virtual genevent_t Generate();
	virtual void Initialize() {
	}
	void setPosition(const G4ThreeVector& position) {
		fVertexGenerator->setBeamsize(position.x()/CLHEP::mm,position.y()/CLHEP::mm,position.z()/CLHEP::mm);
		fPosition = position;
	}

	void setSpotsize(const G4ThreeVector& spotsize) {
		fVertexGenerator->setBeamsize(spotsize.x()/CLHEP::mm,spotsize.y()/CLHEP::mm,spotsize.z()/CLHEP::mm);
		fSpotsize = spotsize;
	}

private:
	G4ParticleGun* fParticleGun;
	G4ThreeVector fPosition, fSpotsize;
	G4double fXPrime,fYPrime, fEnergy;
	VVertexGenerator* fVertexGenerator;
	void SetVertexGenerator(G4String);
	void SetEnergy(G4double);
};

#endif /* INCLUDE_BEAMGENERATOR_HH_ */
