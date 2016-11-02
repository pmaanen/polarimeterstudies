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
using namespace CLHEP;

#include "DeuteronCarbonElasticScatteringModel.hh"
DCElasticEventGenerator::DCElasticEventGenerator(G4ParticleGun* pgun):PhaseSpaceGenerator(pgun,"dcelastic"){

	if(gConfig.count("generator.beam_polarization")){
		fBeamPolarization=gConfig["generator.beam_polarization"].as<double>()*CLHEP::deg;
	}
	else
		fBeamPolarization=Double_t(2./3.);

	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DCElasticEventGenerator::~DCElasticEventGenerator() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DCElasticEventGenerator::Initialize() {
	fParticles.clear();
	fParticles.push_back(G4Deuteron::DeuteronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	for(auto ipart=fParticles.begin();ipart!=fParticles.end();++ipart){
		if(!(*ipart))
			G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",FatalException,"beam particle not found.");
	}
	G4cout<<"Theta_Min="<<fThetaMin/CLHEP::deg<<" Theta_Max="<<fThetaMax/CLHEP::deg<<G4endl;
	Double_t m_target = fParticles[1]->GetPDGMass()/GeV;
	Double_t m_beam = fParticles[0]->GetPDGMass()/GeV;
	fTarget.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[2] = {m_beam, m_target} ;
	fBeam.SetPxPyPzE(0, 0, sqrt(fBeamEnergy/CLHEP::GeV*(fBeamEnergy/CLHEP::GeV+2*m_beam)), fBeamEnergy/CLHEP::GeV+m_beam);
	fCms = fBeam + fTarget;
	TLorentzVector temp=fBeam;
	temp.Boost(-fCms.BoostVector());
	fMomentumCMS=temp.Vect().Mag();
	fPhaseSpace.SetDecay(fCms, 2, masses);
	if(!fScatteringModel)
		fScatteringModel=std::unique_ptr<DeuteronCarbonElasticScatteringModel>(new DeuteronCarbonElasticScatteringModel(fBeamEnergy/CLHEP::MeV,fBeamPolarization));
	else{
		fScatteringModel->setBeamEnergy(fBeamEnergy/CLHEP::MeV);
		fScatteringModel->setBeamPolarization(fBeamPolarization);

	}
	fMaxY=2*fScatteringModel->SigmaUnpol(fBeamEnergy/CLHEP::MeV,sin(fThetaMin/2.)*2*fMomentumCMS/CLHEP::GeV);
	fInitialized=true;

	// TF1 * f = new TF1("f",fptr,&MyFunction::Evaluate,0,1,npar,"MyFunction","Evaluate");   // create TF1 class.
	fQ=std::unique_ptr<TF1>(new TF1("q",fScatteringModel.get(),&DeuteronCarbonElasticScatteringModel::q,0,TMath::Pi(),1,"DeuteronCarbonElasticScatteringModel","q"));
	fPhi=std::unique_ptr<TF1>(new TF1("Phi",fScatteringModel.get(),&DeuteronCarbonElasticScatteringModel::Phi,0,2*TMath::Pi(),3,"DeuteronCarbonElasticScatteringModel","Phi"));
	fQ->SetParameter(0,fBeamEnergy/MeV);
	fPhi->SetParameter(0,fBeamEnergy/MeV);
	fPhi->SetParameter(1,fBeamPolarization);

	fQmin=2*fMomentumCMS*sin(fThetaMin/2);
	fQmax=2*fMomentumCMS*sin(fThetaMax/2);

}

void DCElasticEventGenerator::DefineCommands()
{
	fMessenger->DeclareMethod("polarization", &DCElasticEventGenerator::setBeamPolarization, "beam polarization");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
genevent_t DCElasticEventGenerator::Generate() {
	if(!fInitialized)
		Initialize();
	genevent_t thisEvent;
	VertexGeneratorO::GetInstance()->setBeamposition(fBeamspot.getX()/CLHEP::mm,fBeamspot.getY()/CLHEP::mm,fBeamspot.getZ()/CLHEP::mm);
	VertexGeneratorO::GetInstance()->setBeamsize(fSpotsize.getX()/CLHEP::mm,fSpotsize.getY()/CLHEP::mm,fSpotsize.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamposition(0,0,fBeamspot.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fSpotsize.getZ()/CLHEP::mm);

	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	thisEvent.x=pos.x();
	thisEvent.y=pos.y();
	thisEvent.z=pos.z();
	while (1) {
		auto q=fQ->GetRandom(fQmin,fQmax);
		TLorentzVector d4(fMomentumCMS,0,0,fParticles[0]->GetPDGMass()/GeV);
		auto thetaCMS=2*TMath::ASin(q/2/fMomentumCMS);
		d4.SetTheta(thetaCMS);
		auto d4lab=TLorentzVector(d4);
		d4lab.Boost(fCms.BoostVector());
		auto thetaLab=d4lab.Theta();
		fPhi->SetParameter(2,thetaLab);
		auto phi=fPhi->GetRandom();
		d4.SetPhi(phi);
		auto c4=TLorentzVector(-d4.Vect(),fParticles[1]->GetPDGMass()/GeV);
		c4.Boost(fCms.BoostVector());

		thisEvent.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),d4lab.Vect().X(),d4lab.Vect().Y(),d4lab.Vect().Z(),fParticles[0]->GetPDGMass()/GeV));
		thisEvent.particles.push_back(particle_t(fParticles[1]->GetPDGEncoding(),c4.Vect().X(),c4.Vect().Y(),c4.Vect().Z(),fParticles[1]->GetPDGMass()/GeV));
		break;
	}
	return thisEvent;
}
