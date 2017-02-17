/*
 * DeuteronCarbonElasticScatteringModel.cc
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#include <JediElasticModel.hh>
#include "global.hh"
#include <G4Deuteron.hh>
#include "G4ParticleDefinition.hh"
#include <G4Event.hh>
#include "G4IonTable.hh"
#include "G4ios.hh"
#include "TVector3.h"
#include <math.h>
#include <TMath.h>
JediElasticModel::JediElasticModel():G4HadronicInteraction("dcelastic"),fBeamPolarization(0),fNucleus(nullptr),fNucleusMass(0) {
	fIncidentParticle=G4Deuteron::DeuteronDefinition();
	fQ=std::unique_ptr<TF1>(new TF1("q",this,&JediElasticModel::q,0,TMath::Pi(),1,"DeuteronCarbonElasticScatteringModel","q"));
	fPhi=std::unique_ptr<TF1>(new TF1("Phi",this,&JediElasticModel::Phi,0,2*TMath::Pi(),3,"DeuteronCarbonElasticScatteringModel","Phi"));
	fThetaMin=3*CLHEP::deg;
	fThetaMax=30*CLHEP::deg;


	//G4cout<<"JediElasticModel::JediElasticModel() thread id: "<<G4Threading::G4GetThreadId()<<G4endl;
	if(!fIncidentParticle)
		G4Exception("JediElasticModel::JediElasticModel","",FatalException,"no deuteron definition found");
	fIncidentParticleMass=fIncidentParticle->GetPDGMass();


	fOutParticles.push_back(fIncidentParticle);


	fQmin=0.04;
	fQmax=4;

	DefineCommands();
}

G4HadFinalState* JediElasticModel::ApplyYourself(const G4HadProjectile& aTrack,
		G4Nucleus&) {
	theParticleChange.Clear();


	if(gVerbose>2)
		G4cout<<"JediElasticModel::ApplyYourself"<<G4endl;
	//G4Exception("JediElasticModel::ApplyYourself","",FatalException,"");
	/*
	fMomentumCMS=

			fQmin=2*fMomentumCMS*sin(fThetaMin/2);
	fQmax=2*fMomentumCMS*sin(fThetaMax/2);
	 */

	if(!fNucleus){
		fNucleus=G4IonTable::GetIonTable()->GetIon(6,12);
		if(!fNucleus)
			G4Exception("JediElasticModel::JediElasticModel","",FatalException,"no nucleus definition found");
		fNucleusMass=fNucleus->GetPDGMass();
		fOutParticles.push_back(fNucleus);

	}

	G4double           kinEnergy( aTrack.GetKineticEnergy() );
	G4HadProjectile &  theProjectile( const_cast< G4HadProjectile & >(
			aTrack ) );
	const G4LorentzRotation &  projToLab(
			const_cast< const G4LorentzRotation & >(
					theProjectile.GetTrafoToLab() ) );
	G4LorentzVector incidentParticleMomentum = aTrack.Get4Momentum();

	fIncidentParticleMomentumLAB=incidentParticleMomentum;
	fIncidentParticleMomentumLAB.transform( projToLab );
	fNucleusMomentumLAB.setPx( 0 );
	fNucleusMomentumLAB.setPy( 0 );
	fNucleusMomentumLAB.setPz( 0 );
	fNucleusMomentumLAB.setE( fNucleusMass );

	/*
	if ( fermiMotionIsOn )
	{
		G4ThreeVector  targetNucleusMomentum(
				targetNucleus.GetFermiMomentum() );
		G4double       targetNucleusEnergy(
				std::sqrt( targetNucleusMomentum.mag2() +
						nucleusParticleMass * nucleusParticleMass ) );
		productionModelData.nucleusParticleLAB = G4LorentzVector(
				targetNucleusMomentum, targetNucleusEnergy );
	}
	 */

	fNucleusMomentumLAB.transform( projToLab );
	G4LorentzVector inVecSum= fIncidentParticleMomentumLAB + fNucleusMomentumLAB ;
	G4ThreeVector    boostVec( inVecSum.boostVector() );

	fIncidentParticleMomentumCMS = fIncidentParticleMomentumLAB;
	fNucleusMomentumCMS = fNucleusMomentumLAB;

	fIncidentParticleMomentumCMS.boost( -boostVec );
	fNucleusMomentumCMS.boost( -boostVec );
	fQ->SetParameter(0,kinEnergy/CLHEP::MeV);

	auto fMomentumCMS=fIncidentParticleMomentumCMS.vect().mag();
	G4LorentzVector outDeuteronMomentum;
	do{

	auto q=fQ->GetRandom(fQmin,fQmax)*CLHEP::GeV;
	G4LorentzVector outDeuteronMomentumCMS(fMomentumCMS,0,0,sqrt(fMomentumCMS*fMomentumCMS+fIncidentParticleMass*fIncidentParticleMass));
	auto thetaCMS=2*TMath::ASin(q/2/fMomentumCMS);
	outDeuteronMomentumCMS.setTheta(thetaCMS);
	auto outDeuteronMomentumLAB=outDeuteronMomentumCMS;
	outDeuteronMomentumLAB.boost(boostVec);
	auto thetaLab=outDeuteronMomentumLAB.theta();
	fPhi->SetParameter(0,kinEnergy/CLHEP::MeV);
	fPhi->SetParameter(1,fBeamPolarization);
	fPhi->SetParameter(2,thetaLab);
	auto phi=fPhi->GetRandom(0,2*CLHEP::pi);

	outDeuteronMomentumLAB.setPhi(phi);
	outDeuteronMomentum=outDeuteronMomentumLAB;

	auto outNucleusMomentum=G4LorentzVector(-outDeuteronMomentumCMS.vect(),
			sqrt(fMomentumCMS*fMomentumCMS+fNucleusMass*fNucleusMass));

	outNucleusMomentum.boost(boostVec);
	outNucleusMomentum.transform(projToLab.inverse());

	G4DynamicParticle  outDeuteron(fIncidentParticle,outDeuteronMomentum);

	theParticleChange.SetMomentumChange(outDeuteron.GetMomentumDirection());
	theParticleChange.SetEnergyChange(outDeuteron.GetKineticEnergy());

	G4DynamicParticle *  outNucleus( new G4DynamicParticle(
			fNucleus,
			outNucleusMomentum
	) );
	theParticleChange.AddSecondary( outNucleus );

	} while(outDeuteronMomentum.vect().getTheta()>=fThetaMax || outDeuteronMomentum.vect().getTheta()<=fThetaMin);

	return &theParticleChange;
}

void JediElasticModel::DefineCommands() {
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,"/jedi/elastic/",""));
	fMessenger->DeclarePropertyWithUnit("theta_min","rad",JediElasticModel::fThetaMin,"");
	fMessenger->DeclarePropertyWithUnit("theta_max","rad",JediElasticModel::fThetaMax,"");
}


double JediElasticModel::q(Double_t* x, Double_t* par) {
	double w=log(par[0]);
	return pow(10,a1(w)+a2(w)*x[0]+(1+a5(w)*x[0])*(a3(w)*sin(a6(w)*x[0])+a4(w)*cos(a6(w)*x[0])));
}
double JediElasticModel::Phi(Double_t* x, Double_t* par) {
	return 1+par[1]*Ay(par[0],par[2]*180./TMath::Pi())*cos(x[0]);
}

double JediElasticModel::a1(Double_t x) {
	return -79.173+46.741*x-8.662*x*x+0.52986*x*x*x;
}

double JediElasticModel::a2(Double_t x) {
	return 78.051-34.263*x+3.341*x*x;

}

double JediElasticModel::a3(Double_t x) {
	return -38.231+19.774*x-3.1487*x*x+0.14791*x*x*x;
}

double JediElasticModel::a4(Double_t x) {
	return 9.1111-3.2832*x+0.28525*x*x;
}

double JediElasticModel::a5(Double_t x) {
	return -3.6126+0.18644*x;
}

double JediElasticModel::a6(Double_t x) {
	return 23.91+0.66962*x;
}

double JediElasticModel::Ay(Double_t E, Double_t theta) {
	return (1/(1+900./pow(theta,4)))*
			(b4(E)-b1(E)/(1+exp((theta-b2(E))/b3(E)))+b5(E)*(1-theta/b6(E))*sin(b7(E)+b8(E)*theta));
}

double JediElasticModel::b1(Double_t E) {
	return  1.1556+0.007182*(E-150)+1.3524e-5*pow(( E-150),2)-5.5448e-7*pow(( E-150),3);

}

double JediElasticModel::b2(Double_t E) {
	return 16.029-0.24658*(E-150)+8.6972e-4*pow((E-150),2);

}

double JediElasticModel::b3(Double_t E) {
	return 6.8319+0.052974*( E-150)+6.4864e-4*pow(E-150,2)-4.7648e-6*pow(E-150,3);

}

double JediElasticModel::b4(Double_t E) {
	return 0.94964+8.2885e-4*( E-150)-5.4014e-6*pow(E-150,2);

}

double JediElasticModel::b5(Double_t E) {
	return 0.21588+1.3908e-4*( E-150);

}

double JediElasticModel::b6(Double_t E) {
	return 42.467-0.25468*( E-150)+0.0033973*pow(E-150,2);

}

double JediElasticModel::b7(Double_t E) {
	return 58.573-0.41812*( E-150);

}

double JediElasticModel::b8(Double_t E) {
	return 25.698+0.091205*( E-150)-1.8594e-4*pow(E-150,2);

}
