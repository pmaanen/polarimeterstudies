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


using namespace CLHEP;
DCBreakupEventGenerator::DCBreakupEventGenerator():PhaseSpaceGenerator("dcbreakup"){
	if(gConfig.count("generator.beam_polarization")){
		fBeamPolarization=gConfig["generator.beam_polarization"].as<double>()*CLHEP::deg;
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


	//fQ=std::unique_ptr<TF1>(new TF1("q",fScatteringModel.get(),&DeuteronCarbonElasticScatteringModel::q,0,TMath::Pi(),1,"DeuteronCarbonElasticScatteringModel","q"));
	for(auto ipart=fParticles.begin();ipart!=fParticles.end();++ipart){
		if(!(*ipart))
			throw;//G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",0,"beam particle not found.");
	}
	Double_t m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;
	Double_t m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	fTarget.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	fBeam.SetPxPyPzE(0, 0, sqrt(fBeamEnergy/CLHEP::GeV*(fBeamEnergy/CLHEP::GeV+2*m_beam)), fBeamEnergy/CLHEP::GeV+m_beam);
	fCms = fBeam + fTarget;
	if(!fScatteringModel)
		fScatteringModel=std::unique_ptr<DeuteronCarbonBreakupModel>(new DeuteronCarbonBreakupModel());

	auto Ex_max=fCms.M()*CLHEP::GeV-(fParticles[0]->GetPDGMass()+fParticles[1]->GetPDGMass());



	fThetaEx=std::unique_ptr<TF2>(new TF2("ThetaEx",fScatteringModel.get(),&DeuteronCarbonBreakupModel::ThetaEx,0,180,0,Ex_max/CLHEP::MeV*0.8,5));

	fThetaEx->SetParameter(0,fBeamEnergy/CLHEP::MeV);
	fThetaEx->SetParameter(1,fThetaMin/CLHEP::deg);
	fThetaEx->SetParameter(2,fThetaMax/CLHEP::deg);
	fThetaEx->SetParameter(3,0/CLHEP::deg);
	fThetaEx->SetParameter(4,0.8*Ex_max/CLHEP::MeV);

	fPhi=std::unique_ptr<TF1>(new TF1("Phi",fScatteringModel.get(),&DeuteronCarbonBreakupModel::Phi,0,2*TMath::Pi(),4,"DeuteronCarbonElasticScatteringModel","Phi"));

	fThetaEx->SetNpx(200);
	fThetaEx->SetNpy(200);


	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX()/CLHEP::mm,fBeamposition.getY()/CLHEP::mm,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX()/CLHEP::mm,fBeamsize.getY()/CLHEP::mm,fBeamsize.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamposition(0,0,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ()/CLHEP::mm);



	fInitialized=true;
}

genevent_t DCBreakupEventGenerator::Generate() {

	if(!fInitialized){
		Initialize();
	}

	genevent_t thisEvent;
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());

	thisEvent.x=pos.x();
	thisEvent.y=pos.y();
	thisEvent.z=pos.z();

	Double_t Theta,Ex;
	fThetaEx->GetRandom2(Theta,Ex);
	/*
	 * x[0]=Phi
	 * par[0]=Polarization
	 * par[1]=E
	 * par[2]=theta
	 * par[3]=Ex
	 */
	fPhi->SetParameters(fBeamPolarization,fBeamEnergy/CLHEP::MeV,Theta,Ex);
	Double_t Phi=fPhi->GetRandom();

	//G4cout<<G4BestUnit(fBeamEnergy,"Energy")<<G4endl;
	auto s=fCms.M2();
	auto md=G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	auto mc12=G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;

	auto mp=fParticles[0]->GetPDGMass()/CLHEP::GeV;
	auto mc=mc12+G4Neutron::NeutronDefinition()->GetPDGMass()/CLHEP::GeV+Ex/1000.;
	auto pcm=sqrt(((s-mp*mp-mc*mc)*(s-mp*mp-mc*mc)-4*mp*mp*mc*mc)/4/s);

	auto pcm_1=sqrt(((s-mc12*mc12-md*md)*(s-mc12*mc12-md*md)-4*md*md*mc12*mc12)/4/s);
	auto rap=log((pcm_1+sqrt(mc12*mc12+pcm_1*pcm_1))/mc12);
	auto t=Theta*TMath::DegToRad();
	auto pp=(sqrt(mp*mp+pcm*pcm)*cos(t)*sinh(rap)+cosh(rap)*sqrt(pcm*pcm-mp*mp*sin(t)*sin(t)*sinh(rap)*sinh(rap))) / (1+sinh(rap)*sinh(rap)*sin(t)*sin(t));

	Double_t Ep=(sqrt(mp*mp+pp*pp)-mp);
	thisEvent.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),pp*sin(t)*cos(Phi),pp*sin(t)*sin(Phi),pp*cos(t),Ep*GeV/MeV));
	return thisEvent;
}
