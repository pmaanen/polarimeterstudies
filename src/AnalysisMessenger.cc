/*
 * polarimeterStudiesAMessenger.cc
 *
 *  Created on: Jun 19, 2013
 *      Author: maanen
 */
#if 0
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
	dirCmd=new G4UIcmdWithAString("/analysis/SetPath",this);
	dirCmd->SetGuidance("Set output directory");
	dirCmd->SetParameterName("dir",false);

	enableCmd=new G4UIcommand("/analysis/Enable",this);
	enableCmd->SetGuidance("Enable Output");

	disableCmd=new G4UIcommand("/analysis/Disable",this);
	disableCmd->SetGuidance("Disable Output");

/*
	fileCmd=new G4UIcmdWithAString("/analysis/SetFileName",this);
	fileCmd->SetGuidance("Set output file name");
	fileCmd->SetParameterName("file",false);

*/

}

AnalysisMessenger::~AnalysisMessenger() {
	delete dirCmd;
	delete enableCmd;
	delete disableCmd;
	delete fileCmd;
}

void AnalysisMessenger::SetNewValue(G4UIcommand *cmd, G4String value)
{
	G4AnalysisMessenger::SetNewValue(cmd,value);
	if( cmd== dirCmd){
		ana->setPath(value);
	}
	if( cmd== fileCmd){
		ana->setFilename(value);
	}
	if( cmd== enableCmd){
		ana->Enable(true);
	}
	if( cmd== disableCmd){
		ana->Enable(false);
	}
}
#endif
