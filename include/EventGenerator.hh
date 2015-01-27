#ifndef SFEventGenerator_h
#define SFEventGenerator_h 1
#include <G4VUserPrimaryGeneratorAction.hh>
#include "globals.hh"
#include <iomanip> 
#include <iostream>
#include <fstream>
class G4GenericMessenger;
class G4Event;
class G4ParticleGun;
class DCElasticEventGenerator;
class EventGenerator : public G4VUserPrimaryGeneratorAction {


public:
	enum GeneratorMode {GUN=1,INPUTFILE=2,GENERATE=3,DC=4};

	~EventGenerator();
	EventGenerator() ;
	void GeneratePrimaries(G4Event* E);
	void generateEventFromInput(G4Event* E);
	void generateEventFromGun(G4Event* E);
	void generateEventFromPhaseSpace(G4Event* E);
	void generateDCElasticEvent(G4Event* E);
	G4int getMode() const;
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
	DCElasticEventGenerator* dc;

protected:

} ;

#endif

// eof
