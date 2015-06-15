/*
 * SandwichCalorimeter.cc
 *
 *  Created on: 21.04.2015
 *      Author: pmaanen
 */

#include <SandwichCalorimeter.hh>
static G4Colour
white   (1.0, 1.0, 1.0),  // white
gray    (0.5, 0.5, 0.5), // gray
black   (0.0, 0.0, 0.0), // black
red     (1.0, 0.0, 0.0), // red
green   (0.0, 1.0, 0.0), // green
blue    (0.0, 0.0, 1.0), // blue
cyan    (0.0, 1.0, 1.0), // cyan
magenta (1.0, 0.0, 1.0), // magenta
yellow  (1.0, 1.0, 0.0); // yellow
SandwichCalorimeter::SandwichCalorimeter():SingleCrystal() {
	absorberMaterial=G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
	crystalLength=10*CLHEP::cm;
}

SandwichCalorimeter::~SandwichCalorimeter() {
}

void SandwichCalorimeter::DefineCommands() {
}

G4LogicalVolume* SandwichCalorimeter::MakeCaloCrystal() {
	G4Box* solidDetector= new G4Box("Scintillator",(crystalWidth)/2,(crystalWidth)/2,(crystalLength/20)/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,scintillatorMaterial,"Scintillator");
	G4LogicalVolume* logicAbsorber = new G4LogicalVolume(solidDetector,absorberMaterial,"Absorber");

	G4Box* solidCrystal= new G4Box("Detector",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume* logicCrystal=new G4LogicalVolume(solidCrystal,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Crystal");

	for(size_t iPos=0;iPos<10;iPos++){
		G4double z=-crystalLength/2+crystalLength/40+2*iPos*crystalLength/20;
		new G4PVPlacement(0,G4ThreeVector(0,0,z),logicDetector,"Detector",logicCrystal,false,iPos,false);
		new G4PVPlacement(0,G4ThreeVector(0,0,z+crystalLength/20),logicAbsorber,"Absorber",logicCrystal,false,iPos,false);

	}
	G4VisAttributes* absorberVisAttr=new G4VisAttributes(blue);
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	logicAbsorber->SetVisAttributes(absorberVisAttr);
	logicalVolumes["CaloCrystal"]=logicDetector;
	G4cout<<G4BestUnit(logicCrystal->GetMass(),"Mass")<<G4endl;
	return logicCrystal;
}

G4VPhysicalVolume* SandwichCalorimeter::Construct() {
	SingleCrystal::Construct();
	return physiWorld;
}
