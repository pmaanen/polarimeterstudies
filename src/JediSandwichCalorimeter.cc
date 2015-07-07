/*
 * SandwichCalorimeter.cc
 *
 *  Created on: 21.04.2015
 *      Author: pmaanen
 */

#include <JediSandwichCalorimeter.hh>
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
JediSandwichCalorimeter::JediSandwichCalorimeter():JediCubicPolarimeter() {
	absorberMaterial=G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
	crystalLength=10*CLHEP::cm;
}

JediSandwichCalorimeter::~JediSandwichCalorimeter() {
}

void JediSandwichCalorimeter::DefineCommands() {
}

G4LogicalVolume* JediSandwichCalorimeter::MakeCaloCrystal() {
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
	logicDetector->SetVisAttributes(G4VisAttributes::Invisible);
	logicAbsorber->SetVisAttributes(G4VisAttributes::Invisible);
	caloSDVolumes["CaloCrystal"]=logicDetector;

	return logicCrystal;
}

G4VPhysicalVolume* JediSandwichCalorimeter::Construct() {
	JediPolarimeter::Construct();
	if(infile!=""){
		std::ifstream ifile(infile);
		std::string line;
		//read until first non-comment line, then break loop
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			else{
				std::istringstream(line)>>crystalWidth>>crystalWidth>>crystalLength;
				break;
			}
		}
		G4LogicalVolume* aCrystal=MakeCaloCrystal();
		double placementX,placementY,placementZ;
		int copyNo=0;
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			std::istringstream(line)>>copyNo>>placementX>>placementY>>placementZ;
			new G4PVPlacement (0, G4ThreeVector(placementX*CLHEP::mm,placementY*CLHEP::mm,placementZ*CLHEP::mm), aCrystal, "Crystal", logicWorld, false, copyNo, false);
		}
		return physiWorld;
	}

	int ii=0;
	geomCache.clear();
	std::stringstream buf;
	buf<<crystalWidth<<" "<<crystalWidth<<" "<<" "<<crystalLength;
	geomCache.push_back(buf.str());
	buf.clear();
	buf.str(std::string());
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4LogicalVolume* aDeltaETile=MakeDeltaECrystal();
	for(int iCrystalX=-MaxCrystal-20; iCrystalX<MaxCrystal+20;iCrystalX++){
		for(int iCrystalY=-MaxCrystal-20; iCrystalY<MaxCrystal+20;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,detectorZ+0.5*crystalLength);
			auto dePlacement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,detectorZ-0.5*deltaELength);
			if((placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))<innerDetectorRadius or (placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))>outerDetectorRadius)
				continue;
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;

			G4int copyNo=100*(iCrystalX+20)+iCrystalY+20;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", logicWorld, false, copyNo, false);
			buf<<copyNo<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			geomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
			new G4PVPlacement (0, dePlacement, aDeltaETile, "Tile", logicWorld, false, copyNo, false);
		}
	}
	return physiWorld;
}
