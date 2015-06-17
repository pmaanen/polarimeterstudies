/*
 * DCBreakupEventGenerator.cc
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#include <DCBreakupEventGenerator.hh>
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "TMath.h"
#include "G4ParticleGun.hh"

static double DegToRad=3.14159265359/180.;
static double RadToDeg=1/DegToRad;

DCBreakupEventGenerator::DCBreakupEventGenerator(G4ParticleGun* gun):PhaseSpaceGenerator(gun),cross_section(0) {
	beamEnergy=235.*CLHEP::MeV;
	Initialized=false;

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;
	DefineCommands();
}

DCBreakupEventGenerator::~DCBreakupEventGenerator() {
	// TODO Auto-generated destructor stub
}

TF2* DCBreakupEventGenerator::BuildFunction() {
	scattering_model=new deuteron_breakup_model;
	cross_section=new TF2("xsec",scattering_model,&deuteron_breakup_model::sigma,3.,30.,0.,360.,3,"MyFunction","sigma");
	cross_section->SetParName(0,"Energy");
	cross_section->SetParName(1,"Momentum");
	cross_section->SetParName(2,"Polarization");
	return cross_section;
}

void DCBreakupEventGenerator::Initialize() {
	particles.push_back(G4Proton::ProtonDefinition());
	particles.push_back(G4Neutron::NeutronDefinition());
	particles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	for(auto ipart=particles.begin();ipart!=particles.end();++ipart){
		if(!(*ipart))
			throw;//G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",0,"beam particle not found.");
	}
	Double_t m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;
	Double_t m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	target.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[3] = {particles[0]->GetPDGMass()/CLHEP::GeV,particles[1]->GetPDGMass()/CLHEP::GeV,particles[1]->GetPDGMass()/CLHEP::GeV} ;
	beam.SetPxPyPzE(0, 0, sqrt(beamEnergy/CLHEP::GeV*(beamEnergy/CLHEP::GeV+2*m_beam)), beamEnergy/CLHEP::GeV+m_beam);
	cms = beam + target;
	ps.SetDecay(cms, 3, masses);
	if(!cross_section)
		BuildFunction();
	TLorentzVector temp=beam;
	temp.Boost(-cms.BoostVector());
	momentum_cms=temp.Vect().Mag();
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
	an->FinishNtuple(myTupleId[0]);
	Initialized=true;

	Initialized=true;
}

void DCBreakupEventGenerator::Generate(G4Event* E) {
	auto event=PrimaryEvent(Generate());
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		//TODO Write Truth
		pGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		pGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		pGun->GeneratePrimaryVertex(E);
	}
	return;
}

PrimaryEvent DCBreakupEventGenerator::Generate() {

	if(!Initialized)
		Initialize();
	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		ps.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector proton_4 = *ps.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector neutron_4 = *ps.GetDecay(1) ;

		//L-vector of recoil particle in lab-frame
		TLorentzVector carbon_4 = *ps.GetDecay(2) ;

		//spatial parts of generated L-vectors
		G4ThreeVector proton_3(proton_4.Vect().X()*CLHEP::GeV,proton_4.Vect().Y()*CLHEP::GeV,proton_4.Vect().Z()*CLHEP::GeV);
		G4ThreeVector neutron_3(neutron_4.Vect().X()*CLHEP::GeV,neutron_4.Vect().Y()*CLHEP::GeV,neutron_4.Vect().Z()*CLHEP::GeV);
		G4ThreeVector carbon_3(carbon_4.Vect().X()*CLHEP::GeV,carbon_4.Vect().Y()*CLHEP::GeV,carbon_4.Vect().Z()*CLHEP::GeV);
		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = proton_3.getTheta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = proton_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered>thetaMin and th_scattered<thetaMax){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = cms.BoostVector();     // in case beam simulation

			proton_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for deuteron in cm-frame
			G4double CM_theta_scattered = proton_4.Theta()*CLHEP::rad;
			G4double acc=MaxY*G4UniformRand();
			if(cross_section->Eval(CM_theta_scattered/CLHEP::deg,phi_scattered/CLHEP::deg,0)<acc) continue;
			PrimaryEvent res;


			PrimaryParticle a(2212,1.,1.,1.);
			res.particles.push_back(PrimaryParticle(2212,proton_3.getX(),proton_3.getY(),proton_3.getZ()));
			res.particles.push_back(PrimaryParticle(2112,neutron_3.getX(),neutron_3.getY(),neutron_3.getZ()));
			res.particles.push_back(PrimaryParticle(1000060120,carbon_3.getX(),carbon_3.getY(),carbon_3.getZ()));
			//Analysis::Instance()->FillH1(1, th_scattered/CLHEP::deg);
			//Analysis::Instance()->FillH1(2, phi_scattered/CLHEP::deg);
			//Analysis::Instance()->FillH1(3, th_scattered/CLHEP::deg,1/sin(th_scattered));
			return res;
		}
	}
}
deuteron_breakup_model::deuteron_breakup_model(){}
Double_t deuteron_breakup_model::sigma(Double_t* x, Double_t* par) {
	return SigmaUnpol(par[0],x[0],par[1])*(1+par[2]*Ay(par[0],x[2],0)*cos(x[1]*DegToRad));
}

Double_t deuteron_breakup_model::SigmaUnpol(Double_t E, Double_t theta,
		Double_t Ex) {
	if(Ex>(c2(theta,E)-c3(theta)*c4(theta)))
		return c1(theta,E)*exp(-pow(Ex-c2(theta,E),2)/(2*c3(theta)*c3(theta)));
	else
		return c1(theta,E)*exp(-c4(theta)*c4(theta)/2.)*exp(-(c5(theta)*(c2(theta,E-c4(theta)*c3(theta)-Ex)))/c3(theta));

}

Double_t deuteron_breakup_model::c1(Double_t theta,Double_t E) {
	return sqrt(113./E)*exp(1.5586-0.07178*(theta-40)+4.0733e-4*pow(theta-40,2));
}

Double_t deuteron_breakup_model::c2(Double_t theta, Double_t E) {
	return sqrt(113./E)*(62.214+0.30061*(theta-40)-0.004704*pow(theta-40,2));
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
	fMessenger=new G4GenericMessenger(this, "/PolarimeterStudies/dCbreakup/", "breakup event generator control");
	G4GenericMessenger::Command& polCmd
	= fMessenger->DeclareMethod("polarization", &DCBreakupEventGenerator::setBeamPolarization, "beam polarization");

	G4GenericMessenger::Command& eneCmd
	= fMessenger->DeclareMethod("energy", &DCBreakupEventGenerator::setBeamEnergy, "beam energy");
}
