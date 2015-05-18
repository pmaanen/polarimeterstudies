/*
 * CosmicSetup.cc
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <CosmicSetup.hh>
#include <Analysis.hh>
CosmicSetup::CosmicSetup():SingleCrystal(),logicTrigger(0) {
	crystalLength=10*CLHEP::cm;
	triggerLength=crystalLength;
	triggerWidth=crystalWidth;
	triggerThickness=1*CLHEP::cm;

	scintillatorMaterialName="G4_LYSO_SCINT";
	scintillatorMaterial=G4NistManager::Instance()->FindOrBuildMaterial(scintillatorMaterialName);

	DefineCommands();
}

CosmicSetup::~CosmicSetup(){
}

G4VPhysicalVolume* CosmicSetup::Construct() {
	SingleCrystal::Construct();

	G4Box* solidTrigger=new G4Box("Trigger",triggerWidth/2,triggerThickness/2,triggerLength/2);
	logicTrigger=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Trigger");
	new G4PVPlacement(0,G4ThreeVector(0,crystalWidth/2+triggerThickness/2,triggerLength/2),logicTrigger,"Trigger",logicWorld,false,0,false);
	new G4PVPlacement(0,G4ThreeVector(0,-crystalWidth/2-triggerThickness/2,triggerLength/2),logicTrigger,"Trigger",logicWorld,false,1,false);
	return physiWorld;


}

void CosmicSetup::DefineCommands() {

	JediPolarimeter::DefineCommands();
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

}

void CosmicSetup::ConstructSDandField() {

	if(CaloSD.Get()==0 and logicCaloCrystal){
			CaloSensitiveDetector* SD=new CaloSensitiveDetector("Calorimeter");
			CaloSD.Put(SD);
		}
	if(logicCaloCrystal){
		SetSensitiveDetector(logicCaloCrystal,CaloSD.Get());
	}
	if(logicTrigger and triggerSD.Get()==0){
		triggerSD.Put(new CaloSensitiveDetector("Trigger"));
	}
	if(logicTrigger)
		SetSensitiveDetector(logicTrigger,triggerSD.Get());
	return;
}
