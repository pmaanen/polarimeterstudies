// File:	EventGenerator.cc
// Date:	18-Lan-2008 G.Macharashvili Dubna

#include "Analysis.hh"
#include "global.hh"
#include "DCElasticEventGenerator.hh"
#include "DCBreakupEventGenerator.hh"
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
PrimaryGeneratorAction::PrimaryGeneratorAction():_infile(""),_instream("",std::ifstream::in),evtGen(0) {
	_mode=GUN;
	G4int Nparticle = 1 ;
	_pGun = new G4ParticleGun(Nparticle);
	evtGen=0;
	illuminationAngle=-1;
	DefineCommands();

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
	//check if input filename is set
	if(_infile==""){
		/*
		G4cerr<<"EventGenerator: Error. Input file not set "<<G4endl;
		G4Exception("[EventGenerator]", "generateEventFromInput()", FatalException,
				" ERROR: Input file not set.");
		 */
	}
	//check if input file is open
	if(!_instream){
		//if not, try to open
		_instream.open(_infile.c_str());
		//if not opened, file is not found, throw
		if(!_instream){
			/*
			G4cerr<<"EventGenerator: Error. Input file not found "<<G4endl;
			G4Exception("[EventGenerator]", "generateEventFromInput()", FatalException,
					" ERROR: Input file not found.");
			 */
		}
	}


	const	G4double ProtonMass = 938.27231 ;
	const	G4double DeuteronMass = 1875.61282 ;
	double d2r = 3.141592654/180;
	G4double Pmom, theta_lab, phi, Ek=10*MeV ;
	G4double pmom1, pmom2, theta1, phi1, theta2, phi2,vx,vy,vz,xp,yp;
	int iev, part1, part2 ;

	_instream >> iev >> part1 >> part2 >> pmom1 >> pmom2 >> theta1  >> theta2 >> phi1 >> phi2>>vx>>vy>>vz>>xp>>yp;
	try{
		Analysis::Instance()->GetObject<TNtuple>("GeneratorInfo")->Fill(pmom1,pmom2,phi1,phi2,theta1,theta2,vx,vy,vz,xp,yp);
	}
	catch(myG4Exception& exc){
		//If object is not found, book it and try again.
		if(exc.getExceptionCode()=="ObjectNotFound"){
			Analysis::Instance()->BookObject<TNtuple>("GeneratorInfo","GeneratorInfo","pmom1:pmom2:phi1:phi2:theta1:theta2:vx:vy:vz:xp:yp");
			Analysis::Instance()->GetObject<TNtuple>("GeneratorInfo")->Fill(pmom1,pmom2,phi1,phi2,theta1,theta2,vx,vy,vz,xp,yp);
		}
		//If it is something else, rethrow (which, in geant4 is not an exception, but a function *yik*)
		else
			G4Exception(exc.getOriginOfException().c_str(),exc.getOriginOfException().c_str(),exc.getSeverity(),exc.getDescription().c_str());
	}

	G4ThreeVector	v(vx,vy,vz);
	Pmom = pmom1 * 1000 ;
	theta_lab = theta1 * d2r ;
	phi       = phi1   * d2r ;
	if(part1 == 14) {
		_pGun->SetParticleDefinition(G4Proton::ProtonDefinition()) ;
		Ek = sqrt(Pmom*Pmom + ProtonMass*ProtonMass) - ProtonMass ;
	}
	else if(part1 == 45) {
		_pGun->SetParticleDefinition(G4Deuteron::DeuteronDefinition()) ;
		Ek = sqrt(Pmom*Pmom + DeuteronMass*DeuteronMass) - DeuteronMass ;
	}
	_pGun->SetParticleEnergy(Ek) ;
	_pGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta_lab)*cos(phi),sin(theta_lab)*sin(phi),cos(theta_lab))) ;
	_pGun->SetParticlePosition(v) ;
	_pGun->SetParticleTime(0) ;
	_pGun->GeneratePrimaryVertex(E) ;

	Pmom = pmom2 * 1000;
	theta_lab = theta2 * d2r ;
	phi       = phi2   * d2r ;
	if(part2 == 14) {
		_pGun->SetParticleDefinition(G4Proton::ProtonDefinition()) ;
		Ek = sqrt(Pmom*Pmom + ProtonMass*ProtonMass) - ProtonMass ;
	}
	else if(part2 == 45) {
		_pGun->SetParticleDefinition(G4Deuteron::DeuteronDefinition()) ;
		Ek = sqrt(Pmom*Pmom + DeuteronMass*DeuteronMass) - DeuteronMass ;
	}
	_pGun->SetParticleEnergy(Ek) ;
	_pGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta_lab)*cos(phi),sin(theta_lab)*sin(phi),cos(theta_lab))) ;
	_pGun->SetParticlePosition(v) ;
	_pGun->SetParticleTime(0) ;  
	_pGun->GeneratePrimaryVertex(E) ;

}

void PrimaryGeneratorAction::generateEventFromGun(G4Event *E)
{
	_pGun->GeneratePrimaryVertex(E) ;
}

void PrimaryGeneratorAction::setMode(G4int mode)
{
	this->_mode = static_cast<GeneratorMode>(mode);
	if(!(_mode==GUN or _mode==INPUTFILE or _mode==GENERATE or _mode==DCELASTIC or _mode==DCBREAKUP or _mode==MUON)){
		std::stringstream o;
		o<<"Mode not recognized. Mode: "<<_mode<<G4endl;
		G4Exception("EventGenerator::SetMode()", "ArgumentError", JustWarning,
				o.str().c_str());
	}
	if(_mode==MUON and !dynamic_cast<CosmicMuonGenerator*>(evtGen)){
		if(evtGen)
			delete evtGen;
		evtGen=new CosmicMuonGenerator(_pGun);
	}
	if(_mode==DCELASTIC and !dynamic_cast<DCElasticEventGenerator*>(evtGen))
		if(evtGen)
			delete evtGen;
	evtGen=new DCElasticEventGenerator();

	if(_mode==DCBREAKUP and !dynamic_cast<DCBreakupEventGenerator*>(evtGen)){
		if(evtGen)
		  delete evtGen;
		evtGen=new DCBreakupEventGenerator();
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
		generateEventFromGenerator(E);
		break;
	case DCBREAKUP:
		generateEventFromGenerator(E);
		break;
	case MUON:
		G4cout<<"MUON!"<<G4endl;
		generateEventFromGenerator(E);
		break;
	default:
		std::stringstream o;
		o<<"Mode not recognized. Mode: "<<_mode<<G4endl;
		G4Exception("EventGenerator::SetMode()", "ModeError", FatalException,
				o.str().c_str());
	}
}

void PrimaryGeneratorAction::generateEventFromGenerator(G4Event* E) {
	evtGen->Generate(E);
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

void PrimaryGeneratorAction::beginOfRun() {

	//if external generator is used, generator takes care of truth info
	if(_mode!=GUN and _mode!=INPUTFILE){
G4cout<<"PrimaryGeneratorAction::beginOfRun()"<<G4endl;
		evtGen->beginOfRun();
	}
	if(_mode==GUN){

	}
	if(_mode==INPUTFILE){

	}
}
// eof

