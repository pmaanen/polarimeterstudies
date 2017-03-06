/*
 * DCBreakupEventGenerator.cc
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#include <DCBreakupEventGenerator.hh>
#include <JediConfigurationManager.hh>

#include "VertexGeneratorO.hh"
#include "VertexGeneratorU.hh"

#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "TMath.h"
#include "TF2.h"
#include "G4ParticleGun.hh"
#include "JediScatteringHelperFunctions.hh"
#include "G4LorentzVector.hh"
using namespace CLHEP;
using namespace JediScatteringHelperFunctions::breakup;
DCBreakupEventGenerator::DCBreakupEventGenerator():PhaseSpaceGenerator("dcbreakup"){
	if(JediConfigurationManager::Instance()->GetMap().count("generator.beam_polarization")){
		fBeamPolarization=JediConfigurationManager::Instance()->GetMap()["generator.beam_polarization"].as<double>()*CLHEP::deg;
	}
	else
		fBeamPolarization=Double_t(2./3.);
	DefineCommands();

	fThetaMin=3*CLHEP::deg;
	fThetaMax=30*CLHEP::deg;
}

void DCBreakupEventGenerator::DefineCommands() {
	fMessenger->DeclareMethod("polarization", &DCBreakupEventGenerator::setBeamPolarization, "beam polarization beakup");
}

void DCBreakupEventGenerator::Initialize() {
	fParticles.clear();
	fParticles.push_back(G4Proton::ProtonDefinition());
	//fParticles.push_back(G4Neutron::NeutronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,13));

	for(auto ipart=fParticles.begin();ipart!=fParticles.end();++ipart){
		if(!(*ipart))
			throw;//G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",0,"beam particle not found.");
	}
	G4double m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass();
	G4double m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	fTarget.set(0.0, 0.0, 0.0, m_target);
	fBeam.set(0, 0, sqrt(fBeamEnergy*(fBeamEnergy+2*m_beam)), fBeamEnergy+m_beam);
	fCms = fBeam + fTarget;

	auto Ex_max=fCms.m()-(fParticles[0]->GetPDGMass()+fParticles[1]->GetPDGMass());

	fThetaEx=std::unique_ptr<TF2>(new TF2("ThetaEx",theta_ex,0,180,0,Ex_max/CLHEP::MeV*0.8,5));

	fThetaEx->SetParameter(0,fBeamEnergy/CLHEP::MeV);
	fThetaEx->SetParameter(1,fThetaMin/CLHEP::deg);
	fThetaEx->SetParameter(2,fThetaMax/CLHEP::deg);
	fThetaEx->SetParameter(3,0/CLHEP::deg);
	fThetaEx->SetParameter(4,0.8*Ex_max/CLHEP::MeV);

	fPhi=std::unique_ptr<TF1>(new TF1("Phi",phi,0,2*TMath::Pi(),4));

	fThetaEx->SetNpx(200);
	fThetaEx->SetNpy(200);


	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX()/CLHEP::mm,fBeamposition.getY()/CLHEP::mm,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX()/CLHEP::mm,fBeamsize.getY()/CLHEP::mm,fBeamsize.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamposition(0,0,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ()/CLHEP::mm);

	fInitialized=true;
}

genvertex_t DCBreakupEventGenerator::Generate() {

	if(!fInitialized){
		Initialize();
	}

	genvertex_t thisEvent;
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());

	thisEvent.x=pos.x();
	thisEvent.y=pos.y();
	thisEvent.z=pos.z();

	Double_t theta_lab,ex;
	fThetaEx->GetRandom2(theta_lab,ex);
	fPhi->SetParameters(fBeamPolarization,fBeamEnergy/CLHEP::MeV,theta_lab,ex);
	Double_t Phi=fPhi->GetRandom();

	ex*=CLHEP::MeV;
	G4LorentzVector cms=fTarget+fBeam;
	cms.boost(-cms.boostVector());

	auto carbon_mass=G4IonTable::GetIonTable()->GetIon(6,13)->GetPDGMass()+ex;
	G4double proton_mass=G4Proton::Proton()->GetPDGMass();


	G4double cms_tot_momentum=1/2/cms.m()*sqrt((cms.m2()-pow(proton_mass-carbon_mass,2))*((cms.m2()-pow(proton_mass+carbon_mass,2))));

	G4double proton_etot_cms=sqrt(proton_mass*proton_mass+cms_tot_momentum/2*cms_tot_momentum/2);

	auto proton_momentum_cms=G4LorentzVector(0,0,cms_tot_momentum/2,proton_etot_cms);
	auto proton_momentum_lab=proton_momentum_cms;
	proton_momentum_lab.boost(cms.boostVector());

	proton_momentum_lab.setTheta(theta_lab*CLHEP::deg);
	proton_momentum_lab.setPhi(Phi*CLHEP::deg);

	thisEvent.particles.push_back(particle_t(G4Proton::ProtonDefinition()->GetPDGEncoding(),
			proton_momentum_lab.z()/GeV, proton_momentum_lab.z()/GeV, proton_momentum_lab.z()/GeV,
			proton_momentum_lab.e()-proton_momentum_lab.m()));
	return thisEvent;
}
