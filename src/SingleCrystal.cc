/*
 * SingleCrystal.cc
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */
#include "SingleCrystal.hh"
#include <G4UserLimits.hh>
#include <G4Region.hh>
#include "Colors.hh"
SingleCrystal::SingleCrystal():JediPolarimeter(),fTheta(0),fPhi(0),fPsi(0) {
	fHCalSizeXY=3*CLHEP::cm;
	fHCalSizeZ=8*CLHEP::cm;

	DefineCommands();
}

SingleCrystal::~SingleCrystal() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* SingleCrystal::BuildCaloCrystal() {
	auto solidDetector= new G4Box("Detector",fHCalSizeZ/2,fHCalSizeZ/2,fHCalSizeXY/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,fHCalMaterial,"Detector");
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	fSensitiveDetectors.Update("Detector",SDtype::kCalorimeter,logVolVector{logicDetector});
	return logicDetector;
}

G4VPhysicalVolume* SingleCrystal::Construct() {
	if(fGeometryHasBeenChanged)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	auto detector=BuildVolume<G4Box>("Detector",G4NistManager::Instance()->FindOrBuildMaterial("LYSO"),fHCalSizeXY/2,fHCalSizeXY/2,fHCalSizeZ/2);
	//		auto worldRegion = new G4Region("calorimeter");
	//		worldRegion->AddRootLogicalVolume(aCrystal);
	//		worldRegion->SetUserLimits(new G4UserLimits(10.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	detector->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	G4RotationMatrix* rot=new G4RotationMatrix();
	G4ThreeVector pos(0,0,fHCalSizeXY/2);
	rot->set(fPhi,fTheta,fPsi);
	fSensitiveDetectors.Update("Detector",SDtype::kCalorimeter,logVolVector{detector});
	new G4PVPlacement (rot, pos, detector, "Target", fLogicWorld, false, 0, false);

	return fPhysiWorld;
}

G4LogicalVolume* SingleCrystal::MakeDetector(G4String name, G4Material* mat,G4double halfSizeX, G4double halfSizeY, G4double halfSizeZ) {
	auto solidDetector= new G4Box("Detector",halfSizeX,halfSizeY,halfSizeZ);
	auto logicDetector = new G4LogicalVolume(solidDetector,mat,name);
	return logicDetector;
}

void SingleCrystal::DefineCommands() {
	JediPolarimeter::DefineCommands();

	fMessenger->DeclareMethodWithUnit("theta","deg",
			&SingleCrystal::setTheta,
			"set theta");

	fMessenger->DeclareMethodWithUnit("phi","deg",
			&SingleCrystal::setPhi,
			"set phi");

	fMessenger->DeclareMethodWithUnit("psi","deg",
			&SingleCrystal::setPsi,
			"set psi");
}
