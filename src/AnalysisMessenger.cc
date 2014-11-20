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
AnalysisMessenger::AnalysisMessenger(Analysis *Ana):ana(Ana)
{
	anaDir=new G4UIdirectory("/polarimeterStudies/Analysis/");
	anaDir->SetGuidance("analysis and output module control");

	dirCmd=new G4UIcmdWithAString("/polarimeterStudies/Analysis/SetPath",this);
	dirCmd->SetGuidance("Set output directory");
	dirCmd->SetParameterName("dir",false);

	enableCmd=new G4UIcommand("/polarimeterStudies/Analysis/Enable",this);
	enableCmd->SetGuidance("Enable Output");

	disableCmd=new G4UIcommand("/polarimeterStudies/Analysis/Disable",this);
	disableCmd->SetGuidance("Disable Output");

	fileCmd=new G4UIcmdWithAString("/polarimeterStudies/Analysis/SetFilename",this);
	fileCmd->SetGuidance("Set output file name");
	fileCmd->SetParameterName("file",false);

	auxCmd=new G4UIcmdWithAString("/polarimeterStudies/Analysis/AddAuxiliary",this);
	auxCmd->SetGuidance("Add auxiliary information.");
	auxCmd->SetParameterName("Info",false);
}

AnalysisMessenger::~AnalysisMessenger() {
	delete anaDir;
	delete dirCmd;
}

void AnalysisMessenger::SetNewValue(G4UIcommand *cmd, G4String value)
{
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
	if(cmd == auxCmd){
		std::string data(value.data());
		std::vector<std::string> parts;
		boost::split(parts, data, boost::is_any_of("/"));
		if(parts.size()!=2){
			std::stringstream o;
			o<<"AddAuxiliary expects two string separated by a '/'."<<G4endl;
			G4Exception("AnalysisMessenger::SetNewValue", "ArgumentError", FatalException,
					o.str().c_str());
		}
		ana->AddAuxiliary(G4String(parts[0]),G4String(parts[1]));
	}
}

