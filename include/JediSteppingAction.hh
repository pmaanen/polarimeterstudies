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
	explicit JediSteppingAction(std::shared_ptr<JediPhysicsManager> physicsManager);
	virtual ~JediSteppingAction()=default;

    virtual void  UserSteppingAction( const G4Step *  step );

private:
    std::shared_ptr<JediPhysicsManager> fPhysicsManager;

};

#endif /* INCLUDE_JEDISTEPPINGACTION_HH_ */
