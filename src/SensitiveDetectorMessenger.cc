/*
 * SensitiveDetectorMessenger.cc
 *
 *  Created on: Jul 3, 2013
 *      Author: maanen
 */

#include "SensitiveDetectorMessenger.hh"
#include "SensitiveDetector.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
SensitiveDetectorMessenger::SensitiveDetectorMessenger(SensitiveDetector* sensDet):mydet(sensDet) {
	sensDir=new G4UIdirectory("/polarimeterStudies/det/");
	sensDir->SetGuidance("sensitive detector control");

	sensCmd=new G4UIcmdWithADoubleAndUnit("/polarimeterStudies/det/SetResolution",this);
	sensCmd->SetGuidance("Set detector resolution");
	sensCmd->SetParameterName("reso",false);
	sensCmd->SetRange("reso>=0.");
	sensCmd->SetUnitCategory("Length");
	sensCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

void SensitiveDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
	if(command==sensCmd){
		mydet->SetResolution(sensCmd->GetNewDoubleValue(newValue));
	}
}

SensitiveDetectorMessenger::~SensitiveDetectorMessenger() {
	delete sensDir;
	delete sensCmd;
}
