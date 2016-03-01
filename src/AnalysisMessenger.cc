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
AnalysisMessenger::AnalysisMessenger(Analysis *Ana):G4AnalysisMessenger(Ana),fAnalysis(Ana)
{
	fEnableCmd=new G4UIcommand("/analysis/Enable",this);
	fEnableCmd->SetGuidance("Enable Output");

	fDisableCmd=new G4UIcommand("/analysis/Disable",this);
	fDisableCmd->SetGuidance("Disable Output");

}

AnalysisMessenger::~AnalysisMessenger() {
	delete fEnableCmd;
	delete fDisableCmd;
}

void AnalysisMessenger::SetNewValue(G4UIcommand *cmd, G4String value)
{
	G4AnalysisMessenger::SetNewValue(cmd,value);
	if( cmd== fEnableCmd){
		fAnalysis->setEnabled(true);
	}
	if( cmd== fDisableCmd){
		fAnalysis->setEnabled(false);
	}
}
#endif
