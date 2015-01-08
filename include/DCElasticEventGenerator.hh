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
	DCElasticEventGenerator(G4double T=0);
	virtual ~DCElasticEventGenerator();

	G4double getBeamEnergy() const {
		return T_beam;
	}

	void setBeamEnergy(G4double beamEnergy) {
		T_beam = beamEnergy;
	}
	double Sigma(G4double q, G4double phi);
	double Sigma(G4double E, G4double q, G4double theta,
			G4double phi);

	void Initialize(G4double beamEnergy);
	void GenerateEvent();
private:

	G4double T_beam;
	TF2 SigmaFunc;

	TGenPhaseSpace ps;
	TLorentzVector cms;
	TLorentzVector beam;
	TLorentzVector target;
	G4ParticleDefinition* beam_particle;
	G4ParticleDefinition* target_particle;
	double q(G4double theta);


	double SigmaUnpol(G4double E,G4double theta);

	double a1(G4double x);
	double a2(G4double x);
	double a3(G4double x);
	double a4(G4double x);
	double a5(G4double x);
	double a6(G4double x);

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
