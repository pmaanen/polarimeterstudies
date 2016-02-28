/*
 * EventGenerator.cc
 *
 *  Created on: 27.04.2015
 *      Author: pmaanen
 */

#include "EventGenerator.hh"
#include "G4ParticleGun.hh"
#include <G4GenericMessenger.hh>
EventGenerator::EventGenerator(G4ParticleGun* pgun):fParticleGun(pgun),fRunInitialized(false),fMessenger(0) {}

EventGenerator::~EventGenerator() {
	delete fMessenger;
}

