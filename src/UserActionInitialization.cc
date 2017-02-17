/*
 * UserActionInitialization.cc
 *
 *  Created on: 21.11.2014
 *      Author: pmaanen
 */

#include <PrimaryGeneratorAction.hh>
#include <UserActionInitialization.hh>
#include "JediPhysicsManager.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "JediSteppingAction.hh"
#include "JediTrackingAction.hh"
#include <memory>
#include <G4GenericMessenger.hh>
UserActionInitialization::UserActionInitialization(): G4VUserActionInitialization() {}

void UserActionInitialization::Build() const {

	auto physicsManager=std::make_shared<JediPhysicsManager>();
	SetUserAction(new PrimaryGeneratorAction);

	//Calls InitializeGeometry at begin of every run
	SetUserAction(new RunAction(physicsManager));

	//Needs physicsManager for sampling of interaction length
	SetUserAction(new JediSteppingAction(physicsManager));

	//Needed for reset of physicsManager at begin of every event
	SetUserAction(new EventAction(physicsManager));

	SetUserAction(new JediTrackingAction(physicsManager));
}

void UserActionInitialization::BuildForMaster() const {
	SetUserAction(new RunAction(std::make_shared<JediPhysicsManager>()));
}
