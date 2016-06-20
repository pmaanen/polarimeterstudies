/*
 * SensDet.cc
 *
 *  Created on: 29.04.2016
 *      Author: pmaanen
 */
#include "SensitiveDetectorConfiguration.hh"
#include "JediSensitiveDetector.hh"
void SensitiveDetectorMap::Update(G4String name, SDtype type, logVolVector vector) {

	if(fSDmap.count(name)==0){
		fSDmap[name]=SD();
		fSDmap[name].fType=type;
		fSDmap[name].fLogVol=vector;
	}
	else{
		fSDmap[name].fType=type;
		fSDmap[name].fLogVol=vector;
		//fSDmap[name].fSD.Get()->SetType(type);
	}
}

void SensitiveDetectorMap::Update(G4String name, SDtype type) {
	if(fSDmap.count(name)==0){
		G4Exception("SDHandler::Update","",JustWarning,"Could not find SD. Command ignored");
	}
	else{}
}
