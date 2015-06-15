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

PrimaryGeneratorAction::FileReader* PrimaryGeneratorAction::fileReader = 0;
PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction(),_infile(""),_instream("",std::ifstream::in) {
	_mode=GUN;
	G4int Nparticle = 1 ;
	_pGun = new G4ParticleGun(Nparticle);
	illuminationAngle=-1;
	DefineCommands();
	evtGen=new CosmicMuonGenerator(_pGun);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete _pGun ;
}

void PrimaryGeneratorAction::generateEventFromPhaseSpace(G4Event *E)
{

	TLorentzVector target;
	//TODO: Determine by config file
	// Declare the supported options.
	G4int PDGbeam,PDGtarget;
	G4double T_beam;
	try{
		if(vm.count("generator.beam_particle"))
			PDGbeam=vm["generator.beam_particle"].as<int>();
		else
			throw std::invalid_argument("beam particle not set.");
		if(vm.count("generator.target_particle"))
			PDGtarget=vm["generator.target_particle"].as<int>();
		else
			throw std::invalid_argument("target particle not set.");
		if(vm.count("generator.beam_energy"))
			T_beam = vm["generator.beam_energy"].as<double>()*MeV;
		else
			throw std::invalid_argument("beam energy not set.");

	}
	catch(const std::exception & exc){
		std::stringstream o;
		o<<"Parsing of config file failed with "<<exc.what()<<G4endl;
		G4Exception("EventGenerator::generateEventFromPhaseSpace", "ConfigurationError", FatalException,
				o.str().c_str());

	}
	G4ParticleDefinition * beam_particle=G4ParticleTable::GetParticleTable()->FindParticle(PDGbeam);
	G4ParticleDefinition * target_particle=G4ParticleTable::GetParticleTable()->FindParticle(PDGtarget);
	Double_t m_beam = beam_particle->GetPDGMass()/GeV;
	Double_t m_target = target_particle->GetPDGMass()/GeV;
	target.SetPxPyPzE(0.0, 0.0, 0.0, m_target);

	TLorentzVector beam;
	Double_t masses[2] = {m_beam, m_target} ;
	TGenPhaseSpace event;
	Analysis* an=Analysis::Instance();
	while (1) {

		beam.SetPxPyPzE(0, 0, sqrt(T_beam*(T_beam+2*m_beam)), T_beam+m_beam);

		TLorentzVector W = beam + target;
		event.SetDecay(W, 2, masses);
		//Sample an event assuming constant cross-section in cm-system
		event.Generate();

		//L-vector of recoil particle in lab-frame
		TLorentzVector precoil_4 = *event.GetDecay(1) ;

		//L-vector of scattered particle in lab-frame
		TLorentzVector pscattered_4 = *event.GetDecay(0) ;

		//spatial parts of generated L-vectors
		G4ThreeVector precoil_3(precoil_4.Vect().X()*GeV,precoil_4.Vect().Y()*GeV,precoil_4.Vect().Z()*GeV);
		G4ThreeVector pscattered_3(pscattered_4.Vect().X()*GeV,pscattered_4.Vect().Y()*GeV,pscattered_4.Vect().Z()*GeV);

		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = pscattered_3.getTheta()-beam.Theta();
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();
		//G4double phi_scattered = pscattered_3.getPhi();
		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(true){ // continue;

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = W.BoostVector();     // in case beam simulation
			pscattered_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for deuteron in cm-frame
			//Double_t CM_theta_scattered = pscattered_4.Theta();


			//TODO: Implement Hit or Miss
			//
			if(false) continue;
			else {
				this->_pGun->SetParticleDefinition(beam_particle);
				this->_pGun->SetParticleMomentum(pscattered_3);
				this->_pGun->GeneratePrimaryVertex(E);
				this->_pGun->SetParticleDefinition(target_particle);
				this->_pGun->SetParticleMomentum(precoil_3);
				this->_pGun->GeneratePrimaryVertex(E);
				break;

			}
		}
	}

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
	if(!(_mode==GUN or _mode==INPUTFILE or _mode==GENERATE or _mode==DCELASTIC or _mode==DCBREAKUP or _mode==MUON or _mode==DCELASTICWITHTIME)){
		std::stringstream o;
		o<<"Mode not recognized. Mode: "<<_mode<<G4endl;
		G4Exception("EventGenerator::SetMode()", "ArgumentError", JustWarning,
				o.str().c_str());
		this->_mode=oldmode;
	}
	if(_mode==INPUTFILE){
		if(_infile==""){
			G4Exception("[EventGenerator]", "setMode", JustWarning,
					" ERROR: Set input file before switching mode.");
			this->_mode=oldmode;
		}
		else{
			G4AutoLock lock(&PrimaryGeneratorMutex);
			if(!fileReader)
				fileReader = new PrimaryGeneratorAction::FileReader(_infile);
		}
	}
	if(_mode==MUON and !dynamic_cast<CosmicMuonGenerator*>(evtGen)){
		if(evtGen)
			delete evtGen;
		evtGen=new CosmicMuonGenerator(_pGun);
	}
	if(_mode==DCELASTIC and !dynamic_cast<DCElasticEventGenerator*>(evtGen)){
		if(evtGen)
			delete evtGen;
		evtGen=new DCElasticEventGenerator(_pGun);
	}
	if(_mode==DCBREAKUP and !dynamic_cast<DCBreakupEventGenerator*>(evtGen)){
		if(evtGen)
			delete evtGen;
		evtGen=new DCBreakupEventGenerator(_pGun);
	}
	if(_mode==DCELASTICWITHTIME and !dynamic_cast<DCElasticTimeDependentGenerator*>(evtGen)){
		if(evtGen)
			delete evtGen;
		evtGen=new DCElasticTimeDependentGenerator(_pGun);
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
	case DCELASTIC:
		evtGen->Generate(E);
		break;
	case DCBREAKUP:
		evtGen->Generate(E);
		break;
	case MUON:
		evtGen->Generate(E);
		break;
	case DCELASTICWITHTIME:
		evtGen->Generate(E);
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

	G4GenericMessenger::Command& illuminateCmd
	= fMessenger->DeclarePropertyWithUnit("illuminateAngle","deg",illuminationAngle,"illuminateAngle");

}
// eof

