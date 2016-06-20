/*
 * JediSensitiveDetector.cc

 *
 *  Created on: 19.05.2016
 *      Author: pmaanen
 */
#include <JediSensitiveDetector.hh>
#include "CaloSensitiveDetector.hh"
#include "TrackerSensitiveDetector.hh"
#include "PerfectDetector.hh"
#include "G4Threading.hh"
JediSensitiveDetector::JediSensitiveDetector(const G4String& name, const SDtype& type):fType(type),fName(name),G4VSensitiveDetector(name){

	if(type==SDtype::kCalorimeter)
		fSD=new CaloSensitiveDetector(name);
	else if(type==SDtype::kTracker)
		fSD=new TrackerSensitiveDetector(name);
	else if(type==SDtype::kPerfect)
		fSD=new PerfectDetector(name);
	else if(type==SDtype::kUndefined)
		fSD=nullptr;
	DefineCommands();
}

void JediSensitiveDetector::SetType_impl(SDtype type) {
	if(fType!=type){
		if(fSD)
			delete fSD;
		fSD=nullptr;
		if(type==SDtype::kCalorimeter){
			fSD=new CaloSensitiveDetector(fName);
		}
		else if(type==SDtype::kTracker){
			fSD=new TrackerSensitiveDetector(fName);
		}
		else if(type==SDtype::kPerfect){
			fSD=new PerfectDetector(fName);
		}
	}
}

void JediSensitiveDetector::SetType(G4String command) {
	if(command=="perfect")
		SetType_impl(SDtype::kPerfect);
	else if(command=="calo")
		SetType_impl(SDtype::kCalorimeter);
	else if(command=="tracker")
		SetType_impl(SDtype::kTracker);
	else{
		SetType_impl(SDtype::kUndefined);
		G4Exception("JediSensitiveDetector::SetType","",JustWarning,"SD type not recognized.");
	}
}

void JediSensitiveDetector::DefineCommands() {

	G4String dir;
	dir+="/PolarimeterStudies/";
	dir+=this->GetName();
	dir+="/";
	fMessenger=new G4GenericMessenger(this,dir,"");

	fMessenger->DeclareMethod("Print",&JediSensitiveDetector::Print,"");
	auto cmd=fMessenger->DeclareMethod("SetType",&JediSensitiveDetector::SetType,"");
	cmd.SetCandidates("perfect calo tracker");
}

void JediSensitiveDetector::Print() {}
