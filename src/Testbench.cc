/*
 * CosmicSetup.cc
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <Analysis.hh>
#include <Testbench.hh>
Testbench::Testbench():SingleCrystal(),fLogicTrigger(0),fTriggerOffsetX(0),fTriggerOffsetY(0),fTriggerOffsetZ(0) {
	fHCalSizeXY=10*CLHEP::cm;
	fHCalSizeZ=3*CLHEP::cm;
	fTriggerHeight=2.5*CLHEP::cm;
	fTriggerWidth=2.5*CLHEP::cm;
	fTriggerThickness=.5*CLHEP::cm;
	fTrigger=true;
	fScintillatorMaterialName="LYSO";
	fHCalMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);

	DefineCommands();
}

G4VPhysicalVolume* Testbench::Construct() {
	if(fGeometryHasBeenChanged)
		ComputeParameters();

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	//fSensitiveDetectors.Update("Detector",SDtype::kCalorimeter,{aCrystal});
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,0), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	if(fTriggerWidth>0 and fTriggerHeight>0 and fTriggerThickness>0){
		G4Box* solidTrigger=new G4Box("Trigger",fTriggerWidth/2,fTriggerHeight/2,fTriggerThickness/2);
		auto triggerRot=new G4RotationMatrix();
		auto logicTrigger=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Trigger");
		logicTrigger->SetVisAttributes(new G4VisAttributes(G4Colour(1.,0.,0.)));
		new G4PVPlacement (triggerRot, G4ThreeVector(0,0,-15*CLHEP::cm), logicTrigger, "Trigger", fLogicWorld, false, 0, false);
		new G4PVPlacement (triggerRot, G4ThreeVector(0,0,-10*CLHEP::cm), logicTrigger, "Trigger", fLogicWorld, false, 1, false);
		new G4PVPlacement (triggerRot, G4ThreeVector(0,0,15*CLHEP::cm), logicTrigger, "Trigger", fLogicWorld, false, 2, false);
		fSensitiveDetectors.Update("Trigger",SDtype::kCalorimeter,{logicTrigger});
	}
		return fPhysiWorld;
	}

	void Testbench::DefineCommands() {

		SingleCrystal::DefineCommands();
fMessenger->DeclareMethodWithUnit("triggerlength","mm",
				&Testbench::setTriggerLength,
				"trigger length (mm)");


fMessenger->DeclareMethodWithUnit("triggerthickness","mm",
				&Testbench::setTriggerThickness,
				"trigger thickness (mm)");

		G4GenericMessenger::Command& triggerWidthCmd
		= fMessenger->DeclareMethodWithUnit("triggerwidth","mm",
				&Testbench::setTriggerWidth,
				"trigger width (mm)");

		triggerWidthCmd.SetParameterName("width", true);
		triggerWidthCmd.SetRange("width>=0.");
		triggerWidthCmd.SetDefaultValue("30.");

fMessenger->DeclareMethodWithUnit("trgOffsetX","mm",
				&Testbench::setTriggerOffsetX,
				"trigger offset in x dir. (mm)");

fMessenger->DeclareMethodWithUnit("trgOffsetY","mm",
				&Testbench::setTriggerOffsetY,
				"trigger offset in y dir. (mm)");

fMessenger->DeclareMethodWithUnit("trgOffsetZ","mm",
				&Testbench::setTriggerOffsetZ,
				"trigger offset in z dir. (mm)");


	}
