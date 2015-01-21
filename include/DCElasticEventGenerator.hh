/*
 * DCElasticEventGenerator.hh
 *
 *  Created on: Dec 11, 2014
 *      Author: maanen
 */

#ifndef DCELASTICEVENTGENERATOR_HH_
#define DCELASTICEVENTGENERATOR_HH_
#include <G4ThreeVector.hh>
#include "TMath.h"
#include "TF2.h"
#include "TVector3.h"
#include "TGenPhaseSpace.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "globals.hh"
#include <TMath.h>
#include <utility>
#include <vector>

#include <G4UnitsTable.hh>
class G4ParticleDefinition;
class G4GenericMessenger;

typedef std::vector<std::pair<G4ParticleDefinition*,G4ThreeVector> > ParticleMomentumVector;

class DCElasticEventGenerator {
public:
	DCElasticEventGenerator();
	virtual ~DCElasticEventGenerator();

	G4double getBeamEnergy() const {return T_beam;}
	void setBeamEnergy(G4double beamEnergy) {T_beam = beamEnergy;Initialize();}
	void setBeamPolarization(G4double beamPolarization) {beam_polarization = beamPolarization;Initialize();}


	void Initialize();
	//Generates one Event.
	ParticleMomentumVector GenerateEvent();

private:

	class MyFunction{
	public:
		Double_t sigma(Double_t *x, Double_t *par);
		MyFunction(Double_t mom, Double_t pol);

	private:
		Double_t SigmaUnpol(Double_t, Double_t);
		const Double_t momentum_cms,beam_polarization;
		//quick conversion for theta_cm to momentum transfer
		Double_t q(Double_t theta);


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

  G4double T_beam,MaxY;
  static G4ThreadLocal TF2* SigmaFunc;
	TGenPhaseSpace ps;
	//This is the Function to build the TF2. x[0] is theta_cm, x[1] is phi par[0] is beam energy.
	TLorentzVector cms,beam,target;
	Double_t momentum_cms,beamPolarization;
	G4ParticleDefinition *beam_particle,*target_particle;

	G4GenericMessenger* fMessenger;

	void DefineCommands();
	//Returns a the TF2 for hit and miss.
	TF2* BuildFunction();

	//This is the Function to build the TF2.
	//x[0] is theta_cm, x[1] is phi par[0] is beam energy. par[1] is beam momentum in cms
	//par[2] is polarization
	//Units: E:MeV Theta:deg q:GeV/c
	//Note: The root classes use GeV
	//Sigma=SigmaUnpol*(1+Ay*Py*cos(phi))

	class MyFunction{
	public:
		Double_t sigma(Double_t *x, Double_t *par);
		MyFunction();
	private:
		Double_t SigmaUnpol(Double_t, Double_t, Double_t);
		//quick conversion for theta_cm to momentum transfer
		Double_t q(Double_t theta, Double_t mom);

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
	static G4ThreadLocal MyFunction* func;
	void setBeamEnergy(G4double T){T_beam=T;G4cout<<G4BestUnit(T_beam,"Energy");Initialize();};
	void setBeamPolarization(G4double p){beamPolarization=Double_t(p);Initialize();};
};
#endif /* DCELASTICEVENTGENERATOR_HH_ */
