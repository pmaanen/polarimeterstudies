/*
 * JediCalorimeterHit.cc
 *
 *  Created on: 04.03.2017
 *      Author: pmaanen
 */

#include <JediCalorimeterHit.hh>

G4ThreadLocal G4Allocator<JediCalorimeterHit> *JediCalorimeterHitAllocator = 0;

JediCalorimeterHit::JediCalorimeterHit():G4VHit(),fEdep(0),fDetId(0),fPos(G4ThreeVector()) {
}

void JediCalorimeterHit::Draw() {
}

void JediCalorimeterHit::Print() {
}
