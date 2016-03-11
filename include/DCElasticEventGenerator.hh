/*
 * DCElasticEventGenerator.hh
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#ifndef DCELASTICEVENTGENERATOR_HH_
#define DCELASTICEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
#include <G4ThreeVector.hh>
#include "TF2.h"
#include "TGenPhaseSpace.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "globals.hh"
#include <TMath.h>
#include <utility>
#include <vector>

#include <G4UnitsTable.hh>
class G4ParticleDefinition;
class G4GenericMessenger;
class elastic_scattering_model;
class DCElasticEventGenerator : public PhaseSpaceGenerator {
public:
	DCElasticEventGenerator(G4ParticleGun* pgun=0);
	virtual ~DCElasticEventGenerator();
	virtual void Initialize();
	virtual genevent_t Generate();
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization);	fInitialized=false;};

protected:

	Double_t fMomentumCMS,fBeamPolarization;
	elastic_scattering_model* fScatteringModel;
	virtual void DefineCommands();
	//Returns a the TF2 for hit and miss.
};


//This is the Function to build the TF2.
//x[0] is theta_cm, x[1] is phi par[0] is beam energy. par[1] is beam momentum in cms
//par[2] is polarization
//Units: E:MeV Theta:deg q:GeV/c
//Note: The root classes use GeV
//Sigma=SigmaUnpol*(1+Ay*Py*cos(phi))

class elastic_scattering_model{
public:
	elastic_scattering_model(Double_t,Double_t);
	Double_t sigma(TLorentzVector,TLorentzVector);
	Double_t SigmaUnpol(Double_t, Double_t);
	elastic_scattering_model() :
		fBeamEnergy(0), fBeamPolarization(0) {
	}
	void setBeamEnergy(Double_t beamEnergy) {
		fBeamEnergy = beamEnergy;
	}

	void setBeamPolarization(Double_t beamPolarization) {
		fBeamPolarization = beamPolarization;
	}

private:

	Double_t fBeamEnergy,fBeamPolarization;

	//Parameters for ds/dOmega. x is ln(E)
	double a1(Double_t x);
	double a2(Double_t x);
	double a3(Double_t x);
	double a4(Double_t x);
	double a5(Double_t x);
	double a6(Double_t x);

	//Parameters for Ay. Conversion from theta_cm to theta_lab is missing, but this does not affect us very much.
	double Ay(Double_t E, Double_t theta);
	double b1(Double_t E);
	double b2(Double_t E);
	double b3(Double_t E);
	double b4(Double_t E);
	double b5(Double_t E);
	double b6(Double_t E);
	double b7(Double_t E);
	double b8(Double_t E);
};
#endif /* DCELASTICEVENTGENERATOR_HH_ */
