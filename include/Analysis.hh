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
	void EndOfRun();

	void BeginOfEvent();
	void EndOfEvent();

	void FillTree(){};
	void RegisterTrackerSD(TrackerSensitiveDetector*);
	void RegisterCaloSD(CaloSensitiveDetector*);

private:
	//! Private constructor: part of singleton pattern
	Analysis(G4bool isMaster=true);
	//! Singleton static instance
	static Analysis* fgMasterInstance;
	static G4ThreadLocal Analysis* fgInstance;

	G4GenericMessenger* fAnalysisMessenger;

	bool fEnabled;
	bool fTreeFilled;
	G4int fMyWorkerId;
	G4String fFileName;
	TFile* fOutFile;
	TTree* fOutTree;

	std::vector<std::pair<G4String,G4String> > fTreeDescriptions;
	std::vector<CaloSensitiveDetector*> fCaloSD;
	std::vector<TrackerSensitiveDetector*> fTrackerSD;


};
#endif /* ANALYSIS_HH_ */
