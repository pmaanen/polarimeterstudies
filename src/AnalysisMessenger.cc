/*
 * polarimeterStudiesAMessenger.cc
 *
 *  Created on: Jun 19, 2013
 *      Author: maanen
 */
#include "G4UIdirectory.hh"
#include "AnalysisMessenger.hh"
#include "Analysis.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcommand.hh"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "G4FileMessenger.hh"
AnalysisMessenger::AnalysisMessenger(Analysis *Ana):G4AnalysisMessenger(Ana),ana(Ana)
{
	enableCmd=new G4UIcommand("/analysis/Enable",this);
	enableCmd->SetGuidance("Enable Output");

	disableCmd=new G4UIcommand("/analysis/Disable",this);
	disableCmd->SetGuidance("Disable Output");

}

AnalysisMessenger::~AnalysisMessenger() {
	delete enableCmd;
	delete disableCmd;
}

void AnalysisMessenger::SetNewValue(G4UIcommand *cmd, G4String value)
{
	G4AnalysisMessenger::SetNewValue(cmd,value);
	if( cmd== enableCmd){
		ana->setEnabled(true);
	}
	if( cmd== disableCmd){
		ana->setEnabled(false);
	}
}


