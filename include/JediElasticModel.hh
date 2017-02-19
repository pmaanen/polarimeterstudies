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
	G4double SamplePhi(G4double kinEnergy, G4double pol, G4double thetaLab);
private:
	virtual void DefineCommands();
	G4double SampleQ(G4double kinEnergy);

	Double_t fBeamPolarization;
	std::unique_ptr<TF1> fQ;
	std::unique_ptr<TF1> fPhi;
	G4double fQmin,fQmax,fThetaMin,fThetaMax;
	std::unique_ptr<G4GenericMessenger> fMessenger;
	G4ParticleDefinition *    fIncidentParticle,*fNucleus;
	std::vector<G4ParticleDefinition*> fOutParticles;
	G4LorentzVector fNucleusMomentumLAB,fIncidentParticleMomentumLAB,fNucleusMomentumCMS,fIncidentParticleMomentumCMS;
	G4double fNucleusMass,fIncidentParticleMass;
};

#endif /* INCLUDE_JEDIELASTICMODEL_HH_ */
