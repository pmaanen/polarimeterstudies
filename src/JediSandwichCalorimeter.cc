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
			double placementX,placementY,placementZ,x,y,z;
			G4String name="",matName;
			int copyNo=0;
			while(std::getline(ifile, line)){
				if(line[0]=='#') continue;
				std::istringstream(line)>>copyNo>>name>>matName>>x>>y>>z>>placementX>>placementY>>placementZ;
				if(!fCaloSDVolumes[name]){
					fCaloSDVolumes[name]=MakeDetector(name,G4NistManager::Instance()->FindOrBuildMaterial(matName),x,y,z);;
				}
				new G4PVPlacement (0, G4ThreeVector(placementX*CLHEP::mm,placementY*CLHEP::mm,placementZ*CLHEP::mm),fCaloSDVolumes[name] , name, fLogicWorld, false, copyNo, false);
			}
			return fPhysiWorld;
		}

		fGeomCache.clear();
		auto aCrystal=MakeDetector("Calorimeter",G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName),fCrystalWidth,fCrystalWidth,fCrystalLength);
		aCrystal->SetVisAttributes(new G4VisAttributes(green));
		fCaloSDVolumes["Calorimeter"]=aCrystal;
		auto aDeltaETile=MakeDetector("DeltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
		aDeltaETile->SetVisAttributes(new G4VisAttributes(cyan));
		fCaloSDVolumes["DeltaE"]=aDeltaETile;
		PlaceCalorimeter(aCrystal);
		PlaceHodoscope(aDeltaETile);
		return fPhysiWorld;
}
