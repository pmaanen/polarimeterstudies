#ifndef SFEventGenerator_h
#define SFEventGenerator_h 1
#include <G4VUserPrimaryGeneratorAction.hh>
#include "globals.hh"
#include <iomanip> 
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <list>

#include "TFile.h"
#include "TTree.h"
#include "G4Cache.hh"
#include "EventGenerator.hh"
#include "CLHEP/Units/SystemOfUnits.h"
class G4GenericMessenger;
class G4Event;
class G4ParticleGun;
class G4GeneralParticleSource;
#include "FileReader.hh"
#include "hit.hh"
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
	~PrimaryGeneratorAction();
	PrimaryGeneratorAction() ;
	virtual void GeneratePrimaries(G4Event* E);
	void generateEventFromInput(G4Event* E);
	void generateEventFromGenerator(G4Event* E);
	void generateEventFromGun(G4Event* E);
	void generateEventFromGPS(G4Event* E);
	void illuminateAngle(G4Event* E);
	void setInfile(G4String);

	const genevent_t& getGenEvent() const {
		return fGenEvent;
	}

	const G4String& getGeneratorName() const {
		return fGeneratorName;
	}

	;

	void Print(){G4cout<<this<<G4endl;}
private:
	void DefineCommands();
	std::unique_ptr<G4ParticleGun> fParticleGun ;
	std::unique_ptr<G4GeneralParticleSource> fGeneralParticleSource ;
	std::unique_ptr<G4GenericMessenger> fMessenger;
	G4String 				fInfileName;
	std::ifstream        fInstream;
	std::vector<G4int> fTupleId;
	G4double fIlluminationAngle;
	static FileReader* fgFileReader;
	std::map<G4String,std::unique_ptr<EventGenerator>> fEvtGenerators;
	G4String fGeneratorName;
	genevent_t fGenEvent;
protected:
	void listModes(){G4cout<<"gun "; G4cout<<"file ";for (const auto& iGen:fEvtGenerators){G4cout<<iGen.first<<" ";}};

} ;

#endif

// eof
