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
//static double RadToDeg=1/DegToRad;

DCBreakupEventGenerator::DCBreakupEventGenerator(G4ParticleGun* gun):PhaseSpaceGenerator(gun),fCrossSection(0) {
	fBeamEnergy=235.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=5*CLHEP::deg;
	fThetaMax=20*CLHEP::deg;
	DefineCommands();
}

DCBreakupEventGenerator::~DCBreakupEventGenerator() {
	// TODO Auto-generated destructor stub
}

TF2* DCBreakupEventGenerator::BuildFunction() {
	fScattering_model=new deuteron_breakup_model;
	fCrossSection=new TF2("xsec",fScattering_model,&deuteron_breakup_model::sigma,3.,30.,0.,360.,3,"MyFunction","sigma");
	fCrossSection->SetParName(0,"Energy");
	fCrossSection->SetParName(1,"Momentum");
	fCrossSection->SetParName(2,"Polarization");
	return fCrossSection;
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
	fPhaseSpace.SetDecay(fCms, 3, masses); //first decay into p+13C
	if(!fCrossSection)
		BuildFunction();
	TLorentzVector temp=fBeam;
	temp.Boost(-fCms.BoostVector());
	fMomentum_cms=temp.Vect().Mag();
	if(!fCrossSection)
		BuildFunction();
	fCrossSection->SetParameter(0,fBeamEnergy/CLHEP::MeV);
	fCrossSection->SetParameter(1,fBeamPolarization);
	fMaxY=fCrossSection->GetMaximum();

	Analysis* an=Analysis::Instance();
	fTupleId.push_back(an->CreateNtuple("MCTruth","MCTruth"));
	fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"event"));
	fTupleId.push_back(an->CreateNtupleIColumn(fTupleId[0],"pid"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"px"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"py"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"pz"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"vx"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"vy"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"vz"));
	fTupleId.push_back(an->CreateNtupleFColumn(fTupleId[0],"Ex"));
	an->FinishNtuple(fTupleId[0]);
	fInitialized=true;
}

void DCBreakupEventGenerator::Generate(G4Event* E) {
	auto event=PrimaryEvent(Generate());
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		fParticleGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		fParticleGun->GeneratePrimaryVertex(E);
		Analysis* an=Analysis::Instance();
		an->FillNtupleIColumn(fTupleId[0],fTupleId[1],E->GetEventID());
		an->FillNtupleIColumn(fTupleId[0],fTupleId[2],iPart->id);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[3],iPart->px);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[4],iPart->py);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[5],iPart->pz);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[6],fParticleGun->GetParticlePosition().getX()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[7],fParticleGun->GetParticlePosition().getY()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[8],fParticleGun->GetParticlePosition().getZ()/CLHEP::mm);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[9],fParticleGun->GetParticleTime()/CLHEP::s);
		an->FillNtupleFColumn(fTupleId[0],fTupleId[10],this->fBeamPolarization);
		an->AddNtupleRow(fTupleId[0]);
	}
	return;
}

PrimaryEvent DCBreakupEventGenerator::Generate() {

	if(!fInitialized)
		Initialize();
	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		fPhaseSpace.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector proton_4 = *fPhaseSpace.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector neutron_4 = *fPhaseSpace.GetDecay(1) ;

		//L-vector of recoil particle in lab-frame
		TLorentzVector carbon_4 = *fPhaseSpace.GetDecay(2) ;

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
		if(th_scattered>fThetaMin and th_scattered<fThetaMax){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = fCms.BoostVector();     // in case beam simulation

			proton_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for proton in cm-frame

			G4double Ex=(fBeam.Energy()+fTarget.Energy()-proton_4.Energy()-carbon_4.Energy()-neutron_4.Energy())*CLHEP::GeV-2.22*CLHEP::MeV;
			if(Ex<0)
				continue;
			G4double acc=fMaxY*G4UniformRand();

			if(fCrossSection->Eval(th_scattered/CLHEP::deg,phi_scattered/CLHEP::deg,Ex)<acc) continue;
			PrimaryEvent res;
			res.particles.push_back(PrimaryParticle(fParticles[0]->GetPDGEncoding(),proton_3.getX(),proton_3.getY(),proton_3.getZ()));
			res.particles.push_back(PrimaryParticle(fParticles[1]->GetPDGEncoding(),neutron_3.getX(),neutron_3.getY(),neutron_3.getZ()));
			res.particles.push_back(PrimaryParticle(fParticles[2]->GetPDGEncoding(),carbon_3.getX(),carbon_3.getY(),carbon_3.getZ()));
			return res;
		}
	}
}
deuteron_breakup_model::deuteron_breakup_model(){}
Double_t deuteron_breakup_model::sigma(Double_t* x, Double_t* par) {
	return SigmaUnpol(par[0],x[0],x[2])*(1+par[1]*Ay(par[0],x[0],x[2])*cos(x[1]*DegToRad));
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

	fMessenger->DeclareMethod("polarization", &DCBreakupEventGenerator::setBeamPolarization, "beam polarization");
	fMessenger->DeclareMethod("energy", &DCBreakupEventGenerator::setBeamEnergy, "beam energy");
}
