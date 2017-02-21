/*
 * JediTrackingAction.h
 *
 *  Created on: 06.02.2017
 *      Author: pmaanen
 */

#ifndef JEDITRACKINGACTION_H_
#define JEDITRACKINGACTION_H_

#include <G4UserTrackingAction.hh>
class JediPhysicsManager;
class G4LogicalVolume;

#include <G4Types.hh>
#include <memory>
class JediTrackingAction: public G4UserTrackingAction {
public:
	explicit JediTrackingAction(JediPhysicsManager* physicsManager);
	virtual ~JediTrackingAction()=default;

public:
	virtual void  PreUserTrackingAction( const G4Track *  track );

private:

	JediPhysicsManager* fPhysicsManager;

};
#endif /* JEDITRACKINGACTION_H_ */
