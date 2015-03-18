/*
 * SingleCrystal.cc
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#include <SingleCrystal.hh>
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
SingleCrystal::SingleCrystal() {
	crystalLength=500*CLHEP::cm;
	crystalWidth=10*CLHEP::cm;

	G4cout<<"SingleCrystal::SingleCrystal()"<<G4endl;
}

SingleCrystal::~SingleCrystal() {
	// TODO Auto-generated destructor stub
}

void SingleCrystal::DefineCommands() {
}

G4LogicalVolume* SingleCrystal::MakeCaloCrystal() {
	G4Box* solidWrapping= new G4Box("Wrapping",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Wrapping",(crystalWidth-1*wrappingThickness)/2,(crystalWidth-1*wrappingThickness)/2,(crystalLength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	G4Box* solidDetector= new G4Box("Detector",(crystalWidth-2*wrappingThickness)/2,(crystalWidth-2*wrappingThickness)/2,(crystalLength-2*wrappingThickness)/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,scintillatorMaterial,"Detector");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	logicCaloCrystal=logicDetector;
	return logicWrapping;
}

G4VPhysicalVolume* SingleCrystal::Construct() {
	if(changedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",worldSizeXY/2,worldSizeXY/2,worldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	new G4PVPlacement (0, G4ThreeVector(0,0,crystalLength/2), aCrystal, "Crystal", logicWorld, false, 0, false);

	return physiWorld;
}
