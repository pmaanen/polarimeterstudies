// File:	EventGenerator.cc
// Date:	18-Lan-2008 G.Macharashvili Dubna

#include "Analysis.hh"
#include "global.hh"
#include "DCElasticEventGenerator.hh"
#include "DCBreakupEventGenerator.hh"
#include "DCElasticTimeDependentGenerator.hh"
#include "CosmicMuonGenerator.hh"
#include "BeamGenerator.hh"
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
#include "hit.hh"

namespace { G4Mutex PrimaryGeneratorMutex = G4MUTEX_INITIALIZER; }

FileReader* PrimaryGeneratorAction::fgFileReader = 0;
PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction(),fInfileName(""),fInstream("",std::ifstream::in) {
	G4int Nparticle = 1 ;
	fParticleGun = new G4ParticleGun(Nparticle);
	fIlluminationAngle=-1;
	DefineCommands();
	fEvtGenerators["muon"]=new CosmicMuonGenerator(fParticleGun);
	fEvtGenerators["dcelastic"]=new DCElasticEventGenerator(fParticleGun);
	fEvtGenerators["dcbreakup"]=new DCBreakupEventGenerator(fParticleGun);
	fEvtGenerators["dcelastictime"]=new DCElasticTimeDependentGenerator(fParticleGun);
	fEvtGenerators["beam"]=new BeamGenerator(fParticleGun);
	fGeneratorName="gun";
	fParticleGun->SetParticleEnergy(gConfig["generator.beam_energy"].as<double>()*CLHEP::MeV);
	fParticleGun->SetParticleDefinition(G4Deuteron::DeuteronDefinition());
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete fParticleGun ;
}

void PrimaryGeneratorAction::generateEventFromPhaseSpace(G4Event *E)
{
	fGeneratorName.toLower();
	if(fEvtGenerators.count(fGeneratorName)==0){
		std::stringstream message;
		message<<"event generator "<<fGeneratorName<<" is not known.";
		G4Exception("PrimaryGeneratorAction::generateEventFromPhaseSpace","NO_SUCH_GENERATOR",RunMustBeAborted,message.str().c_str());
		return;
	}
	else{
		auto event=fEvtGenerators[fGeneratorName]->Generate();
		event.eventid=E->GetEventID();
		fGenEvent=event;
		for(auto ipart : event.particles){
			auto part=G4ParticleTable::GetParticleTable()->FindParticle(ipart.id);
			if(!part){
				G4int Z,A,lvl;
				Z=0;
				A=0;
				G4double En=0;
				auto ionFound=G4IonTable::GetIonTable()->GetNucleusByEncoding(ipart.id,Z,A,En,lvl);
				G4cout<<ionFound<<" "<<Z<<" "<<A<<" "<<G4endl;
				part=G4IonTable::GetIonTable()->GetIon(Z,A);
				if(!part){
					std::stringstream message;
					message<<"primary particle not found.particle id: "<<ipart.id;
					G4Exception("EventGenerator::generateEventFromInput()", "ParticleError", FatalException,
							message.str().c_str());
				}
				particle_t a_particle;
			}
			fParticleGun->SetParticleDefinition(part);
			//G4cout<<part->GetParticleName()<<" "<<G4BestUnit(ipart.px,"Energy")<<" "<<G4BestUnit(ipart.py,"Energy")<<" "<<G4BestUnit(ipart.pz,"Energy")<<G4endl;;
			fParticleGun->SetParticleMomentum(G4ThreeVector(ipart.px*CLHEP::GeV,ipart.py*CLHEP::GeV,ipart.pz*CLHEP::GeV)) ;
			fParticleGun->GeneratePrimaryVertex(E);
		}
	}
	return;
}

void PrimaryGeneratorAction::generateEventFromInput(G4Event *E)
{
	genevent_t evt;
	if(fgFileReader)
	{
		G4AutoLock lock(&PrimaryGeneratorMutex);
		evt = fgFileReader->GetEvent();
	}
	else{
		G4AutoLock lock(&PrimaryGeneratorMutex);
		fgFileReader=new FileReader(fInfileName);
		evt = fgFileReader->GetEvent();
	}


	fGenEvent=evt;
	for(auto ipart : evt.particles){
		auto part=G4ParticleTable::GetParticleTable()->FindParticle(ipart.id);
		if(!part){
			G4int Z,A,lvl;
			Z=0;
			A=0;
			G4double En=0;
			auto ionFound=G4IonTable::GetIonTable()->GetNucleusByEncoding(ipart.id,Z,A,En,lvl);
			G4cout<<ionFound<<" "<<Z<<" "<<A<<" "<<G4endl;
			part=G4IonTable::GetIonTable()->GetIon(Z,A);
			if(!part){
				std::stringstream message;
				message<<"primary particle not found.particle id: "<<ipart.id;
				G4Exception("EventGenerator::generateEventFromInput()", "ParticleError", FatalException,
						message.str().c_str());
			}
			particle_t a_particle;
		}

		fParticleGun->SetParticleDefinition(part);
		fParticleGun->SetParticleMomentum(G4ThreeVector(ipart.px*CLHEP::GeV,ipart.py*CLHEP::GeV,ipart.pz*CLHEP::GeV)) ;
		fParticleGun->GeneratePrimaryVertex(E);
	}
	return;

}

void PrimaryGeneratorAction::setInfile(G4String string)
{
	fInfileName=string;
	G4AutoLock lock(&PrimaryGeneratorMutex);
	if(!fgFileReader)
		fgFileReader = new FileReader(fInfileName);
	else
		delete fgFileReader;
	fgFileReader=new FileReader(fInfileName);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* E) {
	fGenEvent.particles.clear();
	if(fIlluminationAngle>0){
		illuminateAngle(E);
		return;
	}
	if(fGeneratorName=="gun"){
		fGenEvent.eventid=E->GetEventID();
		fGenEvent.time=0;
		particle_t aParticle;
		auto mom=fParticleGun->GetParticleMomentum()/CLHEP::GeV;

		if(mom<1e-5){
			auto mass=fParticleGun->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV;
			auto e=fParticleGun->GetParticleEnergy()/CLHEP::GeV+mass;
			mom=sqrt(e*e-mass*mass);
		}
		auto dir=fParticleGun->GetParticleMomentumDirection();
		aParticle.px=(mom*dir.getX());
		aParticle.py=(mom*dir.getY());
		aParticle.pz=(mom*dir.getZ());
		aParticle.id=fParticleGun->GetParticleDefinition()->GetPDGEncoding();
		aParticle.E=fParticleGun->GetParticleEnergy()/CLHEP::GeV;
		fGenEvent.particles.push_back(aParticle);
		fParticleGun->GeneratePrimaryVertex(E) ;
		return;
	}
	else if(fGeneratorName=="file"){
		if(fInfileName==""){
			G4Exception("[EventGenerator]", "GeneratePrimaries", RunMustBeAborted,
					" ERROR: Must set input file before first beamOn.");
		}
		generateEventFromInput(E);
		return;
	}
	else
		generateEventFromPhaseSpace(E);
	return;
}

void PrimaryGeneratorAction::illuminateAngle(G4Event* E) {
	auto oldparticle=this->fParticleGun->GetParticleDefinition();
	auto oldmomentum=this->fParticleGun->GetParticleMomentumDirection();
	auto oldposition=this->fParticleGun->GetParticlePosition();

	G4ThreeVector direction(0,0,1);
	direction.setTheta(fIlluminationAngle);
	this->fParticleGun->SetParticlePosition(G4ThreeVector(0,0,0));
	this->fParticleGun->SetParticleDefinition(G4Geantino::GeantinoDefinition());
	for(int i=0;i<360;i++){
		direction.setPhi(i*CLHEP::deg);
		this->fParticleGun->SetParticleMomentumDirection(direction);
		this->fParticleGun->GeneratePrimaryVertex(E);
	}
	this->fParticleGun->SetParticleDefinition(oldparticle);
	this->fParticleGun->SetParticleMomentumDirection(oldmomentum);
	this->fParticleGun->SetParticlePosition(oldposition);
	fIlluminationAngle=-1;
}

void PrimaryGeneratorAction::DefineCommands()
{
	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/generator/",
			"Generator control");

	G4GenericMessenger::Command& generator
	= fMessenger->DeclareProperty("setGenerator",fGeneratorName,"Set generator name");

	generator.SetGuidance("Possible values are:gun, file, muon, dcelastic, dcbreakup, dcelastictime, beam");

	fMessenger->DeclareProperty("setFilename",fInfileName,"Set input file name");
	fMessenger->DeclarePropertyWithUnit("illuminateAngle","deg",fIlluminationAngle,"illuminateAngle");
	fMessenger->DeclareMethod("list",&PrimaryGeneratorAction::listModes,"List available generators");
}
// eof

