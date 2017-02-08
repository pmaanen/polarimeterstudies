/*
 * JediBreakupProcess.hh
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIBREAKUPPROCESS_HH_
#define INCLUDE_JEDIBREAKUPPROCESS_HH_

#include <G4HadronicProcess.hh>
class JediBreakupModel;
class JediBreakupProcess : public G4HadronicProcess{
public:
	explicit JediBreakupProcess();
	virtual ~JediBreakupProcess();


public:
	G4VParticleChange *  PostStepDoIt( const G4Track &  track,
			const G4Step &  step );

	G4bool  IsApplicable( const G4ParticleDefinition &  particle );

public:

	void    RegisterModel( JediBreakupModel *  model );

private:
	void    CalculateTargetNucleus( const G4Material *  material );

	void    FillTotalResult( G4HadFinalState *  hadFinalState,
			const G4Track &  track );

private:
	JediBreakupModel *   fModel;

private:
	G4ParticleChange *      theTotalResult;

	G4Nucleus               targetNucleus;

	G4bool                  isInitialized;

};

#endif /* INCLUDE_JEDIBREAKUPPROCESS_HH_ */
