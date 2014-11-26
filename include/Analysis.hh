#ifndef ANALYSIS_HH_
#define ANALYSIS_HH_

#include "G4Event.hh"
#include "G4Run.hh"
#include "G4ThreeVector.hh"
//ROOT Histograms
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include <map>
#include <fstream>
#include "hit.hh"
#include "TFile.h"
#include <sstream>
#include "G4Exception.hh"
#include "G4RootAnalysisManager.hh"
#include "G4Threading.hh"
class DetectorConstruction;
class AnalysisMessenger;
/*!
 * \brief Analysis class
 * This class contains the code to collect information from
 * the different UserActions.
 * The class is designed as a singleton.
 * To access it you need to use:
 * Analysis* analysis = Analysis::GetInstance()
 */
class Analysis: public G4RootAnalysisManager {
public:
	//! Singleton
	static Analysis* Instance() {
		{
			if ( fgInstance == 0 ) {
				G4bool isMaster = ! G4Threading::IsWorkerThread();
				fgInstance = new Analysis(isMaster);
			}

			return fgInstance;
		}
	}
	virtual ~Analysis() {};
	//! Should be called at the beginning of an event, hooks into UserEventAction
	void PrepareNewEvent(const G4Event* anEvent);
	//! Should be called at the end of an event, hooks into UserEventAction
	void EndOfEvent(const G4Event* anEvent);
	//! Should be called at the beginning of a run, hooks into UserRunAction
	void PrepareNewRun(const G4Run* aRun);
	//! Should be called at the end of a run, hooks into UserRunAction
	//Automatically resets trees and histos at end of run.
	void EndOfRun(const G4Run* aRun);
	//!Enables or disables analysis module. If disabled, all functions return after calling. Used by messenger.
	void Enable(bool xenable){_enable=xenable;};

	void setFilename(G4String xfilename){this->_basename= xfilename;}
	void setPath(G4String xpath){this->_path = xpath;}

	void AddAuxiliary(G4String name, G4String value);

	const std::string& getFilename() const {
		return _filename;
	}

	//Create a root object and return its adress.
	//Stored objects must inherit from TObject to ensure it has a GetName() and Write() method, throws otherwise
	template<typename T, typename ... Args>
	T* BookObject(const Args& ... args){
		std::stringstream o;
		o<<"Should not be used "<<G4endl;
		G4Exception("Analysis::BookObject()", "TypeError", FatalException,
				o.str().c_str());
		T* t=new T(args ...);
		TObject* retval=dynamic_cast<TObject*>(t);
		if(!retval){
			std::stringstream o;
			o<<"Object "<<" not typed correctly "<<G4endl;
			G4Exception("Analysis::BookObject()", "TypeError", FatalException,
					o.str().c_str());
		}
		_objects[t->GetName()]= t;
		return (T*)_objects[t->GetName()];
	}


	//Get an object from storage.
	//Throws if object is not typed correctly or does not exist.
	template<typename T>
	T* GetObject(G4String name){
		std::stringstream o;
		o<<"Should not be used "<<G4endl;
		G4Exception("Analysis::GetObject()", "TypeError", FatalException,
				o.str().c_str());

		if(!_objects[name]){
			std::stringstream o;
			o<<"Object "<<name<<" not found "<<G4endl;
			throw myG4Exception("Analysis::GetObject()", "ObjectNotFound", FatalException,
					o.str().c_str());
		}
		T* retval=dynamic_cast< T* >(_objects[name]);
		if(!retval){
			std::stringstream o;
			o<<"Object "<<name<<" not typed correctly "<<G4endl;
			G4Exception("Analysis::GetObject()", "TypeError", FatalException,
					o.str().c_str());
		}
		return retval;
	}

private:
	//! Private constructor: part of singleton pattern
	Analysis(G4bool isMaster=true);
	//! Singleton static instance
	static Analysis* fgMasterInstance;
	static G4ThreadLocal Analysis* fgInstance;
	bool _enable;
	std::string _path,_basename,_filename,_oldname;
	AnalysisMessenger* _analysisMessenger;
	TFile* _outFile;
	std::map<G4String,TObject*> _objects;
};


#endif /* ANALYSIS_HH_ */
