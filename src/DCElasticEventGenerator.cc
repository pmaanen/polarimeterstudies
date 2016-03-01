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
DCElasticEventGenerator::DCElasticEventGenerator(G4ParticleGun* pgun):PhaseSpaceGenerator(pgun),fCrossSection(0){
	fBeamEnergy=235.*CLHEP::MeV;
	fBeamPolarization=Double_t(2./3.);
	fInitialized=false;

	fThetaMin=3*CLHEP::deg;
	fThetaMax=30*CLHEP::deg;
	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DCElasticEventGenerator::~DCElasticEventGenerator() {
	if(fCrossSection)
		delete fCrossSection;
	fCrossSection=0;
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
	G4cout<<m_beam<<" "<<m_target<<G4endl;
	fTarget.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[2] = {m_beam, m_target} ;
	fBeam.SetPxPyPzE(0, 0, sqrt(fBeamEnergy/CLHEP::GeV*(fBeamEnergy/CLHEP::GeV+2*m_beam)), fBeamEnergy/CLHEP::GeV+m_beam);
	fCms = fBeam + fTarget;
	TLorentzVector temp=fBeam;
	temp.Boost(-fCms.BoostVector());
	fMomentumCMS=temp.Vect().Mag();
	fPhaseSpace.SetDecay(fCms, 2, masses);
	if(!fCrossSection)
		BuildFunction();
	fCrossSection->SetParameter(0,fBeamEnergy/CLHEP::MeV);
	fCrossSection->SetParameter(1,fBeamPolarization);
	G4cout<<fBeamEnergy/CLHEP::MeV<<G4endl;
	fMaxY=fCrossSection->Eval(0,0);//4600;//fCrossSection->Eval(0,0);
	Analysis* an=Analysis::Instance();
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
TF2* DCElasticEventGenerator::BuildFunction() {
	fScatteringModel=new elastic_scattering_model;
	fCrossSection=new TF2("xsec",fScatteringModel,&elastic_scattering_model::sigma,3.,30.,0.,360.,2,"MyFunction","sigma");
	fCrossSection->SetParName(0,"Energy");
	fCrossSection->SetParName(1,"Polarization");
	return fCrossSection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PrimaryEvent DCElasticEventGenerator::Generate() {
	if(!fInitialized)
		Initialize();
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	while (1) {
		//fBeam.RotateX(fTiltX+G4RandGauss::shoot(fTiltX,fXPrime));
		//fBeam.RotateY(fTiltY+G4RandGauss::shoot(fTiltY,fYPrime));

		//Sample an event assuming constant cross-section in cm-system
		fPhaseSpace.Generate();
		//L-vector of scattered particle in lab-frame
		TLorentzVector pscattered_4 = *fPhaseSpace.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector precoil_4 = *fPhaseSpace.GetDecay(1) ;

		//spatial parts of generated L-vectors
		G4ThreeVector precoil_3(precoil_4.Vect().X()*GeV,precoil_4.Vect().Y()*GeV,precoil_4.Vect().Z()*GeV);
		G4ThreeVector pscattered_3(pscattered_4.Vect().X()*GeV,pscattered_4.Vect().Y()*GeV,pscattered_4.Vect().Z()*GeV);

		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = pscattered_3.getTheta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = pscattered_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered>fThetaMin and th_scattered<fThetaMax){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			//TVector3 CMv = fCms.BoostVector();     // in case beam simulation

			//pscattered_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for deuteron in cm-frame
			//G4double CM_theta_scattered = pscattered_4.Theta()*CLHEP::rad;
			//auto x=G4UniformRand()*(fThetaMax-fThetaMin)+fThetaMin;

			//G4double acc=(2*2372.402057)*exp(-(x-fThetaMin)/((fThetaMax-fThetaMin)/log(2372.402057)));
			auto acc=fMaxY*G4UniformRand();
			auto q_4=pscattered_4-fBeam;
			auto q=sqrt(-q_4*q_4)*CLHEP::GeV;
			//G4cout<<"q "<<q<<" "<<CM_theta_scattered/CLHEP::deg<<G4endl;
			std::stringstream s;
			//s<<"energy="<<fCrossSection->GetParameter(0)<< " xsec="<<fCrossSection->Eval(q/CLHEP::GeV,0)<<" acc="<<acc<<" q=" <<q/CLHEP::GeV<<" "<<th_scattered/CLHEP::deg<<std::endl;
			//s.clear();
			s<<q/CLHEP::GeV<<" "<<th_scattered/CLHEP::deg<<std::endl;
			if(fCrossSection->Eval(q/CLHEP::GeV,phi_scattered/CLHEP::deg)<acc){

				G4cout<<"rejected:"<<s.str();
				continue;
			}
			else {
				G4cout<<"passed:"<<s.str();
				PrimaryEvent res;
				res.vx=pos.getX();
				res.vy=pos.getY();
				res.vz=pos.getZ();
				res.particles.push_back(PrimaryParticle(fParticles[0]->GetPDGEncoding(),pscattered_3.getX(),pscattered_3.getY(),pscattered_3.getZ()));
				res.particles.push_back(PrimaryParticle(fParticles[1]->GetPDGEncoding(),precoil_3.getX(),precoil_3.getY(),precoil_3.getZ()));
				return res;
			}
		}
		else{
		}
	}
}


elastic_scattering_model::elastic_scattering_model(){
	/*
	for(int ii=0;ii<30;ii++){
		G4cout<<"Theta="<<ii<<"deg Ay="<<Ay(270,ii)<<G4endl;
	}
	 */
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Double_t elastic_scattering_model::sigma(Double_t* x, Double_t* par) {
	return SigmaUnpol(par[0],x[0]);//*(1+par[1]*Ay(par[0],x[0])*cos(x[1]*DegToRad));
}

Double_t elastic_scattering_model::sigma0(Double_t* x, Double_t* par) {
	return SigmaUnpol(par[0],x[0]);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double elastic_scattering_model::SigmaUnpol(Double_t E,Double_t q1) {
	double w=log(E);
	return pow(10,a1(w)+a2(w)*q1+(1+a5(w)*q1)*(a3(w)*sin(a6(w)*q1)+a4(w)*cos(a6(w)*q1)));
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
