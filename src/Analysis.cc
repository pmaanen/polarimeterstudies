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
#include "TrackerId.hh"
#include "DetId.hh"
#include "Math/Vector3D.h"
#include <algorithm>
#include "hit.hh"
//
Analysis* Analysis::_singleton = 0;
Analysis::Analysis():_enable(false),_path(""),_filename(""),_oldname(""),_outFile(0)
{
	_analysisMessenger=new AnalysisMessenger(this);
}

void Analysis::PrepareNewEvent(const G4Event* /*anEvent*/)
{
}
void Analysis::AddAuxiliary(G4String name, G4String value)
{
	this->BookObject<TNamed>(TString(name),TString(value));
}

void Analysis::PrepareNewRun(const G4Run* aRun=0)
{
	if(!_enable) return;
	std::stringstream fname;
	if(_path.length()>0 and _path.compare(_path.length()-1,1,"/")!=0){
		_path+="/";
	}
	fname<<_path;

	if(_filename.compare("")==0)
		fname<<"run_"<<aRun->GetRunID()<<".root";
	else{
		fname<<_filename;
		if(_filename.compare(_filename.length()-5,5,".root")!=0){
			fname<<".root";
		}
	}
	if(_oldname.compare(fname.str())==0 and aRun->GetRunID()>0){
		std::stringstream message;
		message<<"Filename has not changed from old run. Overwriting old file.";
		G4Exception("Analysis::PrepareNewRun()","Filename not changed",
				JustWarning,message.str().c_str());
	}
	_oldname=fname.str();
	_outFile=new TFile(fname.str().c_str(),"recreate");

	//Check of outputfile is writeable. Abort run when not. Do it here and in EndOfRun() to save time when runs are long.
	if(!_outFile->IsOpen()|| _outFile->IsZombie()){

			std::stringstream message;
			message<<"Output file "<<_oldname<<" not writeable or not open. Aborting....";
			G4Exception("Analysis::EndOfRun()","FileNotOpen",
					 RunMustBeAborted,message.str().c_str());
		}
}

void Analysis::EndOfEvent(const G4Event* anEvent)
{
	if(!_enable) return;
	return;
}

void Analysis::EndOfRun(const G4Run* aRun)
{
	if(!_enable) return;
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

	_outFile->Write();
	_outFile->Close();
	delete _outFile;
	_outFile=0;
}
