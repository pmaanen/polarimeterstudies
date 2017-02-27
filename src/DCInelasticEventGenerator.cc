/*
 * DCInelasticEventGenerator.cc
 *
 *  Created on: 18.03.2016
 *      Author: pmaanen
 */

#include <DCInelasticEventGenerator.hh>
#include "JediScatteringHelperFunctions.hh"

#include "G4Deuteron.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "TMath.h"
#include "G4ParticleGun.hh"
#include "global.hh"
#include "VertexGeneratorO.hh"
#include "VertexGeneratorU.hh"
#include "TF1.h"

using namespace JediScatteringHelperFunctions::inelastic;
DCInelasticEventGenerator::DCInelasticEventGenerator():PhaseSpaceGenerator("dcinelastic") {
	fBeamEnergy=270.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=2.0*CLHEP::deg;
	fThetaMax=180.*CLHEP::deg;
	DefineCommands();
}

genvertex_t DCInelasticEventGenerator::Generate() {
	G4Exception("DCInelasticEventGenerator::Generate","",FatalException,"Not implemented!");

	if(!fInitialized)
		Initialize();
	genvertex_t thisEvent;
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	thisEvent.x=pos.x();
	thisEvent.y=pos.y();
	thisEvent.z=pos.z();
	return thisEvent;
}

void DCInelasticEventGenerator::Initialize() {
	fParticles.push_back(G4Deuteron::DeuteronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));

	G4double m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;
	G4double m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	fTarget.set(0.0, 0.0, 0.0, m_target);
	fBeam.set(0, 0, sqrt(fBeamEnergy*(fBeamEnergy+2*m_beam)), fBeamEnergy+m_beam);
	fCms = fBeam + fTarget;
	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX(),fBeamposition.getY(),fBeamposition.getZ());
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX(),fBeamsize.getY(),fBeamsize.getZ());
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ());
	fQEx=std::unique_ptr<TF2>(new TF2("qEx",q_ex,0,5,0,100,1));
	fPhi=std::unique_ptr<TF1>(new TF1("Phi",phi,0,2*TMath::Pi(),3));
	fInitialized=true;
}
