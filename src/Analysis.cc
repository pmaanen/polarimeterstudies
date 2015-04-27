#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SDManager.hh"
#include "DetectorHit.hh"
#include "DetectorConstruction.hh"
#include "AnalysisMessenger.hh"
//ROOT Stuff
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "G4RootFileManager.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4THitsMap.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include "DetId.hh"
#include "Math/Vector3D.h"
#include "tools/wroot/file"
#include <algorithm>
#include "hit.hh"
//


Analysis* Analysis::fgMasterInstance = 0;
G4ThreadLocal Analysis* Analysis::fgInstance = 0;

Analysis::Analysis(G4bool isMaster):G4RootAnalysisManager(isMaster),_enable(false),_path(""),_basename(""),_filename(""),_oldname("")
{
	this->SetHistoDirectoryName("histos");
	if ( ( isMaster && fgMasterInstance ) || ( fgInstance ) ) {
		G4ExceptionDescription description;
		description
		<< "      "
		<< "Analysis already exists."
		<< "Cannot create another instance.";
		G4Exception("Analysis::Analysis()",
				"Analysis_F001", FatalException, description);
	}
	if ( isMaster ) fgMasterInstance = this;
	fgInstance = this;
	_analysisMessenger=new AnalysisMessenger(this);
}

void Analysis::PrepareNewEvent(const G4Event*){}

void Analysis::AddAuxiliary(G4String name, G4String value)
{
	if(!_enable) return;
	this->BookObject<TNamed>(TString(name),TString(value));
	return;
}

void Analysis::PrepareNewRun(const G4Run* aRun=0)
{
	if(!_enable) return;
	G4cout<<"Analysis::PrepareNewRun"<<G4endl;
	std::stringstream fname;

	//If path does not end with "/", add one
	if(_path.length()>0 and _path.compare(_path.length()-1,1,"/")!=0){
		_path+="/";
	}


	//Check if basename is set by user, if not add standard name
	if(_basename.compare("")==0){
		fname<<"run_"<<aRun->GetRunID();
	}
	else{
		fname<<_basename;
	}
	//Add suffix if not present (note: if no basename set, we will always need to add it)
	if(_basename.length()>4 and _basename.compare(_basename.length()-5,5,".root")!=0){
				fname<<".root";
			}



	if(_oldname.compare(fname.str())==0){
		std::stringstream message;
		message<<"Filename has not changed from old run. Overwriting old file.";
		G4Exception("Analysis::PrepareNewRun()","Filename not changed",
				JustWarning,message.str().c_str());
	}
	_oldname=fname.str();
	this->_filename=fname.str();
	G4cout<<"Analysis::PrepareNewRun() finished"<<G4endl;
}

void Analysis::EndOfEvent(const G4Event* /*anEvent*/)
{
	if(!_enable) return;
	G4cout<<"Analysis::EndOfEvent"<<G4endl;
	return;
}

void Analysis::EndOfRun(const G4Run* /*aRun*/)
{
	if(!_enable) return;
	G4cout<<"Analysis::EndOfRun"<<G4endl;
	return;
	/*
	G4int numEvents = aRun->GetNumberOfEvent();

	G4cout<<"================="<<G4endl;
	G4cout<<"Summary for run: "<<aRun->GetRunID()<<G4endl;
	G4cout<<"\t Event processed: "<<numEvents<<G4endl;

	if(!_outFile->IsOpen()|| _outFile->IsZombie()){
		std::stringstream message;
		message<<"Output file "<<_path<<_oldname<<" not writeable or not open. Aborting....";
		G4Exception("Analysis::EndOfRun()","FileNotOpen",
				RunMustBeAborted,message.str().c_str());
		return;
	}
	 */
	/*
	//Write hists and trees.
	//This loops over all objects in the object store and if they inherit from TH1 or TTree they are written to file and reset.
	//If inherits from TObject, we can not ensure it has a Reset(). So we only call Write().
	//This could be implemented smarter, may be with an array of function pointers to execute at EndofEvent, EndofRun, etc.
	std::map<G4String, TObject*>::const_iterator iObj;
	for(iObj=_objects.begin();iObj!=_objects.end();iObj++){
		TH1* histo = dynamic_cast< TH1* > ( iObj->second );
		if(histo){
			histo->Write();
			histo->Reset();
			continue;
		}
		TTree* tree=dynamic_cast< TTree* > ( iObj->second );
		if(tree){
			tree->Write();
			tree->Reset();
			continue;
		}
		TObject* object=dynamic_cast<TObject*> (iObj->second);
		if(object){
			object->Write();
		}
	}
	this->Write();
	this->CloseFile();
	 */
}

G4bool Analysis::OpenFile(const G4String& fileName) {
	if(!_enable)
		return false;
	else
		return G4RootAnalysisManager::OpenFile(fileName);
}

G4bool Analysis::Write() {
	if(!_enable)
			return false;
		else
			return G4RootAnalysisManager::Write();
}

G4bool Analysis::CloseFile() {
	if(!_enable)
			return false;
		else
			return G4RootAnalysisManager::CloseFile();
}
