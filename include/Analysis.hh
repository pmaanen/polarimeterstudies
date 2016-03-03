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
	void setEnabled(bool xenable){fEnabled=xenable;};
	G4bool isEnabled() const {return fEnabled;};

private:
	//! Private constructor: part of singleton pattern
	Analysis(G4bool isMaster=true);
	//! Singleton static instance
	static Analysis* fgMasterInstance;
	static G4ThreadLocal Analysis* fgInstance;
	bool fEnabled;
	AnalysisMessenger* fAnalysisMessenger;
};


#endif /* ANALYSIS_HH_ */
