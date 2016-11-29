// File:	EventGenerator.cc
// Date:	18-Lan-2008 G.Macharashvili Dubna

#include "Analysis.hh"
#include "global.hh"
#include "DCInelasticEventGenerator.hh"
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
#include <memory>
#include <utility>
using namespace CLHEP;
#include "G4AutoLock.hh"
#include "hit.hh"

namespace { G4Mutex PrimaryGeneratorMutex = G4MUTEX_INITIALIZER; }

FileReader* PrimaryGeneratorAction::fgFileReader = 0;
PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction(),fInfileName(""),fInstream("",std::ifstream::in) {
	G4int Nparticle = 1 ;
	fParticleGun =std::unique_ptr<G4ParticleGun>(new G4ParticleGun(Nparticle));
	fIlluminationAngle=-1;
	DefineCommands();
	fEvtGenerators["muon"]=std::unique_ptr<CosmicMuonGenerator>(new CosmicMuonGenerator());
	fEvtGenerators["dcelastic"]=std::unique_ptr<DCElasticEventGenerator>(new DCElasticEventGenerator(fParticleGun.get()));
	fEvtGenerators["dcbreakup"]=std::unique_ptr<DCBreakupEventGenerator>(new DCBreakupEventGenerator(fParticleGun.get()));
	fEvtGenerators["dcelastictime"]=std::unique_ptr<DCElasticTimeDependentGenerator>(new DCElasticTimeDependentGenerator(fParticleGun.get()));
	fEvtGenerators["dcinelastic"]=std::unique_ptr<DCInelasticEventGenerator>(new DCInelasticEventGenerator(fParticleGun.get()));
	fEvtGenerators["beam"]=std::unique_ptr<BeamGenerator>(new BeamGenerator(fParticleGun.get()));
	fGeneratorName="gun";
	fParticleGun->SetParticleEnergy(gConfig["generator.beam_energy"].as<double>()*CLHEP::MeV);
	fParticleGun->SetParticleDefinition(G4Deuteron::DeuteronDefinition());
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {}

void PrimaryGeneratorAction::generateEventFromGenerator(G4Event *E)
{
	fGeneratorName.toLower();
	if(fEvtGenerators.count(fGeneratorName)==0){
		std::stringstream message;
		message<<"event generator "<<fGeneratorName<<" is not known.";
		G4Exception("PrimaryGeneratorAction::generateEventFromPhaseSpace","GeneratorError",RunMustBeAborted,message.str().c_str());
		return;
	}
	else{
		fGenEvent=fEvtGenerators[fGeneratorName]->Generate();
		fGenEvent.eventid=E->GetEventID();
		fParticleGun->SetParticlePosition(G4ThreeVector(fGenEvent.x*CLHEP::mm,fGenEvent.y*CLHEP::mm,fGenEvent.z*CLHEP::mm));
		for(auto ipart : fGenEvent.particles){
			auto part=G4ParticleTable::GetParticleTable()->FindParticle(ipart.id);
			if(!part){
				G4int Z,A,lvl;
				Z=0;
				A=0;
				G4double En=0;
				auto ionFound=G4IonTable::GetIonTable()->GetNucleusByEncoding(ipart.id,Z,A,En,lvl);
				G4cout<<ionFound<<" "<<Z<<" "<<A<<" "<<G4endl;
				part=G4IonTable::GetIonTable()->GetParticle(ipart.id);
				//G4cout<<part->GetParticleName()<<G4endl;
				if(!part){
					std::stringstream message;
					message<<"primary particle not found.particle id: "<<ipart.id;
					G4Exception("EventGenerator::generateEventFromInput()", "ParticleError", RunMustBeAborted,
							message.str().c_str());
				}
				particle_t a_particle;
			}
			fParticleGun->SetParticleDefinition(part);
			fParticleGun->SetParticleEnergy(ipart.E*CLHEP::GeV);
			fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ipart.px*CLHEP::GeV,ipart.py*CLHEP::GeV,ipart.pz*CLHEP::GeV)) ;
			fParticleGun->GeneratePrimaryVertex(E);
		}
	}
	return;
}

void PrimaryGeneratorAction::generateEventFromInput(G4Event *E)
{
	if(fInfileName==""){
		G4Exception("[EventGenerator]", "GeneratePrimaries", RunMustBeAborted,
				" ERROR: Must set input file before first beamOn.");
		return;
	}
	genevent_t evt;
	if(fgFileReader)
	{
		G4AutoLock lock(&PrimaryGeneratorMutex);
		fGenEvent = fgFileReader->GetEvent();
	}
	else{
		G4AutoLock lock(&PrimaryGeneratorMutex);
		fgFileReader=new FileReader(fInfileName);
		fGenEvent = fgFileReader->GetEvent();
	}
	for(auto ipart : fGenEvent.particles){
		auto part=G4ParticleTable::GetParticleTable()->FindParticle(ipart.id);
		if(!part){
			G4int Z,A,lvl;
			Z=0;
			A=0;
			G4double En=0;
			G4IonTable::GetIonTable()->GetNucleusByEncoding(ipart.id,Z,A,En,lvl);
			part=G4IonTable::GetIonTable()->GetIon(Z,A);
			if(!part){
				std::stringstream message;
				message<<"primary particle not found.particle id: "<<ipart.id;
				G4Exception("EventGenerator::generateEventFromInput()", "ParticleError", RunMustBeAborted,
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
		generateEventFromGun(E);
	}
	else if(fGeneratorName=="file"){
		generateEventFromInput(E);
	}
	else
		generateEventFromGenerator(E);
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
	fMessenger =std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,
			"/PolarimeterStudies/generator/",
			"Generator control"));

	G4GenericMessenger::Command& generator
	= fMessenger->DeclareProperty("setGenerator",fGeneratorName,"Set generator name");

	generator.SetGuidance("Possible values are:gun, file, muon, dcelastic, dcbreakup, dcelastictime, beam");

	fMessenger->DeclareProperty("setFilename",fInfileName,"Set input file name");
	fMessenger->DeclarePropertyWithUnit("illuminateAngle","deg",fIlluminationAngle,"illuminateAngle");
	auto cmd=fMessenger->DeclareMethod("list",&PrimaryGeneratorAction::listModes,"List available generators");
	cmd.SetToBeBroadcasted(true);
	fMessenger->DeclareMethod("Print",&PrimaryGeneratorAction::Print,"");

}

void PrimaryGeneratorAction::generateEventFromGun(G4Event* E) {
	fGenEvent.particles.clear();
	auto pos=fParticleGun->GetParticlePosition();
	fGenEvent.x=pos.x();
	fGenEvent.y=pos.y();
	fGenEvent.z=pos.z();
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
}
// eof

