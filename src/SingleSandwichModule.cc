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
	fAbsorberFraction=0.25;
	fNumLayers=1;
	DefineCommands();

}

SingleSandwichModule::~SingleSandwichModule() {}

G4LogicalVolume* SingleSandwichModule::MakeCaloCrystal() {
	assert(1-fAbsorberFraction);
	auto absorberLength=fAbsorberFraction*fCrystalLength/fNumLayers;
	auto detectorLength=(1-fAbsorberFraction)*fCrystalLength/fNumLayers;
	auto solidDetector= new G4Box("Scintillator",(fCrystalWidth)/2,(fCrystalWidth)/2,detectorLength/2);
	auto solidAbsorber= new G4Box("Absorber",(fCrystalWidth)/2,(fCrystalWidth)/2,absorberLength/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Scintillator");
	auto logicAbsorber = new G4LogicalVolume(solidAbsorber,fAbsorberMaterial,"Absorber");

	auto solidCrystal= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2);
	auto logicCrystal=new G4LogicalVolume(solidCrystal,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Crystal");

	for(G4int iLayer=0;iLayer<fNumLayers;iLayer++){
		new G4PVPlacement(0,G4ThreeVector(0,0,-fCrystalLength/2+detectorLength/2+iLayer*fCrystalLength/fNumLayers),logicDetector,"Detector",logicCrystal,false,iLayer,false);
		new G4PVPlacement(0,G4ThreeVector(0,0,-fCrystalLength/2+detectorLength+absorberLength/2+iLayer*fCrystalLength/fNumLayers),logicAbsorber,"Absorber",logicCrystal,false,iLayer,false);
	}
	logicDetector->SetVisAttributes(new G4VisAttributes(red));
	logicAbsorber->SetVisAttributes(new G4VisAttributes(blue));
	logicCrystal->SetVisAttributes(G4VisAttributes::Invisible);

	this->fTrackerSDVolumes["Calorimeter"]=logicDetector;
	this->fCaloSDVolumes["Absorber"]=logicAbsorber;

	return logicCrystal;
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
	new G4PVPlacement (rot, G4ThreeVector(0,0,fCrystalLength/2), aCrystal, "Crystal", fLogicWorld, false, 0, false);

	return fPhysiWorld;
}

void SingleSandwichModule::DefineCommands() {
	fMessenger->DeclareMethod("numLayers",&SingleSandwichModule::setNumLayers,"set number of layers");
	fMessenger->DeclareMethod("absorberfraction",&SingleSandwichModule::setAbsorberFraction,"set Labs/Ltot");
	fMessenger->DeclareMethod("absorber",&SingleSandwichModule::setAbsorberMaterialName,"set absorber material");

}
