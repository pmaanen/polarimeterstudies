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
#include <G4GenericMessenger.hh>
#include "G4Cache.hh"
#include <algorithm>
class TrackerSensitiveDetector;
class CaloSensitiveDetector;
/*
 * \brief Analysis class
 * This class contains the code to collect information from
 * the different UserActions.
 * The class is designed as a singleton.
 * To access it you need to use:
 * Analysis* analysis = Analysis::GetInstance()
 * */

class calorhit_t;
class trackerhit_t;
class Analysis {
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
	void setEnabled(bool xenable){fEnabled=xenable;};

	void enable(){fEnabled=true;};
	void disable(){fEnabled=false;};
	G4bool isEnabled() const {
		return fEnabled;
	}
	const G4String& getFileName() const {
		return fFileName;
	}

	void setFileName(const G4String& fileName) {
		fFileName = fileName;
	}

	TTree* GetTree();
	void BeginOfRun();
	void EndOfRun(const G4Run* run);

	void BeginOfEvent();
	void EndOfEvent(const G4Event* evt);

	void FillTree(){};
	void RegisterTrackerSD(TrackerSensitiveDetector*);
	void RegisterCaloSD(CaloSensitiveDetector*);

	const std::vector<G4String>& getCaloSdNames() const {
		return fCaloSDNames;
	}

	const std::vector<G4String>& getTrackerSdNames() const {
		return fTrackerSDNames;
	}

	const std::vector<event_t>& getEvents() const {
		return fEvents;
	}

private:
	//! Private constructor: part of singleton pattern
	Analysis(G4bool isMaster=true);
	//! Singleton static instance
	static Analysis* fgMasterInstance;
	static G4ThreadLocal Analysis* fgInstance;

	G4GenericMessenger* fAnalysisMessenger;

	bool fEnabled;

	G4String fFileName;
	TFile* fOutFile;
	TTree* fOutTree;
	std::map<G4String,TBranch*> fOutBranches;

	std::vector<CaloSensitiveDetector*> fCaloSD;
	std::vector<TrackerSensitiveDetector*> fTrackerSD;

	std::vector<G4String> fCaloSDNames;
	std::vector<G4String> fTrackerSDNames;

	std::map<G4String, std::vector<calorhit_t>* > fCaloHits;
	std::map<G4String, std::vector<trackerhit_t>* > fTrackerHits;

	std::vector<event_t> fEvents;


};
#endif /* ANALYSIS_HH_ */
