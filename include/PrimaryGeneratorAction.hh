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
#include "FileReader.hh"
#include "hit.hh"
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
	~PrimaryGeneratorAction();
	PrimaryGeneratorAction() ;
	virtual void GeneratePrimaries(G4Event* E);
	void generateEventFromInput(G4Event* E);
	void generateEventFromGenerator(G4Event* E);
	void illuminateAngle(G4Event* E);
	void setInfile(G4String);
	G4ParticleGun* getPGun() const {
		return fParticleGun;
	}

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
	G4ParticleGun			*fParticleGun ;
	G4GenericMessenger*		fMessenger;
	G4String 				fInfileName;
	std::ifstream        fInstream;
	std::vector<G4int> fTupleId;
	G4double fIlluminationAngle;
	static FileReader* fgFileReader;
	std::map<G4String,EventGenerator*> fEvtGenerators;
	G4String fGeneratorName;
	genevent_t fGenEvent;
protected:
	void listModes(){G4cout<<"gun "; G4cout<<"file ";for (auto iGen:fEvtGenerators){G4cout<<iGen.first<<" ";}};

} ;

#endif

// eof
