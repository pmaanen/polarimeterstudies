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
#include "G4RootAnalysisManager.hh"
#include "G4Threading.hh"
#include <G4GenericMessenger.hh>
#include <JediException.hh>
#include "G4Cache.hh"
#include <algorithm>
#include "G4ThreadLocalSingleton.hh"
class TrackerSensitiveDetector;
class CaloSensitiveDetector;
class JediSensitiveDetector;
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
	friend class G4ThreadLocalSingleton<Analysis>;
	static Analysis* Instance() {
		static G4ThreadLocalSingleton<Analysis> theInstance;
		return theInstance.Instance();
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

	void BeginOfRun();
	void EndOfRun(const G4Run* run);

	void BeginOfEvent(){};
	void EndOfEvent(const G4Event* evt);

	void RegisterTrackerSD(TrackerSensitiveDetector*);
	void UnRegisterTrackerSD(TrackerSensitiveDetector*);

	void RegisterSD(JediSensitiveDetector*);
	void UnRegisterSD(JediSensitiveDetector*);


	void RegisterCaloSD(CaloSensitiveDetector*);
	void UnRegisterCaloSD(CaloSensitiveDetector*);
	const std::vector<simevent_t>* getSimEvents() const {
		return fSimEvents;
	}

	const std::vector<genevent_t>* getGenEvents() const {
		return fGenEvents;
	}

	const std::map<G4String, std::vector<trackerhit_t> *>& getTrackerHits() const {
		return fTrackerHits;
	}

private:
	//! Private constructor: part of singleton pattern
	Analysis();

	G4GenericMessenger* fAnalysisMessenger;

	bool fEnabled;

	G4String fFileName;
	static G4String fGeneratorName;

	std::vector<JediSensitiveDetector*> fSD;

	std::vector<CaloSensitiveDetector*> fCaloSD;
	std::vector<TrackerSensitiveDetector*> fTrackerSD;


	std::vector<genevent_t>* fGenEvents;
	std::vector<simevent_t>* fSimEvents;
	std::map<G4String, std::vector<calorhit_t>* > fCaloHits;
	std::map<G4String, std::vector<trackerhit_t>* > fTrackerHits;


};
#endif /* ANALYSIS_HH_ */
