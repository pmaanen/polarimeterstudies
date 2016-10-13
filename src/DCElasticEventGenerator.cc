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

#include "DeuteronCarbonElasticScatteringModel.hh"
DCElasticEventGenerator::DCElasticEventGenerator(G4ParticleGun* pgun):PhaseSpaceGenerator(pgun,"dcelastic"){

	if(gConfig.count("generator.beam_polarization")){
		fBeamPolarization=gConfig["generator.beam_polarization"].as<double>()*CLHEP::deg;
	}
	else
		fBeamPolarization=Double_t(2./3.);

	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DCElasticEventGenerator::~DCElasticEventGenerator() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DCElasticEventGenerator::Initialize() {
	fParticles.clear();
	fParticles.push_back(G4Deuteron::DeuteronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	for(auto ipart=fParticles.begin();ipart!=fParticles.end();++ipart){
		if(!(*ipart))
			G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",FatalException,"beam particle not found.");
	}
	G4cout<<"Theta_Min="<<fThetaMin/CLHEP::deg<<" Theta_Max="<<fThetaMax/CLHEP::deg<<G4endl;
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
		fScatteringModel=std::unique_ptr<DeuteronCarbonElasticScatteringModel>(new DeuteronCarbonElasticScatteringModel(fBeamEnergy/CLHEP::MeV,fBeamPolarization));
	else{
		fScatteringModel->setBeamEnergy(fBeamEnergy/CLHEP::MeV);
		fScatteringModel->setBeamPolarization(fBeamPolarization);

	}
	fMaxY=2*fScatteringModel->SigmaUnpol(fBeamEnergy/CLHEP::MeV,sin(fThetaMin/2.)*2*fMomentumCMS/CLHEP::GeV);
	fInitialized=true;
}

void DCElasticEventGenerator::DefineCommands()
{
	fMessenger->DeclareMethod("polarization", &DCElasticEventGenerator::setBeamPolarization, "beam polarization");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
genevent_t DCElasticEventGenerator::Generate() {
	if(!fInitialized)
		Initialize();

	VertexGeneratorO::GetInstance()->setBeamposition(fBeamspot.getX()/CLHEP::mm,fBeamspot.getY()/CLHEP::mm,fBeamspot.getZ()/CLHEP::mm);
	VertexGeneratorO::GetInstance()->setBeamsize(fSpotsize.getX()/CLHEP::mm,fSpotsize.getY()/CLHEP::mm,fSpotsize.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamposition(0,0,fBeamspot.getZ()/CLHEP::mm);
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fSpotsize.getZ()/CLHEP::mm);

	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());

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
				genevent_t res(0,0,pos.getX(),pos.getY(),pos.getZ());
				res.particles.push_back(particle_t(fParticles[1]->GetPDGEncoding(),carbon_3.getX()/CLHEP::GeV,carbon_3.getY()/CLHEP::GeV,carbon_3.getZ()/CLHEP::GeV,carbon_4.Energy()*GeV-carbon_4.M()*GeV));
				res.particles.push_back(particle_t(fParticles[0]->GetPDGEncoding(),deuteron_3.getX()/CLHEP::GeV,deuteron_3.getY()/CLHEP::GeV,deuteron_3.getZ()/CLHEP::GeV,deuteron_4.Energy()*GeV-deuteron_4.M()*GeV));
				return res;
			}
		}
		else{
		}
	}
}
