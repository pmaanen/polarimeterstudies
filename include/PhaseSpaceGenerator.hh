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
#include "TCanvas.h"
#include "TH1.h"
#include "Randomize.hh"
#include "G4GenericMessenger.hh"
#include "Analysis.hh"
#include "TVector3.h"
#include "G4GenericMessenger.hh"
#include <math.h>
#include <G4ThreeVector.hh>
#include "TF2.h"
#include "TGenPhaseSpace.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "globals.hh"
#include <TMath.h>
#include <utility>
#include <vector>

#include <G4UnitsTable.hh>

#include "EventGenerator.hh"
class G4ParticleDefinition;
class G4GenericMessenger;
class G4ParticleGun;

#ifndef FILEWRITER
typedef std::vector<std::pair<G4ParticleDefinition*,G4ThreeVector> > ParticleMomentumVector;
#else
typedef std::vector<std::pair<G4int,G4ThreeVector> > ParticleMomentumVector;
#endif

class PhaseSpaceGenerator: public EventGenerator {
public:
	PhaseSpaceGenerator(G4ParticleGun* gun=0);
	virtual ~PhaseSpaceGenerator();
	virtual ParticleMomentumVector Generate()=0;

	G4double getBeamEnergy() const {return beamEnergy;}
	void setBeamEnergy(G4double xBeamEnergy) {beamEnergy = xBeamEnergy;Initialized=false;}

protected:
	G4double beamEnergy,MaxY,thetaMin,thetaMax;
	G4bool Initialized;
	static G4ThreadLocal TF2* SigmaFunc;
	TGenPhaseSpace ps;

	TLorentzVector cms,beam,target;

	//Definition of particles. particles[0] carries the kinetic energy.
	std::vector<G4ParticleDefinition*> particles;

	G4GenericMessenger* fMessenger;

	virtual void DefineCommands();
	//Returns a the TF2 for hit and miss.
	virtual TF2* BuildFunction()=0;
	virtual void Initialize()=0;
	std::vector<G4int> myTupleId;
	G4ParticleGun* pGun;

};

#endif /* INCLUDE_PHASESPACEGENERATOR_HH_ */
