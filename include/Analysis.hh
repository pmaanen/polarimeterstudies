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
#include "TFile.h"
#include <sstream>
#include "G4RootAnalysisManager.hh"
#include "G4Threading.hh"
#include <G4GenericMessenger.hh>
#include <JediClasses.hh>
#include <JediException.hh>
#include "G4Cache.hh"
#include <algorithm>
#include <memory>
#include "G4ThreadLocalSingleton.hh"
#include "JediUtilities.hh"

class TrackerSensitiveDetector;
class CaloSensitiveDetector;
class JediSensitiveDetector;
class GenEventProducer;
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
	virtual ~Analysis()=default;

	void Enable(bool xenable=true){fEnabled=xenable;};

	G4bool isEnabled() const {
		return fEnabled;
	}
	const G4String& getFileName() const {
		return fFileName;
	}

	void setFileName(G4String fileName) {
		fFileName = fileName;
	}

	void BeginOfRun();
	void EndOfRun(const G4Run* run);

	void BeginOfEvent(){};
	void EndOfEvent(const G4Event* evt);

	void RegisterMe(JediSensitiveDetector*);
	void UnRegisterMe(JediSensitiveDetector*);

	void RegisterMe(GenEventProducer*);
	void UnRegisterMe(GenEventProducer*);

	 const std::vector<simevent_t>* getSimEvents();
	 const std::vector<genevent_t>* getGenEvents();

private:
	Analysis();

	void enable(){Enable(true);}
	void disable(){Enable(false);}


	void FillSimTree(const G4Run* aRun);
	void FillGenTree(const G4Run* aRun);

	//Choose filename from several options
	//Precedence: 1) Messenger 2) Command line 3) Default (run_i.root)
	G4String Filename(const G4Run* run);


	std::unique_ptr<G4GenericMessenger> fAnalysisMessenger;

	bool fEnabled;

	G4String fFileName;
	static G4String fGeneratorName;

	std::vector<JediSensitiveDetector*> fSD;
	std::vector<GenEventProducer*> fGenerators;
	std::unique_ptr<std::vector<genevent_t>> fGenEvents;
	std::unique_ptr<std::vector<simevent_t>> fSimEvents;
	std::map<G4String, std::vector<calorhit_t>* > fCaloHits;
	std::map<G4String, std::vector<trackerhit_t>* > fTrackerHits;


};

inline const std::vector<simevent_t>* Analysis::getSimEvents() {
		return fSimEvents.get();
	}

inline	const std::vector<genevent_t>* Analysis::getGenEvents() {
		return fGenEvents.get();
}

#endif /* ANALYSIS_HH_ */
