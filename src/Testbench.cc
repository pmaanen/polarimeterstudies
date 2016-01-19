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
	fTriggerLength=fCrystalWidth;
	fTriggerWidth=fCrystalWidth;
	fTriggerThickness=1*CLHEP::cm;
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

	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,0), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	if(fTriggerThickness>0 and fTriggerLength>0 and fTriggerWidth>0){
		G4Box* solidTrigger=new G4Box("Trigger",fTriggerWidth/2,fTriggerThickness/2,fTriggerLength/2);
		fCaloSDVolumes["Trigger"]=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Trigger");
		if(fTrigger){
			G4int iTrig=0;
			auto rot=new G4RotationMatrix();
			rot->rotateX(90*CLHEP::deg);
			for(auto iX=-1;iX<2;iX++)
				for(auto iY=-1;iY<2;iY++){
					new G4PVPlacement(rot,G4ThreeVector(fCrystalWidth*iX,fCrystalWidth*iY,fTriggerOffsetZ+25.5*CLHEP::cm),fCaloSDVolumes["Trigger"],"Trigger",fLogicWorld,0,iTrig++,0);
				}
		}

	}
	return fPhysiWorld;
}

void Testbench::DefineCommands() {

	SingleCrystal::DefineCommands();
	G4cout<<"CosmicSetup::DefineCommands() called"<<G4endl;
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

void Testbench::ConstructSDandField() {

	if (fCaloSD["Calorimeter"].Get()==0 and fCaloSDVolumes["Calorimeter"]){
		fCaloSD["Calorimeter"].Put(new CaloSensitiveDetector("Calorimeter"));
	}

	if (fCaloSD["Trigger"].Get()==0 and fCaloSDVolumes["Trigger"]){
		fCaloSD["Trigger"].Put(new CaloSensitiveDetector("Trigger"));
	}

	if(fCaloSDVolumes["Calorimeter"])
		SetSensitiveDetector(fCaloSDVolumes["Calorimeter"],fCaloSD["Calorimeter"].Get());

	if(fCaloSDVolumes["Trigger"])
		SetSensitiveDetector(fCaloSDVolumes["Trigger"],fCaloSD["Trigger"].Get());

	return;
}
