/*
 * JediCalorimeterHit.cc
 *
 *  Created on: 04.03.2017
 *      Author: pmaanen
 */

#include <JediCalorimeterHit.hh>

G4ThreadLocal G4Allocator<JediCalorimeterHit> *JediCalorimeterHitAllocator = 0;

JediCalorimeterHit::JediCalorimeterHit() {
	fEdep=0;
	fDetId=0;
}

void JediCalorimeterHit::Draw() {
}

void JediCalorimeterHit::Print() {
}
