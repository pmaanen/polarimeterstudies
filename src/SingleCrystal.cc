/*
 * SingleCrystal.cc
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#include <SingleCrystal.hh>

SingleCrystal::SingleCrystal() {
	// TODO Auto-generated constructor stub

}

SingleCrystal::~SingleCrystal() {
	// TODO Auto-generated destructor stub
}

void SingleCrystal::DefineCommands() {
}

G4LogicalVolume* SingleCrystal::MakeDetector() {
}

G4VPhysicalVolume* SingleCrystal::Construct() {

	G4LogicalVolume* aCrystal=MakeDetector();
	new G4PVPlacement (0, G4ThreeVector(0,0,crystalLength), aCrystal, "Crystal", logicWorld, false, 0, false);

	return physiWorld;
}
