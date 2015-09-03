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
static double RadToDeg=1/DegToRad;
DCElasticEventGenerator::DCElasticEventGenerator(G4ParticleGun* pgun):PhaseSpaceGenerator(pgun),cross_section(0),tiltx(0),tilty(0),xprime(0),yprime(0),beamspot(0,0,0),spotsize(0,0,0){
	beamEnergy=235.*CLHEP::MeV;
	beamPolarization=Double_t(2./3.);
	Initialized=false;

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;
	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DCElasticEventGenerator::~DCElasticEventGenerator() {
	if(cross_section)
		delete cross_section;
	cross_section=0;
	if(scattering_model)
		delete scattering_model;
	scattering_model=0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DCElasticEventGenerator::Initialize() {
	particles.clear();
	particles.push_back(G4Deuteron::DeuteronDefinition());
	particles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	for(auto ipart=particles.begin();ipart!=particles.end();++ipart){
		if(!(*ipart))
			G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",FatalException,"beam particle not found.");
	}
	Double_t m_target = particles[1]->GetPDGMass()/GeV;
	Double_t m_beam = particles[0]->GetPDGMass()/GeV;
	target.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[2] = {m_beam, m_target} ;
	beam.SetPxPyPzE(0, 0, sqrt(beamEnergy/CLHEP::GeV*(beamEnergy/CLHEP::GeV+2*m_beam)), beamEnergy/CLHEP::GeV+m_beam);
	cms = beam + target;
	TLorentzVector temp=beam;
	temp.Boost(-cms.BoostVector());
	momentum_cms=temp.Vect().Mag();
	ps.SetDecay(cms, 2, masses);
	if(!cross_section)
		BuildFunction();
	cross_section->SetParameter(0,beamEnergy/CLHEP::MeV);
	cross_section->SetParameter(1,momentum_cms);
	cross_section->SetParameter(2,beamPolarization);
	MaxY=cross_section->GetMaximum();
	Analysis* an=Analysis::Instance();
	myTupleId.push_back(an->CreateNtuple("MCTruth","MCTruth"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"event"));
	myTupleId.push_back(an->CreateNtupleIColumn(myTupleId[0],"pid"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"px"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"py"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"pz"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vx"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vy"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"vz"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"t"));
	myTupleId.push_back(an->CreateNtupleFColumn(myTupleId[0],"pol"));
	an->FinishNtuple(myTupleId[0]);

	VertexGeneratorO::GetInstance()->setBeamposition(0,0,0);
	VertexGeneratorO::GetInstance()->setBeamsize(1*CLHEP::mm,1*CLHEP::mm,1*CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,5*CLHEP::mm);
	Initialized=true;
}

void DCElasticEventGenerator::DefineCommands()
{
	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dCelastic/", "elastic event generator control");
	G4GenericMessenger::Command& polCmd
	= fMessenger->DeclareMethod("polarization", &DCElasticEventGenerator::setBeamPolarization, "beam polarization");

	G4GenericMessenger::Command& eneCmd
	= fMessenger->DeclareMethod("energy", &DCElasticEventGenerator::setBeamEnergy, "beam energy");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TF2* DCElasticEventGenerator::BuildFunction() {
	scattering_model=new elastic_scattering_model;
	cross_section=new TF2("xsec",scattering_model,&elastic_scattering_model::sigma,3.,30.,0.,360.,3,"MyFunction","sigma");
	cross_section->SetParName(0,"Energy");
	cross_section->SetParName(1,"Momentum");
	cross_section->SetParName(2,"Polarization");
	return cross_section;
}

void DCElasticEventGenerator::Generate(G4Event* E) {


	auto event=PrimaryEvent(Generate());
	pGun->SetParticlePosition(G4ThreeVector(event.vx,event.vy,event.vz));
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		pGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		pGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		pGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
		an->FillNtupleIColumn(myTupleId[0],myTupleId[1],E->GetEventID());
		an->FillNtupleIColumn(myTupleId[0],myTupleId[2],iPart->id);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[3],iPart->px);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[4],iPart->py);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[5],iPart->pz);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[6],pGun->GetParticlePosition().getX()/CLHEP::mm);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[7],pGun->GetParticlePosition().getY()/CLHEP::mm);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[8],pGun->GetParticlePosition().getZ()/CLHEP::mm);
		an->FillNtupleFColumn(myTupleId[0],myTupleId[9],pGun->GetParticleTime()/CLHEP::s);
		an->AddNtupleRow(myTupleId[0]);
	}
	return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PrimaryEvent DCElasticEventGenerator::Generate() {
	if(!Initialized)
		Initialize();
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	while (1) {
		beam.RotateX(tiltx+G4RandGauss::shoot(tiltx,xprime));
		beam.RotateY(tilty+G4RandGauss::shoot(tilty,yprime));

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
		G4double th_scattered  = pscattered_3.getTheta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = pscattered_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered>thetaMin and th_scattered<thetaMax){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = cms.BoostVector();     // in case beam simulation

			pscattered_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for deuteron in cm-frame
			G4double CM_theta_scattered = pscattered_4.Theta()*CLHEP::rad;
			G4double acc=MaxY*G4UniformRand();
			if(cross_section->Eval(CM_theta_scattered/CLHEP::deg,phi_scattered/CLHEP::deg)<acc){
				continue;
			}
			else {
				PrimaryEvent res;
				res.vx=pos.getX();
				res.vy=pos.getY();
				res.vz=pos.getZ();
				res.particles.push_back(PrimaryParticle(particles[0]->GetPDGEncoding(),pscattered_3.getX(),pscattered_3.getY(),pscattered_3.getZ()));
				res.particles.push_back(PrimaryParticle(particles[1]->GetPDGEncoding(),precoil_3.getX(),precoil_3.getY(),precoil_3.getZ()));
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
	return SigmaUnpol(par[0],x[0],par[1])*(1+par[2]*Ay(par[0],x[0])*cos(x[1]*DegToRad));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Double_t elastic_scattering_model::q(Double_t theta, Double_t mom) {
	return 2*mom*sin(theta/2*DegToRad);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double elastic_scattering_model::SigmaUnpol(Double_t E,Double_t theta_cm, Double_t mom) {
	double w=log(E);
	double q1=q(theta_cm, mom);
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
