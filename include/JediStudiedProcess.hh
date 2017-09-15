/*
 * JediStudiedProcess.hh
 *
 *  Created on: 17.01.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISTUDIEDPROCESS_HH_
#define INCLUDE_JEDISTUDIEDPROCESS_HH_

#include <G4WrapperProcess.hh>


#include <G4WrapperProcess.hh>
#include <G4ProcessType.hh>

class  G4VParticleChange;

class JediStudiedProcess: public G4WrapperProcess {
public:
public:
	explicit  JediStudiedProcess();
  	virtual ~JediStudiedProcess()=default;

  public:
  	virtual G4double PostStepGetPhysicalInteractionLength (const G4Track &, G4double, G4ForceCondition *);

      virtual G4VParticleChange *  PostStepDoIt( const G4Track &  track,
                                         const G4Step &  step );

};

#endif /* INCLUDE_JEDISTUDIEDPROCESS_HH_ */
