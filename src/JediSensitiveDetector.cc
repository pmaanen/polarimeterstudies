/*
 * JediSensitiveDetector.cc

 *
 *  Created on: 19.05.2016
 *      Author: pmaanen
 */
#include <JediConfigurationManager.hh>
#include <JediSensitiveDetector.hh>
#include "CaloSensitiveDetector.hh"
#include "TrackerSensitiveDetector.hh"
#include "PerfectDetector.hh"
#include "G4Threading.hh"
#include <memory>
JediSensitiveDetector::JediSensitiveDetector(const G4String& name)
:G4VSensitiveDetector(G4String("Jedi"+name)),
 fType(SDtype::kUndefined),
 fName(name)
{
	Analysis::Instance()->RegisterMe(this);
	DefineCommands();
}

JediSensitiveDetector::JediSensitiveDetector(const G4String& name, const SDtype& type)
:G4VSensitiveDetector(name),
 fType(type),
 fName(name)
{
	G4cout<<"JediSensitiveDetector::JediSensitiveDetector("<<name<<","<<int(type)<<")"<<G4endl;
	if(type==SDtype::kCalorimeter)
		fSD=std::unique_ptr<CaloSensitiveDetector>(new CaloSensitiveDetector(name));
	else if(type==SDtype::kTracker)
		fSD=std::unique_ptr<TrackerSensitiveDetector>(new TrackerSensitiveDetector(name));
	else if(type==SDtype::kPerfect)
		fSD=std::unique_ptr<PerfectDetector>(new PerfectDetector(name));
	else if(type==SDtype::kUndefined)
		fSD=nullptr;
	Analysis::Instance()->RegisterMe(this);
	DefineCommands();
}

JediSensitiveDetector::~JediSensitiveDetector() {
	Analysis::Instance()->UnRegisterMe(this);
}

SDtype JediSensitiveDetector::GetType() const
{
	return fType;
}

void JediSensitiveDetector::SetType_impl(SDtype type)
{
	if(fType != type) {
		if(type == SDtype::kCalorimeter) {
			fSD.reset(new CaloSensitiveDetector(fName));
		} else if(type == SDtype::kTracker) {
			fSD.reset(new TrackerSensitiveDetector(fName));
		} else if(type == SDtype::kPerfect) {
			fSD.reset(new PerfectDetector(fName));
		}
	}
}

void JediSensitiveDetector::SetType(G4String command)
{
	if(command == "perfect")
		SetType_impl(SDtype::kPerfect);
	else if(command == "calo")
		SetType_impl(SDtype::kCalorimeter);
	else if(command == "tracker")
		SetType_impl(SDtype::kTracker);
	else {
		SetType_impl(SDtype::kUndefined);
		G4Exception("JediSensitiveDetector::SetType", "", JustWarning, "SD type not recognized.");
	}
}

void JediSensitiveDetector::WriteHitsToFile(TTree* aTree, const G4Run* aRun) const
{
	if(fSD)
		fSD->WriteHitsToFile(aTree, aRun);
};

void JediSensitiveDetector::clear() {
}

void JediSensitiveDetector::DefineCommands()
{

	G4String dir;
	dir += "/PolarimeterStudies/";
	dir += this->GetName();
	dir += "/";
	fMessenger = std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this, dir, ""));

	fMessenger->DeclareMethod("Print", &JediSensitiveDetector::Print, "");
	auto cmd = fMessenger->DeclareMethod("SetType", &JediSensitiveDetector::SetType, "");
	cmd.SetCandidates("perfect calo tracker");
}

G4bool JediSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
	if(JediConfigurationManager::Instance()->GetVerbose()>4)
		G4cout<<GetName()<<": "<<"JediSensitiveDetector::ProcessHits"<<G4endl;
	if(fSD)
		return fSD->Hit(step);
	return false;
}

void JediSensitiveDetector::EndOfEvent(G4HCofThisEvent* hitCollection)
{
	if(fSD)
		fSD->EndOfEvent(hitCollection);
}

void JediSensitiveDetector::CopyHitsToRun(simevent_t* anEvent)
{
	if(fSD)
		fSD->CopyHitsToRun(anEvent);
}

void JediSensitiveDetector::Initialize(G4HCofThisEvent* hc)
{
	if(fSD)
		fSD->Initialize(hc);
}

void JediSensitiveDetector::DrawAll()
{
	if(fSD)
		fSD->DrawAll();
}

void JediSensitiveDetector::PrintAll()
{
	if(fSD)
		fSD->PrintAll();
}
void JediSensitiveDetector::AddSD(JediVSensitiveDetector* sd)
{
	fSD.reset(sd);
	fType=sd->GetType();
}
