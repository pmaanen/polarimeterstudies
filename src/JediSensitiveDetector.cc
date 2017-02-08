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
#include <memory>
#include "global.hh"
JediSensitiveDetector::JediSensitiveDetector(const G4String& name, const SDtype& type):G4VSensitiveDetector(name),fType(type),fName(name){
	if(gVerbose>2)
		G4cout<<"JediSensitiveDetector::JediSensitiveDetector("<<name<<","<<int(type)<<")"<<G4endl;
	if(type==SDtype::kCalorimeter)
		fSD=std::unique_ptr<CaloSensitiveDetector>(new CaloSensitiveDetector(name));
	else if(type==SDtype::kTracker)
		fSD=std::unique_ptr<TrackerSensitiveDetector>(new TrackerSensitiveDetector(name));
	else if(type==SDtype::kPerfect)
		fSD=std::unique_ptr<PerfectDetector>(new PerfectDetector(name));
	else if(type==SDtype::kUndefined)
		fSD=nullptr;
	Analysis::Instance()->RegisterSD(this);
	DefineCommands();
}


void JediSensitiveDetector::SetType_impl(SDtype type) {
	if(fType!=type){
		if(type==SDtype::kCalorimeter){
			fSD.reset(new CaloSensitiveDetector(fName));
		}
		else if(type==SDtype::kTracker){
			fSD.reset(new TrackerSensitiveDetector(fName));
		}
		else if(type==SDtype::kPerfect){
			fSD.reset(new PerfectDetector(fName));
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
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,dir,""));

	fMessenger->DeclareMethod("Print",&JediSensitiveDetector::Print,"");
	auto cmd=fMessenger->DeclareMethod("SetType",&JediSensitiveDetector::SetType,"");
	cmd.SetCandidates("perfect calo tracker");
}
