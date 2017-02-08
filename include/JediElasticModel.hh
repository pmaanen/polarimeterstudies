/*
 * DeuteronCarbonElasticScatteringModel.hh
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIELASTICMODEL_HH_
#define INCLUDE_JEDIELASTICMODEL_HH_


#include <G4HadronicInteraction.hh>
#include <G4GenericMessenger.hh>
#include <TF1.h>

#include <memory>

class JediElasticModel: public G4HadronicInteraction {
public:
	JediElasticModel();
	virtual ~JediElasticModel(){theParticleChange.Clear();};

	G4HadFinalState * ApplyYourself(const G4HadProjectile & aTrack, G4Nucleus & targetNucleus);
	void setBeamPolarization(G4double xBeamPolarization) {fBeamPolarization = Double_t(xBeamPolarization);};

private:


	virtual void DefineCommands();
	Double_t q(Double_t*, Double_t*);
	Double_t Phi(Double_t*, Double_t*);

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

	Double_t fBeamPolarization;
	std::unique_ptr<TF1> fQ;
	std::unique_ptr<TF1> fPhi;
	Double_t fQmin,fQmax,fThetaMin,fThetaMax;
	std::unique_ptr<G4GenericMessenger> fMessenger;
	G4ParticleDefinition *    fIncidentParticle,*fNucleus;
	std::vector<G4ParticleDefinition*> fOutParticles;
	G4LorentzVector fNucleusMomentumLAB,fIncidentParticleMomentumLAB,fNucleusMomentumCMS,fIncidentParticleMomentumCMS;
	G4double fNucleusMass,fIncidentParticleMass;

};

#endif /* INCLUDE_JEDIELASTICMODEL_HH_ */
