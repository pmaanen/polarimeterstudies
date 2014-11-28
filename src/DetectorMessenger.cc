/*
 * DetectorMessenger.cc
 *
 *  Created on: Jul 15, 2013
 *      Author: maanen
 */

#include "DetectorMessenger.hh"
#include "G4UIdirectory.hh"
#include "DetectorConstruction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* detc):dc(detc) {
	dcDir=new G4UIdirectory("/polarimeterStudies/DetectorConstruction/");
	dcDir->SetGuidance("Detector Construction Control");

	updateCmd = new G4UIcmdWithoutParameter("/polarimeterStudies/DetectorConstruction/update",this);
	updateCmd->SetGuidance("Update geometry.");
	updateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
	updateCmd->SetGuidance("if you changed geometrical value(s).");
	updateCmd->AvailableForStates(G4State_Idle);

	writeCmd = new G4UIcmdWithAString("/polarimeterStudies/DetectorConstruction/write",this);
	writeCmd->SetGuidance("Write geometry to gdml file.");
	writeCmd->SetParameterName("filename",false);
	writeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);



}
void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue){

	if (command == updateCmd){
		dc->UpdateGeometry();
	}
	else if (command == writeCmd){
		dc->WriteWorldToFile(newValue);
	}
}

DetectorMessenger::~DetectorMessenger() {
	delete dcDir;
	delete writeCmd;
}
