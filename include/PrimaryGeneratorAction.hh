#ifndef SFEventGenerator_h
#define SFEventGenerator_h 1
#include <G4VUserPrimaryGeneratorAction.hh>
#include "globals.hh"
#include <iomanip> 
#include <iostream>
#include <fstream>
#include <vector>
class G4GenericMessenger;
class G4Event;
class G4ParticleGun;
class EventGenerator;
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {


public:
	enum GeneratorMode {GUN=1,INPUTFILE=2,GENERATE=3,DCELASTIC=4,DCBREAKUP=5,MUON=6};

	~PrimaryGeneratorAction();
	PrimaryGeneratorAction() ;
	void GeneratePrimaries(G4Event* E);
	void generateEventFromInput(G4Event* E);
	void generateEventFromGun(G4Event* E);
	void generateEventFromPhaseSpace(G4Event* E);
	void generateEventFromGenerator(G4Event* E);
	void illuminateAngle(G4Event* E);
	//book tuples and histograms in analysis
	virtual void beginOfRun();
	G4int getMode() const{return _mode;}
	void setMode(G4int mode);
	void setInfile(G4String);
	G4ParticleGun* getPGun() const{return _pGun;};
private:
	void DefineCommands();
	G4ParticleGun			*_pGun ;
	GeneratorMode			_mode;
	G4GenericMessenger*		fMessenger;
	G4String 				_infile;
	std::ifstream        _instream;
	EventGenerator*	evtGen;
	std::vector<G4int> myTupleId;
	G4double illuminationAngle;
protected:

} ;

#endif

// eof
