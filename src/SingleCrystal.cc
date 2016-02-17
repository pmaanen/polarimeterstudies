/*
 * SingleCrystal.cc
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */
#include "SingleCrystal.hh"
#include <G4UserLimits.hh>
#include <G4Region.hh>


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
SingleCrystal::SingleCrystal():JediPolarimeter(),fTheta(0),fPhi(0),fPsi(0) {
	fCrystalLength=10*CLHEP::cm;
	fCrystalWidth=3*CLHEP::cm;

	DefineCommands();
}

SingleCrystal::~SingleCrystal() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* SingleCrystal::MakeCaloCrystal() {
	/*
	G4Box* solidWrapping= new G4Box("Wrapping",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Wrapping",(crystalWidth-1*wrappingThickness)/2,(crystalWidth-1*wrappingThickness)/2,(crystalLength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	 */

	G4Box* solidDetector= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Detector");
	/*
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	 */
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	fCaloSDVolumes["Calorimeter"]=logicDetector;
	return logicDetector;
}

G4VPhysicalVolume* SingleCrystal::Construct() {
	if(fChangedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
//		auto worldRegion = new G4Region("calorimeter");
//		worldRegion->AddRootLogicalVolume(aCrystal);
//		worldRegion->SetUserLimits(new G4UserLimits(10.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	aCrystal->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,fCrystalLength/2), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	return fPhysiWorld;
}

void SingleCrystal::DefineCommands() {
	JediPolarimeter::DefineCommands();

	G4GenericMessenger::Command& thetaCmd
	= fMessenger->DeclareMethodWithUnit("theta","deg",
			&SingleCrystal::setTheta,
			"set theta");

	G4GenericMessenger::Command& phiCmd
	= fMessenger->DeclareMethodWithUnit("phi","deg",
			&SingleCrystal::setPhi,
			"set phi");

	G4GenericMessenger::Command& psiCmd
	= fMessenger->DeclareMethodWithUnit("psi","deg",
			&SingleCrystal::setPsi,
			"set psi");
}
