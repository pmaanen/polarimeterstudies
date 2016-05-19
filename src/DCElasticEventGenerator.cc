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

static double DegToRad=3.14159265359/180.;
//static double RadToDeg=1/DegToRad;
DCElasticEventGenerator::DCElasticEventGenerator(G4ParticleGun* pgun):PhaseSpaceGenerator(pgun),fScatteringModel(nullptr){
	fBeamEnergy=270.*CLHEP::MeV;
	fBeamPolarization=Double_t(2./3.);
	fInitialized=false;

	fThetaMin=5*CLHEP::deg;
	fThetaMax=20*CLHEP::deg;
	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DCElasticEventGenerator::~DCElasticEventGenerator() {
	if(fScatteringModel)
		delete fScatteringModel;
	fScatteringModel=0;

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
		fScatteringModel=new elastic_scattering_model(fBeamEnergy/CLHEP::MeV,fBeamPolarization);
	else{
		fScatteringModel->setBeamEnergy(fBeamEnergy/CLHEP::MeV);
		fScatteringModel->setBeamPolarization(fBeamPolarization);

	}
	fMaxY=2*fScatteringModel->SigmaUnpol(fBeamEnergy/CLHEP::MeV,sin(fThetaMin/2.)*2*fMomentumCMS/CLHEP::GeV);


	VertexGeneratorO::GetInstance()->setBeamposition(fBeamspot.getX(),fBeamspot.getY(),fBeamspot.getZ());
	VertexGeneratorO::GetInstance()->setBeamsize(fSpotsize.getX(),fSpotsize.getY(),fSpotsize.getZ());
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fSpotsize.getZ());
	fInitialized=true;
}

void DCElasticEventGenerator::DefineCommands()
{
	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dcelastic/", "elastic event generator control");
	fMessenger->DeclarePropertyWithUnit("beamspot","mm",PhaseSpaceGenerator::fBeamspot,"position of beam centroid.");
	fMessenger->DeclarePropertyWithUnit("beamsize","mm",PhaseSpaceGenerator::fSpotsize,"beam size.");
	fMessenger->DeclareMethod("polarization", &DCElasticEventGenerator::setBeamPolarization, "beam polarization");
	fMessenger->DeclareMethod("energy", &DCElasticEventGenerator::setBeamEnergy, "beam energy");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
genevent_t DCElasticEventGenerator::Generate() {
	if(!fInitialized)
		Initialize();

	auto position=fParticleGun->GetParticlePosition();
	G4double x=position.x(),y=position.y(),z=position.z();
	if(fSpotsize.x()>0)
		x+=G4RandGauss::shoot(0,fSpotsize.x());
	if(fSpotsize.y()>0)
		y+=G4RandGauss::shoot(0,fSpotsize.y());
	if(fSpotsize.z()>0)
		z+=G4RandGauss::shoot(0,fSpotsize.z());
	while (1) {
		fBeam.RotateX(fTiltX+G4RandGauss::shoot(fTiltX,fXPrime));
		fBeam.RotateY(fTiltY+G4RandGauss::shoot(fTiltY,fYPrime));

		//Sample an event assuming constant cross-section in cm-system
		fPhaseSpace.Generate();
		//L-vector of scattered particle in lab-frame
		TLorentzVector deuteron_4 = *fPhaseSpace.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector carbon_4 = *fPhaseSpace.GetDecay(1) ;

		//spatial parts of generated L-vectors
		G4ThreeVector carbon_3(carbon_4.Vect().X()*GeV,carbon_4.Vect().Y()*GeV,carbon_4.Vect().Z()*GeV);
		G4ThreeVector deuteron_3(deuteron_4.Vect().X()*GeV,deuteron_4.Vect().Y()*GeV,deuteron_4.Vect().Z()*GeV);

		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = deuteron_3.getTheta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = deuteron_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered>fThetaMin and th_scattered<fThetaMax){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = fCms.BoostVector();     // in case beam simulation

			//deuteron_4.Boost(-CMv);          // in case beam simulation
			G4double acc=fMaxY*G4UniformRand();
			if(fScatteringModel->sigma(fBeam,deuteron_4)<acc){
				continue;
			}
			else {
				genevent_t res(0,0,x/CLHEP::mm,y/CLHEP::mm,z/CLHEP::mm);
				res.particles.push_back(particle_t(fParticles[1]->GetPDGEncoding(),carbon_3.getX()/CLHEP::GeV,carbon_3.getY()/CLHEP::GeV,carbon_3.getZ()/CLHEP::GeV,carbon_4.Energy()*GeV-carbon_4.M()*GeV));
				res.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),deuteron_3.getX()/CLHEP::GeV,deuteron_3.getY()/CLHEP::GeV,deuteron_3.getZ()/CLHEP::GeV,deuteron_4.Energy()*GeV-deuteron_4.M()*GeV));
				return res;
			}
		}
		else{
		}
	}
}

elastic_scattering_model::elastic_scattering_model(Double_t beamEnergy,Double_t beamPolarization):fBeamEnergy(beamEnergy),fBeamPolarization(beamPolarization){}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Double_t elastic_scattering_model::sigma(TLorentzVector in, TLorentzVector out) {
	auto q=sqrt(-(out-in).M2());
	auto phi=out.Vect().Phi()*CLHEP::rad;
	auto theta=out.Vect().Theta()*CLHEP::rad;
	return SigmaUnpol(fBeamEnergy,q)*(1+fBeamPolarization*Ay(fBeamEnergy,theta/CLHEP::deg)*cos(phi));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double elastic_scattering_model::SigmaUnpol(Double_t E,Double_t q) {
	double w=log(E);
	return pow(10,a1(w)+a2(w)*q+(1+a5(w)*q)*(a3(w)*sin(a6(w)*q)+a4(w)*cos(a6(w)*q)));
}

double elastic_scattering_model::a1(Double_t x) {
	return -79.173+46.741*x-8.662*x*x+0.52986*x*x*x;
}

double elastic_scattering_model::a2(Double_t x) {
	return 78.051-34.263*x+3.341*x*x;

}

double elastic_scattering_model::a3(Double_t x) {
	return -38.231+19.774*x-3.1487*x*x+0.14791*x*x*x;
}

double elastic_scattering_model::a4(Double_t x) {
	return 9.1111-3.2832*x+0.28525*x*x;
}

double elastic_scattering_model::a5(Double_t x) {
	return -3.6126+0.18644*x;
}

double elastic_scattering_model::a6(Double_t x) {
	return 23.91+0.66962*x;
}

double elastic_scattering_model::Ay(Double_t E, Double_t theta) {
	return (1/(1+900./pow(theta,4)))*
			(b4(E)-b1(E)/(1+exp((theta-b2(E))/b3(E)))+b5(E)*(1-theta/b6(E))*sin(b7(E)+b8(E)*theta));
}

double elastic_scattering_model::b1(Double_t E) {
	return  1.1556+0.007182*(E-150)+1.3524e-5*pow(( E-150),2)-5.5448e-7*pow(( E-150),3);

}

double elastic_scattering_model::b2(Double_t E) {
	return 16.029-0.24658*(E-150)+8.6972e-4*pow((E-150),2);

}

double elastic_scattering_model::b3(Double_t E) {
	return 6.8319+0.052974*( E-150)+6.4864e-4*pow(E-150,2)-4.7648e-6*pow(E-150,3);

}

double elastic_scattering_model::b4(Double_t E) {
	return 0.94964+8.2885e-4*( E-150)-5.4014e-6*pow(E-150,2);

}

double elastic_scattering_model::b5(Double_t E) {
	return 0.21588+1.3908e-4*( E-150);

}

double elastic_scattering_model::b6(Double_t E) {
	return 42.467-0.25468*( E-150)+0.0033973*pow(E-150,2);

}

double elastic_scattering_model::b7(Double_t E) {
	return 58.573-0.41812*( E-150);

}

double elastic_scattering_model::b8(Double_t E) {
	return 25.698+0.091205*( E-150)-1.8594e-4*pow(E-150,2);

}
