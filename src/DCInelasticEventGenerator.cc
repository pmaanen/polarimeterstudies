/*
 * DCInelasticEventGenerator.cc
 *
 *  Created on: 18.03.2016
 *      Author: pmaanen
 */

#include <DCInelasticEventGenerator.hh>
#include "G4Deuteron.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "TMath.h"
#include "G4ParticleGun.hh"
#include "global.hh"
#include "VertexGeneratorO.hh"
#include "VertexGeneratorU.hh"
DCInelasticEventGenerator::DCInelasticEventGenerator(G4ParticleGun* pgun):PhaseSpaceGenerator(pgun),fBeamPolarization(0) {
	fBeamEnergy=270.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=2.0*CLHEP::deg;
	fThetaMax=180.*CLHEP::deg;
	fScatteringModel=std::unique_ptr<inelastic_scattering_model>(new inelastic_scattering_model(fBeamPolarization));
	//DefineCommands();

}

void DCInelasticEventGenerator::Generate(G4Event* E) {
	auto event=genevent_t(Generate());
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
	}
	return;
}

genevent_t DCInelasticEventGenerator::Generate() {


	if(!fInitialized)
		Initialize();
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		fPhaseSpace.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector deuteron_4 = *fPhaseSpace.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector carbon_4 = *fPhaseSpace.GetDecay(1) ;

		//L-vector of recoil particle in lab-frame
		TLorentzVector gamma_4 = *fPhaseSpace.GetDecay(2) ;

		//Polar angle for deuteron in lab-frame
		G4double th_scattered  = deuteron_4.Vect().Theta()*CLHEP::rad;

		G4double phi_scattered = deuteron_4.Vect().Phi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		auto gammaCMS=gamma_4;
		gammaCMS.Boost(-fCms.BoostVector());

		G4double Ex=gammaCMS.Energy()*CLHEP::GeV;
		carbon_4=carbon_4+gamma_4;

		if(th_scattered>fThetaMin and th_scattered<fThetaMax){
			G4double acc=100*G4UniformRand();
			auto d3=deuteron_4.Vect();
			auto c3=carbon_4.Vect();
			G4ThreeVector deuteron_3(d3.X()*CLHEP::GeV,d3.Y()*CLHEP::GeV,d3.Z()*CLHEP::GeV);
			G4ThreeVector carbon_3(c3.X()*CLHEP::GeV,c3.Y()*CLHEP::GeV,c3.Z()*CLHEP::GeV);
			auto q4=fBeam-deuteron_4;
			auto q=sqrt(-(q4*q4))*CLHEP::GeV;
			if(gVerbose>3)
				G4cout<<"q="<<q/CLHEP::GeV<<G4endl;

			if(fScatteringModel->Sigma(q/CLHEP::GeV,Ex/CLHEP::MeV)<acc){
				continue;
			}
			else{
				genevent_t res(0,0,pos.getX(),pos.getY(),pos.getZ());
				if(gVerbose>3)
					G4cout<<"DATA="<<Ex/CLHEP::MeV<<" "<<(deuteron_4.Energy()-deuteron_4.M())/CLHEP::MeV<<G4endl;
				auto excitedCarbon=G4IonTable::GetIonTable()->GetIon(6,12,Ex);
				res.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),deuteron_3.getX()/CLHEP::GeV,deuteron_3.getY()/CLHEP::GeV,deuteron_3.getZ()/CLHEP::GeV,deuteron_4.Energy()*CLHEP::GeV-deuteron_4.M()*CLHEP::GeV));
				res.particles.push_back(particle_t(excitedCarbon->GetPDGEncoding(),carbon_3.getX()/CLHEP::GeV,carbon_3.getY()/CLHEP::GeV,carbon_3.getZ()/CLHEP::GeV,carbon_4.Energy()*CLHEP::GeV-carbon_4.M()*CLHEP::GeV));
				return res;
			}
		}
	}

}

void DCInelasticEventGenerator::Initialize() {
	fParticles.push_back(G4Deuteron::DeuteronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	fParticles.push_back(G4Electron::ElectronDefinition());

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
		fScatteringModel=std::unique_ptr<inelastic_scattering_model>(new inelastic_scattering_model(fBeamPolarization));
	else{
		//fScatteringModel->setBeamPolarization(fBeamPolarization);
	}
	fMaxY=100;
	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX(),fBeamposition.getY(),fBeamposition.getZ());
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX(),fBeamsize.getY(),fBeamsize.getZ());
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ());
	//G4cout<<(fScatteringModel->c2(0,270)-fScatteringModel->c3(0)*fScatteringModel->c4(0))<<G4endl;


	fInitialized=true;
}
