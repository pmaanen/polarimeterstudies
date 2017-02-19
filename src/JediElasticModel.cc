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
#include "JediScatteringHelperFunctions.hh"
#include <G4NucleiProperties.hh>
#include <TROOT.h>
#include <TMath.h>
#include <TRandom3.h>
JediElasticModel::JediElasticModel():G4HadronicInteraction("dcelastic"),fBeamPolarization(0),fNucleus(nullptr),fNucleusMass(0) {
	ROOT::EnableThreadSafety();
	theMinEnergy=30*CLHEP::MeV;
	SetRecoilEnergyThreshold(10*CLHEP::keV);
	fIncidentParticle=G4Deuteron::DeuteronDefinition();

	fQmin=0.04*CLHEP::GeV;
	fQmax=.4*CLHEP::GeV;

	TRandom3 gen(G4Threading::G4GetThreadId());
	fQ=new TF1("q",JediScatteringHelperFunctions::elastic::q,fQmin/CLHEP::GeV,fQmax/CLHEP::GeV,1);

	fPhi=new TF1("Phi",JediScatteringHelperFunctions::elastic::phi,0,2*TMath::Pi(),3);
	fThetaMax=13*CLHEP::deg;

	fQ->SetNpx(1000);
	fPhi->SetNpx(1000);

	//G4cout<<"JediElasticModel::JediElasticModel() thread id: "<<G4Threading::G4GetThreadId()<<G4endl;
	if(!fIncidentParticle)
		G4Exception("JediElasticModel::JediElasticModel","",FatalException,"no deuteron definition found");
	fIncidentParticleMass=fIncidentParticle->GetPDGMass();
	fOutParticles.push_back(fIncidentParticle);
	verboseLevel=gVerbose;
	DefineCommands();
}

G4HadFinalState* JediElasticModel::ApplyYourself(const G4HadProjectile& aTrack,
		G4Nucleus& targetNucleus) {
	theParticleChange.Clear();

	const G4HadProjectile* aParticle = &aTrack;
	G4double ekin = aParticle->GetKineticEnergy();
	if(ekin <= theMinEnergy) {
		theParticleChange.SetEnergyChange(ekin);
		theParticleChange.SetMomentumChange(aTrack.Get4Momentum().vect().unit());
		return &theParticleChange;
	}
	G4int A = targetNucleus.GetA_asInt();
	G4int Z = targetNucleus.GetZ_asInt();

	G4double plab = aParticle->GetTotalMomentum();

	// Scattered particle referred to axis of incident particle
	const G4ParticleDefinition* theParticle = aParticle->GetDefinition();
	G4double m1 = theParticle->GetPDGMass();

	if (verboseLevel>1) {
		G4cout << "JediElasticProcess: "
				<< aParticle->GetDefinition()->GetParticleName()
				<< " Plab(GeV/c)= " << plab/CLHEP::GeV
				<< " Ekin(MeV) = " << ekin/CLHEP::MeV
				<< " scattered off Z= " << Z
				<< " A= " << A
				<< G4endl;
	}

	G4double mass2 = G4NucleiProperties::GetNuclearMass(A, Z);
	G4LorentzVector lv1 = aParticle->Get4Momentum();
	G4LorentzVector lv(0.0,0.0,0.0,mass2);
	lv += lv1;

	G4ThreeVector bst = lv.boostVector();
	lv1.boost(-bst);

	G4ThreeVector p1 = lv1.vect();
	G4double momentumCMS = p1.mag();


	G4double thetaLab=0;
	do{
		theParticleChange.Clear();
		// Sampling in CM system
		G4double q    = SampleQ(ekin);
		G4double theta = 2*TMath::ASin(q/2/momentumCMS)*CLHEP::rad;
		G4double phi  = 0.;
		// problem in sampling
		if( q<fQmin || q> fQmax ) {
			//if(verboseLevel > 0) {
			G4cout << "JediElasticModel WARNING q= " << q
					<< " after scattering of "
					<< aParticle->GetDefinition()->GetParticleName()
					<< " p(GeV/c)= " << plab/CLHEP::GeV
					<< " on an ion Z= " << Z << " A= " << A
					<< G4endl;
			//}
			theta=0;
			// normal situation
		}
		if (verboseLevel>1) {
			G4cout << " q(GeV)= " << q/CLHEP::GeV
					<< " Pcms(GeV)= " << momentumCMS/CLHEP::GeV
					<< " theta=" << theta/CLHEP::deg << G4endl;
		}
		//v1=3-Momentum in CMS, nlv1=4-Mom ins cms
		G4ThreeVector v1(std::sin(theta)*std::cos(phi),std::sin(theta)*std::sin(phi),std::cos(theta));
		v1 *= momentumCMS;
		G4LorentzVector nlv1(v1.x(),v1.y(),v1.z(),
				std::sqrt(momentumCMS*momentumCMS + m1*m1));


		//boost 4-mom to lab system
		nlv1.boost(bst);


		G4double eFinal = nlv1.e() - m1;
		if (verboseLevel > 1) {
			G4cout <<" m= " << m1 << " Efin(MeV)= " << eFinal
					<< " Proj: 4-mom " << lv1 << " Final: " << nlv1
					<< G4endl;
		}

		if(eFinal <= theMinEnergy) {
			if(eFinal < 0.0 && verboseLevel > 0) {
				G4cout << "JediElastic WARNING Efinal= " << eFinal
						<< " after scattering of "
						<< aParticle->GetDefinition()->GetParticleName()
						<< " p(GeV/c)= " << plab/CLHEP::GeV
						<< " on an ion Z= " << Z << " A= " << A
						<< G4endl;
			}
			theParticleChange.SetEnergyChange(0.0);
			nlv1 = G4LorentzVector(0.0,0.0,0.0,m1);

		} else {
			theParticleChange.SetMomentumChange(nlv1.vect().unit());
			theParticleChange.SetEnergyChange(eFinal);
		}

		lv -= nlv1;
		G4double erec =  lv.e() - mass2;
		if (verboseLevel > 1) {
			G4cout << "Recoil: " <<" m= " << mass2 << " Erec(MeV)= " << erec
					<< " 4-mom: " << lv
					<< G4endl;
		}

		if(erec > 10*CLHEP::keV) {
			G4ParticleDefinition * theDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z,A,0.0);
			G4DynamicParticle * aSec = new G4DynamicParticle(theDef, lv);
			theParticleChange.AddSecondary(aSec);
		} else if(erec > 0.0) {
			theParticleChange.SetLocalEnergyDeposit(erec);
		}
		thetaLab=nlv1.vect().getTheta();
	} while(thetaLab<fThetaMin or thetaLab>fThetaMax);
	return &theParticleChange;

}

void JediElasticModel::DefineCommands() {
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,"/jedi/elastic/",""));
	fMessenger->DeclarePropertyWithUnit("theta_min","rad",JediElasticModel::fThetaMin,"");
	fMessenger->DeclarePropertyWithUnit("theta_max","rad",JediElasticModel::fThetaMax,"");
}

G4double JediElasticModel::SampleQ(G4double kinEnergy) {

	/*
	auto ymax=JediScatteringHelperFunctions::elastic::q(fQmin,kinEnergy/CLHEP::MeV);
	G4double xrand,yrand;
	while(true){

		xrand=(fQmax/CLHEP::GeV-fQmin/CLHEP::GeV)*G4UniformRand()+fQmin/CLHEP::GeV;
		yrand=G4UniformRand()*ymax;
		//G4cout<<"ymax" <<ymax<<" xrand "<<xrand<<" yrand "<<yrand<<" y(xrand) "<<JediScatteringHelperFunctions::elastic::q(xrand,kinEnergy/CLHEP::MeV)<<G4endl;
		if(yrand<JediScatteringHelperFunctions::elastic::q(xrand,kinEnergy/CLHEP::MeV))
			break;
	}

	auto q=xrand*CLHEP::GeV;

	 */
	fQ->SetParameter(0,kinEnergy/CLHEP::MeV);
	auto q=fQ->GetRandom(fQmin/CLHEP::GeV,fQmax/CLHEP::GeV)*CLHEP::GeV;
	return q;
}

G4double JediElasticModel::SamplePhi(G4double kinEnergy, G4double pol, G4double thetaLab) {
	//G4cout<<"Setting parameters to "<<kinEnergy/CLHEP::MeV<<" "<<pol<<" "<<thetaLab/CLHEP::deg<<G4endl;
	fPhi->SetParameters(kinEnergy/CLHEP::MeV,pol,thetaLab/CLHEP::deg);
	G4double phi=fPhi->GetRandom();
	phi*=CLHEP::rad;
	return phi;
}
