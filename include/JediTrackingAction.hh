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
	explicit JediTrackingAction(std::shared_ptr<JediPhysicsManager> physicsManager);
	virtual ~JediTrackingAction()=default;

public:
	virtual void  PreUserTrackingAction( const G4Track *  track );

private:

	const G4LogicalVolume *  targetVolume;

	std::shared_ptr<JediPhysicsManager> fPhysicsManager;

};
#endif /* JEDITRACKINGACTION_H_ */
