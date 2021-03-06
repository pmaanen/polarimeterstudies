/*
 * PhaseSpaceGenerator.hh
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_PHASESPACEGENERATOR_HH_
#define INCLUDE_PHASESPACEGENERATOR_HH_
#include "G4ParticleDefinition.hh"
#include <G4Deuteron.hh>
#include "G4IonTable.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4GenericMessenger.hh"
#include "Analysis.hh"
#include "G4GenericMessenger.hh"
#include <math.h>
#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>
#include "TF2.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "globals.hh"
#include <TMath.h>
#include <utility>
#include <vector>
#include <G4UnitsTable.hh>
#include <JediConfigurationManager.hh>

#include "EventGenerator.hh"
class G4ParticleDefinition;
class G4GenericMessenger;
class G4ParticleGun;

class PhaseSpaceGenerator: public EventGenerator {
public:
	PhaseSpaceGenerator(G4String name);
	virtual ~PhaseSpaceGenerator()=default;

	G4double getBeamEnergy() const {return fBeamEnergy;}
	void setBeamEnergy(G4double xBeamEnergy) {fBeamEnergy = xBeamEnergy; fInitialized=false;}

	void setThetaMax(G4double thetaMax) {fThetaMax = thetaMax;fInitialized=false;}
	void setThetaMin(G4double thetaMin) {fThetaMin = thetaMin; fInitialized=false;}

	void setBeamposition( G4ThreeVector beamspot) {fBeamposition = beamspot; fInitialized=false;}
	void setBeamsize( G4ThreeVector spotsize) {fBeamsize = spotsize; fInitialized=false;}

	G4double getTiltX() const {
		return fTiltX;
	}

	void setTiltX(G4double tiltX) {
		fTiltX = tiltX;
	}

protected:
	G4double fBeamEnergy,fMaxY,fThetaMin,fThetaMax;
	G4bool fInitialized;

	G4LorentzVector fCms,fBeam,fTarget;

	//Definition of particles. particles[0] carries the kinetic energy.
	std::vector<G4ParticleDefinition*> fParticles;

	std::unique_ptr<G4GenericMessenger> fMessenger;
	G4double fXPrime,fYPrime,fTiltX,fTiltY;
	G4ThreeVector fBeamposition, fBeamsize;
	G4String fName;

	virtual void DefineCommands();
	//Returns a the TF2 for hit and miss.
	virtual void Initialize()=0;


};

#endif /* INCLUDE_PHASESPACEGENERATOR_HH_ */
