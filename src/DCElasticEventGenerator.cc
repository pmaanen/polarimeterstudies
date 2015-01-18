/*
 * DCElasticEventGenerator.cc
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#include "DCElasticEventGenerator.hh"
#include "G4ParticleDefinition.hh"
#include <G4Deuteron.hh>
#include "G4IonTable.hh"
#include "G4ios.hh"
#include "TCanvas.h"
#include "TH1.h"
#include "Randomize.hh"
#include "Analysis.hh"
#include "G4GenericMessenger.hh"
using namespace CLHEP;
G4ThreadLocal DCElasticEventGenerator::MyFunction* DCElasticEventGenerator::func = 0;
G4ThreadLocal TF2* DCElasticEventGenerator::SigmaFunc = 0;
DCElasticEventGenerator::DCElasticEventGenerator(G4double T):T_beam(T) {
	beam_particle=G4Deuteron::DeuteronDefinition();
	target_particle=G4IonTable::GetIonTable()->FindIon(6,12);

	Double_t m_target = target_particle->GetPDGMass()/GeV;
	target.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Initialize();
	/*
	G4cout<<"DCElasticEventGenerator::DCElasticEventGenerator"<<G4endl;
	TCanvas c1("c1","c1",800,600);
	TF2* func=BuildFunction();
	for(int i=3;i<30;i++){
		G4cout<<"Theta="<<float(i)<<" Deg q="<<q(i)<<" GeV/c";
		for(int kk=0;kk<1;kk++){
			double phi=3.1415/2*kk;
			G4cout<<" sigma="<<func->Eval(float(i),phi)<<" Ay="<<Ay(T_beam/MeV,i);
		}
		G4cout<<G4endl;
		}

	func->CreateHistogram()->Draw();
	c1.Print("c1.root");
	 */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DCElasticEventGenerator::~DCElasticEventGenerator() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DCElasticEventGenerator::Initialize() {
	Double_t m_beam = beam_particle->GetPDGMass()/GeV;
	Double_t m_target = target_particle->GetPDGMass()/GeV;
	Double_t masses[2] = {m_beam, m_target} ;
	beam.SetPxPyPzE(0, 0, sqrt(T_beam/GeV*(T_beam/GeV+2*m_beam)), T_beam/CLHEP::GeV+m_beam);
	cms = beam + target;
	TLorentzVector temp=beam;
	temp.Boost(-cms.BoostVector());
	momentum_cms=temp.Vect().Mag();
	ps.SetDecay(cms, 2, masses);
	BuildFunction();
	MaxY=SigmaFunc->GetMaximum();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TF2* DCElasticEventGenerator::BuildFunction() {
	//if(SigmaFunc)
	//	delete SigmaFunc;
	//if(func)
	//	delete func;
	func=new MyFunction(momentum_cms,beam_polarization);
	SigmaFunc=new TF2("xsec",func,&DCElasticEventGenerator::MyFunction::sigma,3.,30.,0.,360.,1,"MyFunction","sigma");
	SigmaFunc->SetParName(0,"Energy");
	//Root uses GeV
	SigmaFunc->SetParameter(0,T_beam/CLHEP::GeV);
	return SigmaFunc;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ParticleMomentumVector DCElasticEventGenerator::GenerateEvent() {

	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		ps.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector pscattered_4 = *ps.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector precoil_4 = *ps.GetDecay(1) ;

		//spatial parts of generated L-vectors
		G4ThreeVector precoil_3(precoil_4.Vect().X()*GeV,precoil_4.Vect().Y()*GeV,precoil_4.Vect().Z()*GeV);
		G4ThreeVector pscattered_3(pscattered_4.Vect().X()*GeV,pscattered_4.Vect().Y()*GeV,pscattered_4.Vect().Z()*GeV);

		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = pscattered_3.getTheta();
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = pscattered_4.Vect().Phi()*TMath::RadToDeg()+180;//pscattered_3.getPhi();
		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered/deg>3 && th_scattered/deg<30){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = cms.BoostVector();     // in case beam simulation

			pscattered_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for deuteron in cm-frame
			Double_t CM_theta_scattered = pscattered_4.Theta();
			Double_t acc=MaxY*G4UniformRand();
			Analysis::Instance()->FillH1(4, phi_scattered);
			if(SigmaFunc->Eval(CM_theta_scattered*180/TMath::Pi(),Double_t(phi_scattered))<acc) continue;
			else {
				ParticleMomentumVector res;
				res.push_back(std::make_pair(beam_particle,pscattered_3));
				res.push_back(std::make_pair(target_particle,precoil_3));
				Analysis::Instance()->FillH1(1, th_scattered*180/TMath::Pi());
				Analysis::Instance()->FillH1(2, phi_scattered);
				G4cout<<Double_t(phi_scattered)<<G4endl;
				Analysis::Instance()->FillH1(3, th_scattered*TMath::RadToDeg(),1/TMath::Sin(th_scattered));
				return res;
			}
		}
	}
}


void DCElasticEventGenerator::DefineCommands() {
	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/DCGenerator/",
			"Generator control");

	G4GenericMessenger::Command& energyCmd
	= fMessenger->DeclareMethod("energy",
			&DCElasticEventGenerator::setBeamEnergy,
			"Set beam energy");
	energyCmd.SetParameterName("energy", true);
	energyCmd.SetRange("energy>=50. && mode<=270.");
	energyCmd.SetDefaultValue("235");

	G4GenericMessenger::Command& polarization
		= fMessenger->DeclareMethod("polarization",
				&DCElasticEventGenerator::setBeamPolarization,
				"Set beam polarization");
		polarization.SetParameterName("polarization", true);
		polarization.SetRange("energy>=-1. && mode<=1.");
		polarization.SetDefaultValue("0");



}


DCElasticEventGenerator::MyFunction::MyFunction(Double_t mom, Double_t pol):momentum_cms(mom),beam_polarization(pol){}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Double_t DCElasticEventGenerator::MyFunction::sigma(Double_t* x, Double_t* par) {

	return SigmaUnpol(par[0],x[0])*(1+beam_polarization*Ay(par[0],x[0])*TMath::Cos(x[1]*TMath::DegToRad()));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Double_t DCElasticEventGenerator::MyFunction::q(Double_t theta) {
	return 2*momentum_cms*TMath::Sin(theta/2*TMath::Pi()/180);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double DCElasticEventGenerator::MyFunction::SigmaUnpol(Double_t E,Double_t theta_cm) {
	double w=TMath::Log(E);
	double q1=q(theta_cm);
	return TMath::Power(10,a1(w)+a2(w)*q1+(1+a5(w)*q1)*(a3(w)*TMath::Sin(a6(w)*q1)+a4(w)*TMath::Cos(a6(w)*q1)));
}

double DCElasticEventGenerator::MyFunction::a1(Double_t x) {
	return -79.173+46.741*x-8.662*x*x+0.52986*x*x*x;
}

double DCElasticEventGenerator::MyFunction::a2(Double_t x) {
	return 78.051-34.263*x+3.341*x*x;

}

double DCElasticEventGenerator::MyFunction::a3(Double_t x) {
	return -38.231+19.774*x-3.1487*x*x+0.14791*x*x*x;
}

double DCElasticEventGenerator::MyFunction::a4(Double_t x) {
	return 9.1111-3.2832*x+0.28525*x*x;
}

double DCElasticEventGenerator::MyFunction::a5(Double_t x) {
	return -3.6126+0.18644*x;
}

double DCElasticEventGenerator::MyFunction::a6(Double_t x) {
	return 23.91+0.66962*x;
}

double DCElasticEventGenerator::MyFunction::Ay(Double_t E, Double_t theta) {
	return 1/(1+900/TMath::Power(theta,4))*
			(b4(E)-
					b1(E)/(1+TMath::Exp((theta-b2(E))/b3(E)))+
					b5(E)*(1-theta/b6(E))*sin(b7(E)+b8(E)*theta));
}

double DCElasticEventGenerator::MyFunction::b1(Double_t E) {
	return  1.1556+0.007182*(E-150)+1.3524e-5*TMath::Power(( E-150),2)-5.5448e-7*TMath::Power(( E-150),3);

}

double DCElasticEventGenerator::MyFunction::b2(Double_t E) {
	return 16.029-0.24658*(E-150)+8.6972e-4*TMath::Power((E-150),2);

}

double DCElasticEventGenerator::MyFunction::b3(Double_t E) {
	return 6.8319+0.052974*( E-150)+6.4864e-4*TMath::Power(E-150,2)-4.7648e-6*TMath::Power(E-150,3);

}

double DCElasticEventGenerator::MyFunction::b4(Double_t E) {
	return 0.94964+8.2885e-4*( E-150)-5.4014e-6*TMath::Power(E-150,2);

}

double DCElasticEventGenerator::MyFunction::b5(Double_t E) {
	return 0.21588+1.3908e-4*( E-150);

}

double DCElasticEventGenerator::MyFunction::b6(Double_t E) {
	return 42.467-0.25468*( E-150)+0.0033973*TMath::Power(E-150,2);

}

double DCElasticEventGenerator::MyFunction::b7(Double_t E) {
	return 58.573-0.41812*( E-150);

}

double DCElasticEventGenerator::MyFunction::b8(Double_t E) {
	return 25.698+0.091205*( E-150)-1.8594e-4*TMath::Power(E-150,2);

}
