/*
 * JediSteppingAction.hh
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISTEPPINGACTION_HH_
#define INCLUDE_JEDISTEPPINGACTION_HH_

#include <G4UserSteppingAction.hh>

class  G4Step;
class  G4LogicalVolume;
class JediPhysicsManager;
#include <memory>
class JediSteppingAction: public G4UserSteppingAction {
public:
	explicit JediSteppingAction(JediPhysicsManager* physicsManager);
	virtual ~JediSteppingAction()=default;

    virtual void  UserSteppingAction( const G4Step *  step );

private:
    JediPhysicsManager* fPhysicsManager;

};

#endif /* INCLUDE_JEDISTEPPINGACTION_HH_ */
