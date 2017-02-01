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
	fHCalSizeXY=10*CLHEP::cm;
	fHCalSizeZ=3*CLHEP::cm;

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

	auto solidDetector= new G4Box("Detector",fHCalSizeZ/2,fHCalSizeZ/2,fHCalSizeXY/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,fHCalMaterial,"Detector");
	//auto solidEnvelope=new G4Box("solidEnv1",fCrystalWidth/2+1*CLHEP::mm,fCrystalWidth/2+1*CLHEP::mm,fCrystalLength/2+1*CLHEP::mm);
	//auto solidEnv2= new G4Box("solidEnv2",fCrystalWidth/2+1*CLHEP::mm,fCrystalWidth/2+1*CLHEP::mm,1*CLHEP::mm);
	//auto solidEnvelope=new G4SubtractionSolid("solidEnvelope",solidEnv1,solidEnv2,0,G4ThreeVector(0,0,-fCrystalLength/2-.5*CLHEP::mm));
	//G4LogicalVolume* logicEnvelope = new G4LogicalVolume(solidEnvelope,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Envelope");
	//new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"Detector",logicEnvelope,false,0,false);



	/*
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	 */
	G4cout<<"logicDetector="<<logicDetector<<G4endl;
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
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	//		auto worldRegion = new G4Region("calorimeter");
	//		worldRegion->AddRootLogicalVolume(aCrystal);
	//		worldRegion->SetUserLimits(new G4UserLimits(10.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	aCrystal->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,fHCalSizeXY/2), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	return fPhysiWorld;
}

G4LogicalVolume* SingleCrystal::MakeDetector(G4String name, G4Material* mat,G4double halfSizeX, G4double halfSizeY, G4double halfSizeZ) {
	auto solidDetector= new G4Box("Detector",halfSizeX,halfSizeY,halfSizeZ);
	auto logicDetector = new G4LogicalVolume(solidDetector,mat,name);
	return logicDetector;
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
