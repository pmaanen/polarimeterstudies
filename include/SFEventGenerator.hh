// File:	SFEventGenerator.hh
// Date:	18-Lan-2008 G.Macharashvili Dubna

#ifndef SFEventGenerator_h
#define SFEventGenerator_h 1

#include <iomanip> 
#include <G4RunManager.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4DecayTable.hh>
#include <G4PhaseSpaceDecayChannel.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryParticle.hh>
#include <G4DynamicParticle.hh>
#include <G4DecayProducts.hh>
#include <G4LorentzVector.hh>
#include <G4PrimaryVertex.hh>
#include <G4ThreeVector.hh>

#include <G4Proton.hh>
#include <G4Deuteron.hh>
#include <G4ChargedGeantino.hh>
#include <Randomize.hh>

#include <TH1.h>
#include <TH2.h>
#include "SFMessenger.hh"
class SFEventGenerator : public G4VUserPrimaryGeneratorAction {


public:
	enum GeneratorMode {GUN=1,INPUTFILE=2,GENERATE=3};

	~SFEventGenerator();
	SFEventGenerator() ;
	void GeneratePrimaries(G4Event* E);
	void generateEventFromInput(G4Event* E);
	void generateEventFromGun(G4Event* E);
	void generateEventFromPhaseSpace(G4Event* E);
	G4int getMode() const;
	void setMode(G4int mode);
	void setInfile(TString);
	G4ParticleGun* getPGun() const{return _pGun;};
private:
	G4ParticleGun			*_pGun ;
	GeneratorMode			_mode;
	SFMessenger*			_messenger;
	TString 				_infile;
	std::ifstream        _instream;

protected:

} ;

#endif

// eof
