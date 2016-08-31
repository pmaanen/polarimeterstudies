/*
 * SensDet.cc
 *
 *  Created on: 29.04.2016
 *      Author: pmaanen
 */
#include "SensitiveDetectorConfiguration.hh"
#include "JediSensitiveDetector.hh"
#include "G4UImanager.hh"
#include <sstream>
void SensitiveDetectorMap::Update(G4String name, SDtype type, logVolVector vector) {

	if(fSDmap.count(name)==0){
		fSDmap[name]=SD();
		fSDmap[name].fType=type;
		fSDmap[name].fLogVol=vector;
	}
	else{
		fSDmap[name].fType=type;
		fSDmap[name].fLogVol=vector;
		std::stringstream cmd;
		cmd<<"/PolarimeterStudies/"<<name<<"SetType ";

		switch(SDtype)
		case SDtype::kCalorimeter:
			cmd<<"calo";
			break;
		case SDtype::kPerfect:
			cmd<<"perfect";
			break;
		case SDtype::kTracker:
			cmd<<"tracker";
			break;
		default:
			return;
		G4UImanager::GetUIpointer()->ApplyCommand(cmd.str().c_str());
	}
}

void SensitiveDetectorMap::Update(G4String name, SDtype type) {
	if(fSDmap.count(name)==0){
		G4Exception("SDHandler::Update","",JustWarning,"Could not find SD. Command ignored");
	}
	else{}
}
