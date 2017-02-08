/*
 * JediElasticScatteringProcess.hh
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIELASTICPROCESS_HH_
#define INCLUDE_JEDIELASTICPROCESS_HH_

#include <G4HadronicProcess.hh>
#include <memory>
#include <Rtypes.h>
class TGenPhaseSpace;
class JediElasticModel;
class JediElasticProcess: public G4HadronicProcess {
public:
	explicit JediElasticProcess();
	virtual ~JediElasticProcess();


public:
	G4VParticleChange *  PostStepDoIt( const G4Track &  track,
			const G4Step &  step );

	G4bool  IsApplicable( const G4ParticleDefinition &  particle );

	void    RegisterModel( JediElasticModel *  model );

private:
	void    CalculateTargetNucleus( const G4Material *  material );

	void    FillTotalResult( G4HadFinalState *  hadFinalState,
			const G4Track &  track );

private:
	JediElasticModel *   fModel;

	G4ParticleChange *      theTotalResult;

	G4Nucleus               targetNucleus;
};

#endif /* INCLUDE_JEDIELASTICPROCESS_HH_ */
