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
	fAbsorberLength=5*CLHEP::cm;
	fNumLayers=1;
	DefineCommands();
}

JediSandwichCalorimeter::~JediSandwichCalorimeter() {
}


G4LogicalVolume* JediSandwichCalorimeter::MakeCaloCrystal() {

	auto motherLength=fCrystalLength;

	auto detectorLength=(fCrystalLength-fAbsorberLength)/fNumLayers;
	auto solidDetector= new G4Box("Scintillator",fCrystalWidth/2,fCrystalWidth/2,detectorLength/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Scintillator");

	auto solidMother= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,motherLength/2);
	auto logicMother=new G4LogicalVolume(solidMother,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Detector");
	if(fAbsorberLength>1e-3*CLHEP::mm){
	  auto solidAbsorber= new G4Box("Absorber",(fCrystalWidth)/2,(fCrystalWidth)/2,fAbsorberLength/fNumLayers/2);
	  auto logicAbsorber = new G4LogicalVolume(solidAbsorber,fAbsorberMaterial,"Absorber");
	  logicAbsorber->SetVisAttributes(new G4VisAttributes(blue));
	  this->fCaloSDVolumes["Absorber"]=logicAbsorber;
	  for(G4int iLayer=0;iLayer<fNumLayers;iLayer++)
	    new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+iLayer*motherLength/fNumLayers+fAbsorberLength/2),logicAbsorber,"Absorber",logicMother,false,iLayer,false);
	}
	 for(G4int iLayer=0;iLayer<fNumLayers;iLayer++)
	    new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+iLayer*motherLength/fNumLayers+fAbsorberLength+detectorLength/2),logicDetector,"Scintillator",logicMother,false,iLayer,false);

	logicDetector->SetVisAttributes(new G4VisAttributes(red));
	logicMother->SetVisAttributes(G4VisAttributes::Invisible);

	this->fCaloSDVolumes["Calorimeter"]=logicDetector;
	return logicMother;
}

void JediSandwichCalorimeter::DefineCommands() {
	fMessenger->DeclareMethod("numLayers",&JediSandwichCalorimeter::setNumLayers,"set number of layers");
	fMessenger->DeclareMethodWithUnit("absorberlength","mm",&JediSandwichCalorimeter::setAbsorberLength,"set absorber length");
	fMessenger->DeclareMethod("absorber",&JediSandwichCalorimeter::setAbsorberMaterialName,"set absorber material");

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
	auto aCrystal=MakeCaloCrystal();
	//aCrystal->SetVisAttributes(new G4VisAttributes(green));

	if(fHodoscopeShape=="pizza"){
		auto placement=G4ThreeVector(0,0,DetectorZ-fDeltaELength/2);
		auto solidSlice=new G4Tubs("DeltaE",fInnerDetectorRadius,fOuterDetectorRadius,fDeltaELength/4,10*CLHEP::deg,10*CLHEP::deg);
		auto aDetectorElement=new G4LogicalVolume(solidSlice,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Hodoscope");
		fCaloSDVolumes["Hodoscope"]=aDetectorElement;
		for(int iSlice=0;iSlice<36;iSlice++){
			auto rot1=new G4RotationMatrix();
			rot1->rotateZ(iSlice*10*CLHEP::deg);
			new G4PVPlacement (rot1, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, iSlice);

		}
		placement=G4ThreeVector(0,0,DetectorZ-fDeltaELength);
		for(int iSlice=0;iSlice<36;iSlice++){
			auto rot2=new G4RotationMatrix();
			rot2->rotateZ(iSlice*10*CLHEP::deg+5*CLHEP::deg);
			new G4PVPlacement (rot2, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, iSlice+36, false);
		}

	}
	else if(fHodoscopeShape=="square"){
		auto aDeltaETile=MakeDetector("DeltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
		aDeltaETile->SetVisAttributes(new G4VisAttributes(cyan));
		fCaloSDVolumes["Hodoscope"]=aDeltaETile;
		PlaceHodoscope(aDeltaETile);
	}
	else{
		std::stringstream description;
		description<<"hodoscope shape: "<< fHodoscopeShape<<" not found.";
		G4Exception("JediCubicPolarimeter::Construct()","",FatalException,description.str().c_str());
	}

	PlaceCalorimeter(aCrystal);
	return fPhysiWorld;
}
