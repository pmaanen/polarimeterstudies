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
#include "G4ParticleGun.hh"
#include "DeuteronCarbonBreakupModel.hh"
//static double DegToRad=3.14159265359/180.;
//static double RadToDeg=1/DegToRad;

DCBreakupEventGenerator::DCBreakupEventGenerator(G4ParticleGun* gun):PhaseSpaceGenerator(gun) {
	if(gConfig.count("generator.beam_polarization")){
		fBeamPolarization=gConfig["generator.beam_polarization"].as<double>()*CLHEP::deg;
	}
	else
		fBeamPolarization=Double_t(2./3.);

	DefineCommands();
}

DCBreakupEventGenerator::~DCBreakupEventGenerator() {}

void DCBreakupEventGenerator::DefineCommands() {
	fMessenger->DeclareMethod("polarization", &DCBreakupEventGenerator::setBeamPolarization, "beam polarization");
}

void DCBreakupEventGenerator::Initialize() {
	fParticles.push_back(G4Proton::ProtonDefinition());
	fParticles.push_back(G4Neutron::NeutronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
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
	TLorentzVector temp=fBeam;
	temp.Boost(-fCms.BoostVector());
	fMomentum_cms=temp.Vect().Mag();
	fMaxY=1000;
	fInitialized=true;
}

void DCBreakupEventGenerator::Generate(G4Event* E) {
	auto event=genevent_t(Generate());
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
	}
	return;
}

genevent_t DCBreakupEventGenerator::Generate() {

	if(!fInitialized)
		Initialize();

	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX()/CLHEP::mm,fBeamposition.getY()/CLHEP::mm,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX()/CLHEP::mm,fBeamsize.getY()/CLHEP::mm,fBeamsize.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamposition(0,0,fBeamposition.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ()/CLHEP::mm);

	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		fPhaseSpace.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector proton_4 = *fPhaseSpace.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector neutron_4 = *fPhaseSpace.GetDecay(1) ;

		//L-vector of recoil particle in lab-frame
		TLorentzVector carbon_4 = *fPhaseSpace.GetDecay(2) ;

		//Polar angle for proton in lab-frame (degrees)
		G4double th_scattered  = proton_4.Vect().Theta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = neutron_4.Vect().Theta()*CLHEP::rad;

		G4double phi_scattered = proton_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;


		G4double Ex=0;
		auto bound=neutron_4+carbon_4;

		auto beamCMS=fBeam;
		beamCMS.Boost(-fCms.BoostVector());
		auto protonCMS=proton_4;
		protonCMS.Boost(-fCms.BoostVector());
		auto carbonCMS=carbon_4;
		carbonCMS.Boost(-fCms.BoostVector());
		auto neutronCMS=neutron_4;;
		neutronCMS.Boost(-fCms.BoostVector());
		Ex=(bound.M()-carbon_4.M()-neutron_4.M())*CLHEP::GeV;
		if(th_scattered<fThetaMax and th_scattered>fThetaMin){
			if(Ex<0)
				continue;
			G4double acc=200*G4UniformRand();
			auto p3=proton_4.Vect();
			auto c3=carbon_4.Vect();
			auto n3=neutron_4.Vect();
			G4ThreeVector proton_3(p3.X()*CLHEP::GeV,p3.Y()*CLHEP::GeV,p3.Z()*CLHEP::GeV);
			G4ThreeVector carbon_3(c3.X()*CLHEP::GeV,c3.Y()*CLHEP::GeV,c3.Z()*CLHEP::GeV);
			G4ThreeVector neutron_3(n3.X()*CLHEP::GeV,n3.Y()*CLHEP::GeV,n3.Z()*CLHEP::GeV);
			if(fScatteringModel->sigma(th_scattered/CLHEP::deg,phi_scattered/CLHEP::deg,Ex/CLHEP::MeV)<acc){
				continue;
			}
			else{
				genevent_t res(0,0,pos.getX(),pos.getY(),pos.getZ());
				res.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),proton_3.getX()/CLHEP::GeV,proton_3.getY()/CLHEP::GeV,proton_3.getZ()/CLHEP::GeV,proton_4.Energy()*CLHEP::GeV-proton_4.M()*CLHEP::GeV));
				res.particles.push_back(particle_t(fParticles[1]->GetPDGEncoding(),neutron_3.getX()/CLHEP::GeV,neutron_3.getY()/CLHEP::GeV,neutron_3.getZ()/CLHEP::GeV,neutron_4.Energy()*CLHEP::GeV-neutron_4.M()*CLHEP::GeV));
				res.particles.push_back(particle_t(fParticles[2]->GetPDGEncoding(),carbon_3.getX()/CLHEP::GeV,carbon_3.getY()/CLHEP::GeV,carbon_3.getZ()/CLHEP::GeV,carbon_4.Energy()*CLHEP::GeV-carbon_4.M()*CLHEP::GeV));
				return res;
			}
		}
	}
}
