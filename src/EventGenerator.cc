/*
 * EventGenerator.cc
 *
 *  Created on: 27.04.2015
 *      Author: pmaanen
 */

#include <EventGenerator.hh>
#include "G4ParticleGun.hh"
EventGenerator::EventGenerator(G4ParticleGun* pgun):fParticleGun(pgun),fRunInitialized(false) {}

EventGenerator::~EventGenerator() {}

