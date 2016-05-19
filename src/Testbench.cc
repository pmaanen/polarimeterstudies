/*
 * CosmicSetup.cc
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <Analysis.hh>
#include <Testbench.hh>
Testbench::Testbench():SingleCrystal(),fLogicTrigger(0),fTriggerOffsetX(0),fTriggerOffsetY(0),fTriggerOffsetZ(0) {
	fCrystalLength=10*CLHEP::cm;
	fCrystalWidth=3*CLHEP::cm;
	fTriggerHeight=2.5*CLHEP::cm;
	fTriggerWidth=1.5*CLHEP::cm;
	fTriggerThickness=.5*CLHEP::cm;
	fTrigger=true;
	fScintillatorMaterialName="LYSO";
	fScintillatorMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);

	DefineCommands();
}

Testbench::~Testbench(){
}

G4VPhysicalVolume* Testbench::Construct() {
	if(fChangedParameters)
		ComputeParameters();

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	auto solidTarget=new G4Box("Target",.5*CLHEP::cm,.5*CLHEP::cm,1*CLHEP::mm);
	auto logicTarget=new G4LogicalVolume(solidTarget,G4NistManager::Instance()->FindOrBuildMaterial("G4_C"),"Target");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicTarget,"Target",fLogicWorld,0,0,0);

	auto solidDetector=new G4Tubs("Detector",3.5*CLHEP::mm,12.3*CLHEP::mm,.5*CLHEP::mm,0,360*CLHEP::deg);
	auto logicDetector=new G4LogicalVolume(solidDetector,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"Detector");
	new G4PVPlacement(0,G4ThreeVector(0,0,10*CLHEP::cm),logicDetector,"Detector",fLogicWorld,0,0,0);

	fSensitiveDetectors.Update("Detector",SDtype::kPerfect,{logicDetector});

	return fPhysiWorld;
}

void Testbench::DefineCommands() {

	SingleCrystal::DefineCommands();
	G4GenericMessenger::Command& triggerLengthCmd
	= fMessenger->DeclareMethodWithUnit("triggerlength","mm",
			&Testbench::setTriggerLength,
			"trigger length (mm)");


	G4GenericMessenger::Command& triggerThicknessCmd
	= fMessenger->DeclareMethodWithUnit("triggerthickness","mm",
			&Testbench::setTriggerThickness,
			"trigger thickness (mm)");

	G4GenericMessenger::Command& triggerWidthCmd
	= fMessenger->DeclareMethodWithUnit("triggerwidth","mm",
			&Testbench::setTriggerWidth,
			"trigger width (mm)");

	triggerWidthCmd.SetParameterName("width", true);
	triggerWidthCmd.SetRange("width>=0.");
	triggerWidthCmd.SetDefaultValue("30.");

	G4GenericMessenger::Command& trgOffsetXCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetX","mm",
			&Testbench::setTriggerOffsetX,
			"trigger offset in x dir. (mm)");
	G4GenericMessenger::Command& trgOffsetYCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetY","mm",
			&Testbench::setTriggerOffsetY,
			"trigger offset in y dir. (mm)");
	G4GenericMessenger::Command& trgOffsetZCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetZ","mm",
			&Testbench::setTriggerOffsetZ,
			"trigger offset in z dir. (mm)");


}
