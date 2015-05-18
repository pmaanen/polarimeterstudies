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
#include "G4Cache.hh"
#include "EventGenerator.hh"
#include "CLHEP/Units/SystemOfUnits.h"
class G4GenericMessenger;
class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
	class FileReader{
	public:
		FileReader(G4String fileName){
			iEvent=0;
			inputFile.open(fileName.data());
		};
		~FileReader(){inputFile.close();};

		PrimaryEvent GetEvent(){
			if( evCache.size() == 0 )
			{
				PrimaryEvent thisEvent;
				PrimaryParticle thisParticle(0,0,0,0);
				std::string line;
				for(auto iev=0;iev<100;++iev){
					while(std::getline(inputFile,line)){
						G4double px,py,pz;
						G4int ev,id;
						std::istringstream iss(line);
						iss>>ev>>id>>px>>py>>pz;
						px*=CLHEP::GeV,py*=CLHEP::GeV,pz*=CLHEP::GeV;
						thisParticle=PrimaryParticle(id,px,py,pz);
						if(ev!=iEvent){
							evCache.push_back(thisEvent);
							thisEvent.clear();
							thisEvent.push_back(thisParticle);
							iEvent=ev;
							break;
						}
						else{
							thisEvent.push_back(thisParticle);
						}
					}
				}
			}
			PrimaryEvent ev = evCache.front();
			evCache.pop_front();
			return ev;
		}
	private:
		G4int iEvent;
		std::ifstream inputFile;
		std::list<PrimaryEvent> evCache;
	};
public:
	enum GeneratorMode {GUN=1,INPUTFILE=2,GENERATE=3,DCELASTIC=4,DCBREAKUP=5,MUON=6};

	~PrimaryGeneratorAction();
	PrimaryGeneratorAction() ;
	virtual void GeneratePrimaries(G4Event* E);
	void generateEventFromInput(G4Event* E);
	void generateEventFromGun(G4Event* E);
	void generateEventFromPhaseSpace(G4Event* E);
	void illuminateAngle(G4Event* E);
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
	EventGenerator* evtGen;
	std::vector<G4int> myTupleId;
	G4double illuminationAngle;
	static FileReader* fileReader;

protected:

} ;

#endif

// eof
