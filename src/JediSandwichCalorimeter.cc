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
	fAbsorberMaterial=G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
	fCrystalLength=10*CLHEP::cm;
}

JediSandwichCalorimeter::~JediSandwichCalorimeter() {
}

void JediSandwichCalorimeter::DefineCommands() {
}

G4LogicalVolume* JediSandwichCalorimeter::MakeCaloCrystal() {
	G4Box* solidDetector= new G4Box("Scintillator",(fCrystalWidth)/2,(fCrystalWidth)/2,(fCrystalLength/20)/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Scintillator");
	G4LogicalVolume* logicAbsorber = new G4LogicalVolume(solidDetector,fAbsorberMaterial,"Absorber");

	G4Box* solidCrystal= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2);
	G4LogicalVolume* logicCrystal=new G4LogicalVolume(solidCrystal,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Crystal");

	for(size_t iPos=0;iPos<10;iPos++){
		G4double z=-fCrystalLength/2+fCrystalLength/40+2*iPos*fCrystalLength/20;
		new G4PVPlacement(0,G4ThreeVector(0,0,z),logicDetector,"Detector",logicCrystal,false,iPos,false);
		new G4PVPlacement(0,G4ThreeVector(0,0,z+fCrystalLength/20),logicAbsorber,"Absorber",logicCrystal,false,iPos,false);

	}
	logicDetector->SetVisAttributes(G4VisAttributes::Invisible);
	logicAbsorber->SetVisAttributes(G4VisAttributes::Invisible);
	fCaloSDVolumes["CaloCrystal"]=logicDetector;

	return logicCrystal;
}

G4VPhysicalVolume* JediSandwichCalorimeter::Construct() {
	JediPolarimeter::Construct();
	if(fInfileName!=""){
		std::ifstream ifile(fInfileName);
		std::string line;
		//read until first non-comment line, then break loop
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			else{
				std::istringstream(line)>>fCrystalWidth>>fCrystalWidth>>fCrystalLength;
				break;
			}
		}
		G4LogicalVolume* aCrystal=MakeCaloCrystal();
		double placementX,placementY,placementZ;
		int copyNo=0;
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			std::istringstream(line)>>copyNo>>placementX>>placementY>>placementZ;
			new G4PVPlacement (0, G4ThreeVector(placementX*CLHEP::mm,placementY*CLHEP::mm,placementZ*CLHEP::mm), aCrystal, "Crystal", fLogicWorld, false, copyNo, false);
		}
		return fPhysiWorld;
	}

	int ii=0;
	fGeomCache.clear();
	std::stringstream buf;
	buf<<fCrystalWidth<<" "<<fCrystalWidth<<" "<<" "<<fCrystalLength;
	fGeomCache.push_back(buf.str());
	buf.clear();
	buf.str(std::string());
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4LogicalVolume* aDeltaETile=MakeDeltaECrystal();
	for(int iCrystalX=-fMaxCrystal-20; iCrystalX<fMaxCrystal+20;iCrystalX++){
		for(int iCrystalY=-fMaxCrystal-20; iCrystalY<fMaxCrystal+20;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*fCrystalWidth,iCrystalY*fCrystalWidth,DetectorZ+0.5*fCrystalLength);
			auto dePlacement=G4ThreeVector(iCrystalX*fCrystalWidth,iCrystalY*fCrystalWidth,DetectorZ-0.5*fDeltaELength);
			if((placement.perp()-fCrystalWidth/CLHEP::mm/sqrt(2))<fInnerDetectorRadius or (placement.perp()-fCrystalWidth/CLHEP::mm/sqrt(2))>fOuterDetectorRadius)
				continue;
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;

			G4int copyNo=100*(iCrystalX+20)+iCrystalY+20;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", fLogicWorld, false, copyNo, false);
			buf<<copyNo<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
			new G4PVPlacement (0, dePlacement, aDeltaETile, "Tile", fLogicWorld, false, copyNo, false);
		}
	}
	return fPhysiWorld;
}
