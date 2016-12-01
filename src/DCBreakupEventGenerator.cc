/*
 * DCBreakupEventGenerator.cc
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#include <DCBreakupEventGenerator.hh>

#include "global.hh"

#include "VertexGeneratorO.hh"
#include "VertexGeneratorU.hh"

#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "TMath.h"
#include "TF2.h"
#include "G4ParticleGun.hh"
#include "DeuteronCarbonBreakupModel.hh"
//static double DegToRad=3.14159265359/180.;
//static double RadToDeg=1/DegToRad;

DCBreakupEventGenerator::DCBreakupEventGenerator():PhaseSpaceGenerator("dcbreakup"){
	if(gConfig.count("generator.beam_polarization")){
		fBeamPolarization=gConfig["generator.beam_polarization"].as<double>()*CLHEP::deg;
	}
	else
		fBeamPolarization=Double_t(2./3.);
	DefineCommands();
}

void DCBreakupEventGenerator::DefineCommands() {
	fMessenger->DeclareMethod("polarization", &DCBreakupEventGenerator::setBeamPolarization, "beam polarization beakup");
}

void DCBreakupEventGenerator::Initialize() {
	fParticles.push_back(G4Proton::ProtonDefinition());
	fParticles.push_back(G4Neutron::NeutronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));


	//fQ=std::unique_ptr<TF1>(new TF1("q",fScatteringModel.get(),&DeuteronCarbonElasticScatteringModel::q,0,TMath::Pi(),1,"DeuteronCarbonElasticScatteringModel","q"));
	for(auto ipart=fParticles.begin();ipart!=fParticles.end();++ipart){
		if(!(*ipart))
			throw;//G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",0,"beam particle not found.");
	}
	Double_t m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;
	Double_t m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	fTarget.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[3] = {fParticles[0]->GetPDGMass()/CLHEP::GeV,fParticles[1]->GetPDGMass()/CLHEP::GeV,fParticles[2]->GetPDGMass()/CLHEP::GeV} ;
	fBeam.SetPxPyPzE(0, 0, sqrt(fBeamEnergy/CLHEP::GeV*(fBeamEnergy/CLHEP::GeV+2*m_beam)), fBeamEnergy/CLHEP::GeV+m_beam);
	fCms = fBeam + fTarget;
	fPhaseSpace.SetDecay(fCms, 3, masses);
	if(!fScatteringModel)
		fScatteringModel=std::unique_ptr<DeuteronCarbonBreakupModel>(new DeuteronCarbonBreakupModel(fBeamEnergy/CLHEP::MeV,fBeamPolarization));
	else{
		fScatteringModel->setBeamEnergy(fBeamEnergy/CLHEP::MeV);
		fScatteringModel->setBeamPolarization(fBeamPolarization);
	}

	TLorentzVector temp=fCms;
	temp.Boost(-fCms.BoostVector());

	auto emax=temp.Energy()-temp.M();
	G4cout<<"Maximum Ex="<<emax<<" GeV"<<G4endl;
	fThetaEx=std::unique_ptr<TF2>(new TF2("ThetaEx",fScatteringModel.get(),&DeuteronCarbonBreakupModel::ThetaEx,0,TMath::Pi(),0,emax*1000,1));
	fPhi=std::unique_ptr<TF1>(new TF1("Phi",fScatteringModel.get(),&DeuteronCarbonBreakupModel::Phi,0,2*TMath::Pi(),4,"DeuteronCarbonElasticScatteringModel","Phi"));
	fInitialized=true;
}

genevent_t DCBreakupEventGenerator::Generate() {

	if(!fInitialized)
		Initialize();

	genevent_t res;
	return res;
}
