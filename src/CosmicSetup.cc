/*
 * CosmicSetup.cc
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <CosmicSetup.hh>
#include <Analysis.hh>
CosmicSetup::CosmicSetup():SingleCrystal(),fLogicTrigger(0),fTriggerOffsetX(0),fTriggerOffsetY(0),fTriggerOffsetZ(0) {
	fCrystalLength=10*CLHEP::cm;
	fTriggerLength=fCrystalLength;
	fTriggerWidth=fCrystalWidth;
	fTriggerThickness=1*CLHEP::cm;
	fUpperTrigger=true;
	fLowerTrigger=false;
	fScintillatorMaterialName="G4_LYSO_SCINT";
	fScintillatorMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);

	DefineCommands();
}

CosmicSetup::~CosmicSetup(){
}

G4VPhysicalVolume* CosmicSetup::Construct() {
	if(fChangedParameters)
		ComputeParameters();

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,fCrystalLength/2), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	if(fTriggerThickness>0 and fTriggerLength>0 and fTriggerWidth>0){
		G4Box* solidTrigger=new G4Box("Trigger",fTriggerWidth/2,fTriggerThickness/2,fTriggerLength/2);
		fCaloSDVolumes["Trigger"]=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Trigger");
		if(fUpperTrigger)
		new G4PVPlacement(0,G4ThreeVector(fTriggerOffsetX,fCrystalWidth/2+fTriggerThickness/2+fTriggerOffsetY,fTriggerLength/2+fTriggerOffsetZ),fCaloSDVolumes["Trigger"],"Trigger",fLogicWorld,false,0,false);
		if(fLowerTrigger)
		new G4PVPlacement(0,G4ThreeVector(fTriggerOffsetX,-fCrystalWidth/2-fTriggerThickness/2-fTriggerOffsetY,fTriggerLength/2+fTriggerOffsetZ),fCaloSDVolumes["Trigger"],"Trigger",fLogicWorld,false,1,false);
	}
	return fPhysiWorld;
}

void CosmicSetup::DefineCommands() {

	SingleCrystal::DefineCommands();
	G4cout<<"CosmicSetup::DefineCommands() called"<<G4endl;
	G4GenericMessenger::Command& triggerLengthCmd
	= fMessenger->DeclareMethodWithUnit("triggerlength","mm",
			&CosmicSetup::setTriggerLength,
			"trigger length (mm)");


	G4GenericMessenger::Command& triggerThicknessCmd
	= fMessenger->DeclareMethodWithUnit("triggerthickness","mm",
			&CosmicSetup::setTriggerThickness,
			"trigger thickness (mm)");

	G4GenericMessenger::Command& triggerWidthCmd
	= fMessenger->DeclareMethodWithUnit("triggerwidth","mm",
			&CosmicSetup::setTriggerWidth,
			"trigger width (mm)");

	triggerWidthCmd.SetParameterName("width", true);
	triggerWidthCmd.SetRange("width>=0.");
	triggerWidthCmd.SetDefaultValue("30.");

	G4GenericMessenger::Command& trgOffsetXCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetX","mm",
			&CosmicSetup::setTriggerOffsetX,
			"trigger offset in x dir. (mm)");
	G4GenericMessenger::Command& trgOffsetYCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetY","mm",
			&CosmicSetup::setTriggerOffsetY,
			"trigger offset in y dir. (mm)");
	G4GenericMessenger::Command& trgOffsetZCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetZ","mm",
			&CosmicSetup::setTriggerOffsetZ,
			"trigger offset in z dir. (mm)");


}

void CosmicSetup::ConstructSDandField() {

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
