/*
 * PhaseSpaceGenerator.cc
 *
 *  Created on: 28.01.2015
 *      Author: pmaanen
 */


#include "G4ParticleGun.hh"
#include <PhaseSpaceGenerator.hh>

PhaseSpaceGenerator::PhaseSpaceGenerator(G4ParticleGun* gun):fTiltX(0),fTiltY(0),fXPrime(0),fYPrime(0),fBeamspot(0,0,0),fSpotsize(0,0,0) {

	fBeamEnergy=235.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=5*CLHEP::deg;
	fThetaMax=20*CLHEP::deg;
	DefineCommands();

	fParticleGun=gun;
}

PhaseSpaceGenerator::~PhaseSpaceGenerator() {}

void PhaseSpaceGenerator::DefineCommands() {}

void PhaseSpaceGenerator::Generate(G4Event* E) {


	auto event=PrimaryEvent(Generate());
	fParticleGun->SetParticlePosition(G4ThreeVector(event.vx,event.vy,event.vz));
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
		an->FillNtupleIColumn(fTupleId[0],fTupleId[1],E->GetEventID());
		an->FillNtupleIColumn(fTupleId[0],fTupleId[2],iPart->id);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[3],iPart->px);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[4],iPart->py);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[5],iPart->pz);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[6],fParticleGun->GetParticlePosition().getX()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[7],fParticleGun->GetParticlePosition().getY()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[8],fParticleGun->GetParticlePosition().getZ()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[9],fParticleGun->GetParticleTime()/CLHEP::s);
		an->AddNtupleRow(fTupleId[0]);
	}
	return;
}
