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

static double DegToRad=3.14159265359/180.;
//static double RadToDeg=1/DegToRad;

DCBreakupEventGenerator::DCBreakupEventGenerator(G4ParticleGun* gun):PhaseSpaceGenerator(gun),fScatteringModel(nullptr) {
	fBeamEnergy=113.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=0*CLHEP::deg;
	fThetaMax=180.*CLHEP::deg;
	DefineCommands();
}

DCBreakupEventGenerator::~DCBreakupEventGenerator() {
	// TODO Auto-generated destructor stub
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
		fScatteringModel=new deuteron_breakup_model(fBeamEnergy,fBeamPolarization);
	else{
		fScatteringModel->setBeamEnergy(fBeamEnergy/CLHEP::MeV);
		fScatteringModel->setBeamPolarization(fBeamPolarization);
	}
	TLorentzVector temp=fBeam;
	temp.Boost(-fCms.BoostVector());
	fMomentum_cms=temp.Vect().Mag();
	fMaxY=1000;
	VertexGeneratorO::GetInstance()->setBeamposition(fBeamspot.getX(),fBeamspot.getY(),fBeamspot.getZ());
	VertexGeneratorO::GetInstance()->setBeamsize(fSpotsize.getX(),fSpotsize.getY(),fSpotsize.getZ());
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fSpotsize.getZ());
	//G4cout<<(fScatteringModel->c2(0,270)-fScatteringModel->c3(0)*fScatteringModel->c4(0))<<G4endl;


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

	VertexGeneratorO::GetInstance()->setBeamposition(fBeamspot.getX(),fBeamspot.getY(),fBeamspot.getZ());
	VertexGeneratorO::GetInstance()->setBeamsize(fSpotsize.getX(),fSpotsize.getY(),fSpotsize.getZ());
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fSpotsize.getZ());

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
		//G4double th_recoil  = precoil_3.getTheta();

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
		Ex=(neutronCMS.E()-neutronCMS.M())*CLHEP::GeV;
		//G4cout<<G4BestUnit(Ex,"Energy")<<G4endl;
		//Set angular cut in lab-frame
		if(th_scattered>fThetaMin and th_scattered<fThetaMax){
			if(Ex<0)
				continue;
			G4double acc=100*G4UniformRand();
			auto p3=proton_4.Vect();
			auto c3=carbon_4.Vect();
			auto n3=neutron_4.Vect();
			G4ThreeVector proton_3(p3.X()*CLHEP::GeV,p3.Y()*CLHEP::GeV,p3.Z()*CLHEP::GeV);
			G4ThreeVector carbon_3(c3.X()*CLHEP::GeV,c3.Y()*CLHEP::GeV,c3.Z()*CLHEP::GeV);
			G4ThreeVector neutron_3(n3.X()*CLHEP::GeV,n3.Y()*CLHEP::GeV,n3.Z()*CLHEP::GeV);
			if(fScatteringModel->sigma(th_scattered/CLHEP::deg,0,Ex/CLHEP::MeV)<acc){
				continue;
			}
			else{
				genevent_t res(0,0,pos.getX(),pos.getY(),pos.getZ());
				if(gVerbose>3)
					G4cout<<"Ep="<<(proton_4.Energy()*CLHEP::GeV-proton_4.M()*CLHEP::GeV)/CLHEP::MeV<<G4endl;
				res.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),proton_3.getX()/CLHEP::GeV,proton_3.getY()/CLHEP::GeV,proton_3.getZ()/CLHEP::GeV,proton_4.Energy()*CLHEP::GeV-proton_4.M()*CLHEP::GeV));
				res.particles.push_back(particle_t(fParticles[1]->GetPDGEncoding(),neutron_3.getX()/CLHEP::GeV,neutron_3.getY()/CLHEP::GeV,neutron_3.getZ()/CLHEP::GeV,neutron_4.Energy()*CLHEP::GeV-neutron_4.M()*CLHEP::GeV));
				res.particles.push_back(particle_t(fParticles[2]->GetPDGEncoding(),carbon_3.getX()/CLHEP::GeV,carbon_3.getY()/CLHEP::GeV,carbon_3.getZ()/CLHEP::GeV,carbon_4.Energy()*CLHEP::GeV-carbon_4.M()*CLHEP::GeV));
				return res;
			}
		}
	}
}

Double_t deuteron_breakup_model::sigma(Double_t theta, Double_t phi, Double_t Ex) {
	return SigmaUnpol(fBeamEnergy,theta,Ex)*(1+fBeamPolarization*Ay(fBeamEnergy,theta,Ex)*cos(phi));
}

Double_t deuteron_breakup_model::SigmaUnpol(Double_t E, Double_t theta,
		Double_t Ex) {
	if(Ex>(c2(theta,E)-c3(theta)*c4(theta)))
		return c1(theta,E)*exp(-pow(Ex-c2(theta,E),2)/(2*c3(theta)*c3(theta)));
	else
		return c1(theta,E)*exp(-c4(theta)*c4(theta)/2.)*exp(-(c5(theta)*(c2(theta,E)-c4(theta)*c3(theta)-Ex))/c3(theta));

}

Double_t deuteron_breakup_model::c1(Double_t theta,Double_t E) {
	return sqrt(113./E)*exp(1.5586-0.07178*(theta-40.)+4.0733e-4*pow(theta-40.,2));
}

Double_t deuteron_breakup_model::c2(Double_t theta, Double_t E) {
	return E/113*(62.214+0.30061*(theta-40)-0.004704*pow(theta-40.,2));
}

Double_t deuteron_breakup_model::c3(Double_t theta) {
	return 14.384-0.048676*(theta-40);
}

Double_t deuteron_breakup_model::c4(Double_t theta) {
	return 0.74342+0.0069414*(theta-40);
}

Double_t deuteron_breakup_model::c5(Double_t theta) {
	return c3(theta)*(0.070367-3.3534e-4*(theta-40)+1.1845e-5*(theta-40)*(theta-40)+1.1436e-6*pow(theta-40,3));
}

Double_t deuteron_breakup_model::Ay(Double_t E, Double_t theta,
		Double_t Ex) {
	return d1(theta,E)+d2(theta,E)*(Ex-E/2.);
}

Double_t deuteron_breakup_model::d1(Double_t theta, Double_t E) {
	return -1280./E/E+0.528*theta/E;
}

Double_t deuteron_breakup_model::d2(Double_t theta, Double_t E) {
	return -7.35/pow(E,3./2.)+6.2e-5*(theta-40)+11.7/pow(E,3)*pow(theta-40,2)-3.1e-7*pow(theta-40,3);
}

void DCBreakupEventGenerator::DefineCommands() {
	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dcbreakup/", "elastic event generator control");
	fMessenger->DeclarePropertyWithUnit("beamspot","mm",PhaseSpaceGenerator::fBeamspot,"position of beam centroid.");
	fMessenger->DeclarePropertyWithUnit("beamsize","mm",PhaseSpaceGenerator::fSpotsize,"beam size.");
	fMessenger->DeclareMethod("polarization", &DCBreakupEventGenerator::setBeamPolarization, "beam polarization");
	fMessenger->DeclareMethod("energy", &DCBreakupEventGenerator::setBeamEnergy, "beam energy");
}
