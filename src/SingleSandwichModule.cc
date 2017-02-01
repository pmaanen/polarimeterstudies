/*
 * SingleSandwichModule.cc
 *
 *  Created on: Nov 3, 2015
 *      Author: maanen
 */

#include "SingleSandwichModule.hh"
#include <G4UserLimits.hh>
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
	fHCalSizeXY=5*CLHEP::cm;
	fAbsorberLength=3*CLHEP::cm;
	fDeltaELength=2*CLHEP::cm;
	fNumLayers=1;
	DefineCommands();

}

SingleSandwichModule::~SingleSandwichModule() {}

G4LogicalVolume* SingleSandwichModule::MakeCaloCrystal() {

	auto motherLength=fHCalSizeXY+fAbsorberLength+fDeltaELength;
	auto solidDetector= new G4Box("Scintillator",fHCalSizeZ/2,fHCalSizeZ/2,fHCalSizeXY/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,fHCalMaterial,"Detector");

	auto solidMother= new G4Box("Detector",fHCalSizeZ/2,fHCalSizeZ/2,motherLength/2);
	auto logicMother=new G4LogicalVolume(solidMother,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Crystal");
	if(fAbsorberLength>1e-3*CLHEP::mm){		 
		auto solidAbsorber= new G4Box("Absorber",(fHCalSizeZ)/2,(fHCalSizeZ)/2,fAbsorberLength/fNumLayers/2);
		auto logicAbsorber = new G4LogicalVolume(solidAbsorber,fAbsorberMaterial,"Absorber");
		logicAbsorber->SetVisAttributes(new G4VisAttributes(gray));
		fSensitiveDetectors.Update("Absorber",SDtype::kCalorimeter,logVolVector{logicAbsorber});
		new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+fDeltaELength+fAbsorberLength/2),logicAbsorber,"Absorber",logicMother,false,0,false);
	}
	new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+fDeltaELength+fAbsorberLength+fHCalSizeXY/2),logicDetector,"Detector",logicMother,false,0,false);
	auto solidDe=new G4Box("Hodoscope",fDeltaEWidth/2,fDeltaEWidth/2,fDeltaELength/4);
	auto logicDe = new G4LogicalVolume(solidDe,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Hodoscope");
	new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+fDeltaELength/4),logicDe,"Hodoscope",logicMother,false,0,false);
	new G4PVPlacement(0,G4ThreeVector(0,0,-motherLength/2+3*fDeltaELength/4),logicDe,"Hodoscope",logicMother,false,1,false);
	logicDe->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	logicDetector->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	logicDetector->SetVisAttributes(new G4VisAttributes(green));
	logicDe->SetVisAttributes(new G4VisAttributes(cyan));
	logicMother->SetVisAttributes(G4VisAttributes::Invisible);
	fSensitiveDetectors.Update("Detector",SDtype::kCalorimeter,logVolVector{logicDetector});
	fSensitiveDetectors.Update("Hodoscope",SDtype::kCalorimeter,logVolVector{logicDe});
	return logicMother;
}

G4VPhysicalVolume* SingleSandwichModule::Construct() {
	if(fGeometryHasBeenChanged)
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
	new G4PVPlacement (rot, G4ThreeVector(0,0,(fHCalSizeXY+fAbsorberLength+fDeltaELength)/2-fDeltaELength), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	/*
	auto solidExitWindow=new G4Tubs("exitWindow",0,5*CLHEP::cm,100*CLHEP::um,0,2*CLHEP::pi*CLHEP::rad);
	auto logicExitWindow=new G4LogicalVolume(solidExitWindow,G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe"),"exitWindow");
	logicExitWindow->SetVisAttributes(new G4VisAttributes(gray));
	new G4PVPlacement(0,G4ThreeVector(0,0,-1*CLHEP::cm),logicExitWindow,"exitWindow",fLogicWorld,false,0,false);
	*/
	return fPhysiWorld;
}

void SingleSandwichModule::DefineCommands() {
	fMessenger->DeclareMethod("numLayers",&SingleSandwichModule::setNumLayers,"set number of layers");
	fMessenger->DeclareMethodWithUnit("absorberlength","mm",&SingleSandwichModule::setAbsorberLength,"set absorber length");
	fMessenger->DeclareMethod("absorber",&SingleSandwichModule::setAbsorberMaterialName,"set absorber material");

}
