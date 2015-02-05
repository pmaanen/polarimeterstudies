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

class G4ParticleDefinition;
class G4GenericMessenger;

typedef std::vector<std::pair<G4ParticleDefinition*,G4ThreeVector> > ParticleMomentumVector;

class PhaseSpaceGenerator {
public:
	PhaseSpaceGenerator();
	virtual ~PhaseSpaceGenerator();
};

#endif /* INCLUDE_PHASESPACEGENERATOR_HH_ */
