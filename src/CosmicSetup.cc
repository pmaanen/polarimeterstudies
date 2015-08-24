/*
 * CosmicSetup.cc
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#include <CosmicSetup.hh>
#include <Analysis.hh>
CosmicSetup::CosmicSetup():SingleCrystal(),logicTrigger(0),theta(0),phi(0),psi(0) {
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
	if(changedParameters)
		ComputeParameters();

	G4Box* solidWorld=new G4Box("World",worldSizeXY/2,worldSizeXY/2,worldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);

	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(phi,theta,psi);
	new G4PVPlacement (rot, G4ThreeVector(0,0,crystalLength/2), aCrystal, "Crystal", logicWorld, false, 0, false);
	/*
	G4Box* solidTrigger=new G4Box("Trigger",triggerWidth/2,triggerThickness/2,triggerLength/2);
	logicTrigger=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Trigger");
	new G4PVPlacement(0,G4ThreeVector(0,crystalWidth/2+triggerThickness/2,triggerLength/2),logicTrigger,"Trigger",logicWorld,false,0,false);
	new G4PVPlacement(0,G4ThreeVector(0,-crystalWidth/2-triggerThickness/2,triggerLength/2),logicTrigger,"Trigger",logicWorld,false,1,false);
	 */
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

	G4GenericMessenger::Command& thetaCmd
	= fMessenger->DeclareMethodWithUnit("theta","deg",
			&CosmicSetup::setTheta,
			"set theta");

	G4GenericMessenger::Command& phiCmd
	= fMessenger->DeclareMethodWithUnit("phi","deg",
			&CosmicSetup::setPhi,
			"set phi");

	G4GenericMessenger::Command& psiCmd
	= fMessenger->DeclareMethodWithUnit("psi","deg",
			&CosmicSetup::setPsi,
			"set psi");

}

void CosmicSetup::ConstructSDandField() {

	if (CaloSD["Calorimeter"].Get()==0 and caloSDVolumes["Calorimeter"]){
		CaloSD["Calorimeter"].Put(new CaloSensitiveDetector("Calorimeter"));
	}

	if (CaloSD["Trigger"].Get()==0 and caloSDVolumes["Trigger"]){
		CaloSD["Trigger"].Put(new CaloSensitiveDetector("Trigger"));
	}

	if(caloSDVolumes["Calorimeter"])
		SetSensitiveDetector(caloSDVolumes["Calorimeter"],CaloSD["Calorimeter"].Get());

	if(caloSDVolumes["Trigger"])
		SetSensitiveDetector(caloSDVolumes["Trigger"],CaloSD["Trigger"].Get());

	return;
}
