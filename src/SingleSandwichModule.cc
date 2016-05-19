/*
 * SingleSandwichModule.cc
 *
 *  Created on: Nov 3, 2015
 *      Author: maanen
 */

#include "SingleSandwichModule.hh"
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
SingleSandwichModule::SingleSandwichModule():SingleCrystal() {
	fAbsorberMaterial=G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
	fCrystalLength=10*CLHEP::cm;
	fAbsorberLength=5*CLHEP::cm;
	fDeLength=1*CLHEP::cm;
	fNumLayers=1;
	DefineCommands();

}

SingleSandwichModule::~SingleSandwichModule() {}

G4LogicalVolume* SingleSandwichModule::MakeCaloCrystal() {

	auto motherLength=fCrystalLength+fAbsorberLength+fDeLength;
	auto solidDetector= new G4Box("Scintillator",fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Detector");

	auto solidMother= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,motherLength/2);
	auto logicMother=new G4LogicalVolume(solidMother,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Crystal");
	if(fAbsorberLength>1e-3*CLHEP::mm){		 
		auto solidAbsorber= new G4Box("Absorber",(fCrystalWidth)/2,(fCrystalWidth)/2,fAbsorberLength/fNumLayers/2);
		auto logicAbsorber = new G4LogicalVolume(solidAbsorber,fAbsorberMaterial,"Absorber");
		logicAbsorber->SetVisAttributes(new G4VisAttributes(blue));
		new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+fDeLength+fAbsorberLength/2),logicAbsorber,"Absorber",logicMother,false,0,false);
	}
	new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+fDeLength+fAbsorberLength+fCrystalLength/2),logicDetector,"Detector",logicMother,false,0,false);
	auto solidDe=new G4Box("deltaE",fCrystalWidth/2,fCrystalWidth/2,fDeLength/2);
	auto logicDe = new G4LogicalVolume(solidDe,fScintillatorMaterial,"Detector");
	new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+fDeLength/2),logicDe,"deltaE",logicMother,false,0,false);
	logicDetector->SetVisAttributes(new G4VisAttributes(red));
	logicMother->SetVisAttributes(G4VisAttributes::Invisible);
	//fCaloSDVolumes["Mother"].push_back(logicMother);
	fCaloSDVolumes["Calorimeter"].push_back(logicDetector);
	fCaloSDVolumes["DeltaE"].push_back(logicDe);
	return logicMother;
}

G4VPhysicalVolume* SingleSandwichModule::Construct() {
	if(fChangedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	//	auto worldRegion = new G4Region("calorimeter");
	//	worldRegion->AddRootLogicalVolume(aCrystal);
	//	worldRegion->SetUserLimits(new G4UserLimits(100.0 * CLHEP::mm,1000*CLHEP::mm,100*CLHEP::ns,0,0));

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,(fCrystalLength+fAbsorberLength+fDeLength)/2-2*CLHEP::mm), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	auto solidExitWindow=new G4Tubs("exitWindow",0,5*CLHEP::cm,100*CLHEP::um,0,2*CLHEP::pi*CLHEP::rad);
	auto logicExitWindow=new G4LogicalVolume(solidExitWindow,G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe"),"exitWindow");
	logicExitWindow->SetVisAttributes(new G4VisAttributes(gray));
	new G4PVPlacement(0,G4ThreeVector(0,0,-1*CLHEP::cm),logicExitWindow,"exitWindow",fLogicWorld,false,0,false);
	return fPhysiWorld;
}

void SingleSandwichModule::DefineCommands() {
	fMessenger->DeclareMethod("numLayers",&SingleSandwichModule::setNumLayers,"set number of layers");
	fMessenger->DeclareMethodWithUnit("absorberlength","mm",&SingleSandwichModule::setAbsorberLength,"set absorber length");
	fMessenger->DeclareMethod("absorber",&SingleSandwichModule::setAbsorberMaterialName,"set absorber material");

}
