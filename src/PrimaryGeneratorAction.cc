// File:	EventGenerator.cc
// Date:	18-Lan-2008 G.Macharashvili Dubna

#include "Analysis.hh"
#include "global.hh"
#include "DCElasticEventGenerator.hh"
#include "DCBreakupEventGenerator.hh"
#include "DCElasticTimeDependentGenerator.hh"
#include "CosmicMuonGenerator.hh"
//Geant headers
#include <G4ParticleTable.hh>
#include <G4IonTable.hh>
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4GenericMessenger.hh"
#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4DecayTable.hh>
#include <G4RunManager.hh>
#include <G4PhaseSpaceDecayChannel.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryParticle.hh>
#include <G4DynamicParticle.hh>
#include <G4DecayProducts.hh>
#include <G4LorentzVector.hh>
#include <G4PrimaryVertex.hh>
#include <G4ThreeVector.hh>
#include "G4ios.hh"

//Particle def's
#include <G4Geantino.hh>
#include <G4Proton.hh>
#include <G4Deuteron.hh>
#include <G4ChargedGeantino.hh>
#include <PrimaryGeneratorAction.hh>
#include <Randomize.hh>
//Root headers
#include "TNtuple.h"
#include "TGenPhaseSpace.h"
#include <TH1.h>
#include <TH2.h>

//System headers
#include <vector>
#include <utility>
using namespace CLHEP;
#include "G4AutoLock.hh"

namespace { G4Mutex PrimaryGeneratorMutex = G4MUTEX_INITIALIZER; }

FileReader* PrimaryGeneratorAction::fileReader = 0;
PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction(),_infile(""),_instream("",std::ifstream::in) {
	_mode=GUN;
	G4int Nparticle = 1 ;
	_pGun = new G4ParticleGun(Nparticle);
	illuminationAngle=-1;
	DefineCommands();
	evtGenerators["muon"]=new CosmicMuonGenerator(_pGun);
	evtGenerators["dcelastic"]=new DCElasticEventGenerator(_pGun);
	evtGenerators["dcbreakup"]=new DCBreakupEventGenerator(_pGun);
	evtGenerators["dcelastictime"]=new DCElasticTimeDependentGenerator(_pGun);
	evtGen=evtGenerators["muon"];

	_pGun->SetParticleEnergy(gConfig["generator.beam_energy"].as<double>()*CLHEP::MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete _pGun ;
}

void PrimaryGeneratorAction::generateEventFromPhaseSpace(G4Event *E)
{
	generatorName.toLower();
	if(evtGenerators.count(generatorName)==0){
		std::stringstream message;
		message<<"event generator "<<generatorName<<" is not known.";
		G4Exception("PrimaryGeneratorAction::generateEventFromPhaseSpace","NO_SUCH_GENERATOR",RunMustBeAborted,message.str().c_str());
		return;
	}
	else evtGenerators[generatorName]->Generate(E);
	return;
}

void PrimaryGeneratorAction::generateEventFromInput(G4Event *E)
{
	PrimaryEvent evt;
	if(fileReader)
	{
		G4AutoLock lock(&PrimaryGeneratorMutex);
		evt = fileReader->GetEvent();
	}

	for(auto ipart : evt){
		auto part=G4ParticleTable::GetParticleTable()->FindParticle(ipart.id);
		if(!part){
			G4int Z,A,lvl;
			Z=0;
			A=0;
			G4double E;
			auto ionFound=G4IonTable::GetIonTable()->GetNucleusByEncoding(ipart.id,Z,A,E,lvl);
			G4cout<<ionFound<<" "<<Z<<" "<<A<<" "<<G4endl;
			part=G4IonTable::GetIonTable()->GetIon(Z,A);
			if(!part){
				std::stringstream message;
				message<<"primary particle not found.particle id: "<<ipart.id;
				G4Exception("EventGenerator::generateEventFromInput()", "ParticleError", FatalException,
						message.str().c_str());
			}
		}
		_pGun->SetParticleDefinition(part);
		_pGun->SetParticleMomentum(G4ThreeVector(ipart.px,ipart.py,ipart.pz)) ;
		_pGun->GeneratePrimaryVertex(E);
	}

	return;

}

void PrimaryGeneratorAction::generateEventFromGun(G4Event *E)
{
	_pGun->GeneratePrimaryVertex(E) ;
}

void PrimaryGeneratorAction::setMode(G4int mode)
{
	auto oldmode=this->_mode;
	this->_mode = static_cast<GeneratorMode>(mode);
	if(!(_mode==GUN or _mode==INPUTFILE or _mode==GENERATE)){
		std::stringstream o;
		o<<"Mode not recognized. Mode: "<<_mode<<G4endl;
		G4Exception("EventGenerator::SetMode()", "ArgumentError", JustWarning,
				o.str().c_str());
		this->_mode=oldmode;
	}
	if(_mode==INPUTFILE){
		if(_infile==""){
			G4Exception("[EventGenerator]", "setMode", JustWarning,
					" ERROR: Set input file before switching mode. Command ignored");
			this->_mode=oldmode;
		}
		else{
			G4AutoLock lock(&PrimaryGeneratorMutex);
			if(!fileReader)
				fileReader = new FileReader(_infile);
		}
	}
}


void PrimaryGeneratorAction::setInfile(G4String string)
{
	_infile=string;
	//check if input file is open
	if(!_instream){
		//if not, try to open
		_instream.open(_infile.c_str());
		//if not opened, file is not found, throw
		if(!_instream){
			/*
			G4cerr<<"EventGenerator: Error. Input file not found "<<G4endl;
			G4Exception("[EventGenerator]", "setInfile", FatalException,
					" ERROR: Input file not found.");
			 */
		}
	}
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* E) {
	if(illuminationAngle>0){
		illuminateAngle(E);
		return;
	}
	switch(_mode){
	case GUN:
		generateEventFromGun(E);
		break;
	case INPUTFILE:
		generateEventFromInput(E);
		break;
	case GENERATE:
		generateEventFromPhaseSpace(E);
		break;
	default:
		std::stringstream o;
		o<<"Mode not recognized. Mode: "<<_mode<<G4endl;
		G4Exception("EventGenerator::SetMode()", "ModeError", FatalException,
				o.str().c_str());
	}
}

void PrimaryGeneratorAction::illuminateAngle(G4Event* E) {
	auto oldparticle=this->_pGun->GetParticleDefinition();
	auto oldmomentum=this->_pGun->GetParticleMomentumDirection();
	auto oldposition=this->_pGun->GetParticlePosition();

	G4ThreeVector direction(0,0,1);
	direction.setTheta(illuminationAngle);
	this->_pGun->SetParticlePosition(G4ThreeVector(0,0,0));
	this->_pGun->SetParticleDefinition(G4Geantino::GeantinoDefinition());
	for(int i=0;i<360;i++){
		direction.setPhi(i*CLHEP::deg);
		this->_pGun->SetParticleMomentumDirection(direction);
		this->_pGun->GeneratePrimaryVertex(E);
	}
	this->_pGun->SetParticleDefinition(oldparticle);
	this->_pGun->SetParticleMomentumDirection(oldmomentum);
	this->_pGun->SetParticlePosition(oldposition);
	illuminationAngle=-1;
}

void PrimaryGeneratorAction::DefineCommands()
{
	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/generator/",
			"Generator control");

	G4GenericMessenger::Command& modeCmd
	= fMessenger->DeclareMethod("Mode",
			&PrimaryGeneratorAction::setMode,
			"Set mode of generator.");
	modeCmd.SetParameterName("mode", true);
	//	modeCmd.SetRange("mode>=1. && mode<=2.");
	modeCmd.SetDefaultValue("1");

	G4GenericMessenger::Command& inputCmd
	= fMessenger->DeclareProperty("setFilename",_infile,"Set input file name");

	G4GenericMessenger::Command& generator
	= fMessenger->DeclareProperty("setGenerator",generatorName,"Set generator name");

	generator.SetGuidance("Possible values are: muon, dcelastic, dcbreakup, dcelastictime.");

	G4GenericMessenger::Command& illuminateCmd
	= fMessenger->DeclarePropertyWithUnit("illuminateAngle","deg",illuminationAngle,"illuminateAngle");

}
// eof

