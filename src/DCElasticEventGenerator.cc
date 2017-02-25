/*
 * DCElasticEventGenerator.cc
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#include "DCElasticEventGenerator.hh"
#include "G4ParticleDefinition.hh"
#include <G4Deuteron.hh>
#include <G4Event.hh>
#include "G4IonTable.hh"
#include "G4ios.hh"
#include "TCanvas.h"
#include "TH1.h"
#include "Randomize.hh"
#include "G4GenericMessenger.hh"
#include "Analysis.hh"
#include "TVector3.h"
#include "G4GenericMessenger.hh"
#include <math.h>
#include "G4ParticleGun.hh"
#include <signal.h>
#include "Randomize.hh"
#include "VertexGeneratorO.hh"
#include "VertexGeneratorU.hh"

#include "JediScatteringHelperFunctions.hh"
using namespace CLHEP;
using namespace JediScatteringHelperFunctions::elastic;
DCElasticEventGenerator::DCElasticEventGenerator():PhaseSpaceGenerator("dcelastic"){

	if(gConfig.count("generator.beam_polarization")){
		fBeamPolarization=gConfig["generator.beam_polarization"].as<double>()*CLHEP::deg;
	}
	else
		fBeamPolarization=Double_t(0);

	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DCElasticEventGenerator::Initialize() {
	fParticles.clear();
	fParticles.push_back(G4Deuteron::DeuteronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	for(auto ipart=fParticles.begin();ipart!=fParticles.end();++ipart){
		if(!(*ipart))
			G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",FatalException,"beam particle not found.");
	}
	if(gVerbose>2)
		G4cout<<"Theta_Min="<<fThetaMin/CLHEP::deg<<" Theta_Max="<<fThetaMax/CLHEP::deg<<G4endl;
	G4double m_target = fParticles[1]->GetPDGMass();
	G4double m_beam = fParticles[0]->GetPDGMass();
	fTarget.set(0,0,0,m_target);
	Double_t masses[2] = {m_beam, m_target} ;
	fBeam.set(0, 0, sqrt(fBeamEnergy*(fBeamEnergy+2*m_beam)), fBeamEnergy+m_beam);
	fCms = fBeam + fTarget;
	G4LorentzVector temp=fBeam;

	temp.boost(-fCms.boostVector());
	fMomentumCMS=temp.vect().mag();
	fQ=std::unique_ptr<TF1>(new TF1("q",q,0,TMath::Pi(),1));
	fPhi=std::unique_ptr<TF1>(new TF1("Phi",phi,0,2*TMath::Pi(),3));
	fQ->SetParameter(0,fBeamEnergy/MeV);
	fPhi->SetParameter(0,fBeamEnergy/MeV);
	fPhi->SetParameter(1,fBeamPolarization);

	fQmin=2*fMomentumCMS/GeV*sin(fThetaMin/2);
	fQmax=2*fMomentumCMS/GeV*sin(fThetaMax/2);

	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX()/CLHEP::mm,fBeamposition.getY()/CLHEP::mm,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX()/CLHEP::mm,fBeamsize.getY()/CLHEP::mm,fBeamsize.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamposition(0,0,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ()/CLHEP::mm);

	fInitialized=true;

}

void DCElasticEventGenerator::DefineCommands()
{
	fMessenger->DeclareMethod("polarization", &DCElasticEventGenerator::setBeamPolarization, "beam polarization");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
genvertex_t DCElasticEventGenerator::Generate() {
	if(!fInitialized)
		Initialize();
	genvertex_t thisEvent;
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	thisEvent.x=pos.x();
	thisEvent.y=pos.y();
	thisEvent.z=pos.z();
	auto q=fQ->GetRandom(fQmin,fQmax)*CLHEP::GeV;
	G4LorentzVector d_momentum_cms;

	d_momentum_cms.set(fMomentumCMS,0,0,sqrt(fMomentumCMS*fMomentumCMS+fParticles[0]->GetPDGMass()*fParticles[0]->GetPDGMass()));
	auto thetaCMS=2*TMath::ASin(q/2/fMomentumCMS);

	d_momentum_cms.setTheta(thetaCMS);
	auto d_momentum_lab=d_momentum_cms;
	d_momentum_lab.boost(fCms.boostVector());

	auto thetaLab=d_momentum_lab.theta();
	fPhi->SetParameter(2,thetaLab/CLHEP::deg);
	auto phi=fPhi->GetRandom(0,2*CLHEP::pi);

	d_momentum_cms.setPhi(phi);
	d_momentum_lab.setPhi(phi);

	auto carbon_momentum_cms=G4LorentzVector(-d_momentum_cms.vect(),sqrt(fMomentumCMS*fMomentumCMS+fParticles[1]->GetPDGMass()*fParticles[1]->GetPDGMass()));
	auto carbon_momentum_lab=carbon_momentum_cms;
	carbon_momentum_cms.boost(fCms.boostVector());

	thisEvent.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),
			carbon_momentum_cms.x()/GeV,carbon_momentum_lab.y()/GeV,carbon_momentum_lab.z()/GeV,
			carbon_momentum_lab.e()/GeV-fParticles[1]->GetPDGMass()/GeV));
	thisEvent.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),
			d_momentum_lab.x()/GeV,d_momentum_lab.y()/GeV,d_momentum_lab.z()/GeV,
			d_momentum_lab.e()/GeV-fParticles[0]->GetPDGMass()/GeV));
	return thisEvent;
}
