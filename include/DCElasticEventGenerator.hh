/*
 * DCElasticEventGenerator.hh
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#ifndef DCELASTICEVENTGENERATOR_HH_
#define DCELASTICEVENTGENERATOR_HH_
#include "TMath.h"
#include "TF2.h"
#include "TGenPhaseSpace.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "globals.hh"
#include <TMath.h>

class G4ParticleDefinition;

class DCElasticEventGenerator {
public:
	DCElasticEventGenerator(G4double T=235*CLHEP::MeV);
	virtual ~DCElasticEventGenerator();

	G4double getBeamEnergy() const {return T_beam;}
	void setBeamEnergy(G4double beamEnergy) {T_beam = beamEnergy;Initialize();}
	//Initialize generates the phase space object and the tf2.
	//Because it has to be called after every change in energy, this is not in c'tor
	void Initialize();
	//Generates one Event.
	void GenerateEvent();

private:

	G4double T_beam,MaxY;
	TF2* SigmaFunc;


	TGenPhaseSpace ps;
	TLorentzVector cms;
	TLorentzVector beam;
	TLorentzVector target;
	Double_t momentum_cms;
	G4ParticleDefinition* beam_particle;
	G4ParticleDefinition* target_particle;



	//Returns a the TF2 for hit and miss.
	TF2* BuildFunction();
	//This is the Function to build the TF2. x[0] is theta_cm, x[1] is phi par[0] is beam energy.
	//Units: E:MeV Theta:deg q:GeV/c
	//Note: The root classes use GeV
	//Sigma=SigmaUnpol*(1-Ay*cos(phi))
	Double_t sigma(Double_t *x, Double_t *par);
	Double_t SigmaUnpol(Double_t, Double_t);

	//quick conversion for theta_cm to momentum transfer
	Double_t q(Double_t theta);

	//Parameters for ds/dOmega. x is ln(E)
	double a1(G4double x);
	double a2(G4double x);
	double a3(G4double x);
	double a4(G4double x);
	double a5(G4double x);
	double a6(G4double x);

	//Parameters for Ay
	double Ay(G4double E, G4double theta);
	double b1(G4double E);
	double b2(G4double E);
	double b3(G4double E);
	double b4(G4double E);
	double b5(G4double E);
	double b6(G4double E);
	double b7(G4double E);
	double b8(G4double E);
};

#endif /* DCELASTICEVENTGENERATOR_HH_ */
