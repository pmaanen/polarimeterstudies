/*
 * JediPhysicsManager.hh
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 *
 *      Manages the physics in the case of biased event sampling
 *
 *
 */

#ifndef INCLUDE_JEDIPHYSICSMANAGER_HH_
#define INCLUDE_JEDIPHYSICSMANAGER_HH_
#include <G4AffineTransform.hh>
class JediPolarimeter;
class G4Track;
class G4StepPoint;
class G4VSolid;
class G4GenericMessenger;
#include <memory>


class JediPhysicsManager {
public:
	JediPhysicsManager();
	virtual ~JediPhysicsManager()=default;

	void    ResampleTrackLengthInTarget( const G4Track *  track,
			const G4StepPoint *  stepPoint=nullptr );

	void    GeometryHasChanged( const JediPolarimeter *  setup );

	G4bool  IsStudiedProcessAllowed( void ) const;

	G4bool    OnlyBeamParticleCanTriggerStudiedProcess( void ) const;

	void      IncrementNumberOfTriggeredStudiedInteractions( void );

	void      ResetNumberOfTriggeredStudiedInteractions( void );

	G4double  GetProposedMaxIL( void ) const;

	void      SetMaxIL( const G4ThreeVector &  direction );

	void      SetMaxILCorrection( G4double  value );

	void      SetProposedMaxIL( G4double  value );

protected:
	virtual void    CalculateBasicMaxIL(
			const G4ThreeVector &  direction );

protected:
	G4double  basicMaxIL;

	G4double  maxILCorrection;

	G4double  proposedMaxIL;

	G4int     numberOfTriggeredStudiedInteractions;

	G4bool    onlyBeamParticleCanTriggerStudiedProcess;

private:
	std::unique_ptr<G4GenericMessenger> fMessenger;

	G4VSolid *                           targetSolid;

	G4AffineTransform                    targetTransform;
};

inline G4bool  JediPhysicsManager::OnlyBeamParticleCanTriggerStudiedProcess(
                                                                    void ) const
{
    return onlyBeamParticleCanTriggerStudiedProcess;
}


inline void  JediPhysicsManager::IncrementNumberOfTriggeredStudiedInteractions(
                                                                        void )
{
    ++numberOfTriggeredStudiedInteractions;
}


inline void  JediPhysicsManager::ResetNumberOfTriggeredStudiedInteractions(
                                                                        void )
{
    numberOfTriggeredStudiedInteractions = 0;
}


inline G4double  JediPhysicsManager::GetProposedMaxIL( void ) const
{
    return proposedMaxIL;
}

inline void  JediPhysicsManager::SetMaxIL( const G4ThreeVector &  direction )
{
    CalculateBasicMaxIL( direction );
    proposedMaxIL = basicMaxIL + maxILCorrection;
}


inline void  JediPhysicsManager::SetMaxILCorrection( G4double  value )
{
    maxILCorrection = value;
    proposedMaxIL = basicMaxIL + maxILCorrection;
}


inline void  JediPhysicsManager::SetProposedMaxIL( G4double  value )
{
    proposedMaxIL = value;
}

#endif /* INCLUDE_JEDIPHYSICSMANAGER_HH_ */
