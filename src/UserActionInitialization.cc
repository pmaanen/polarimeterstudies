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
#include <JediCommon.hh>
#include <TROOT.h>
#include "Rtypes.h"
UserActionInitialization::UserActionInitialization(): G4VUserActionInitialization() {}
void UserActionInitialization::Build() const {

	SetUserAction(new PrimaryGeneratorAction);
	auto physicsManager=JediPhysicsManager::GetInstance();
	//Calls InitializeGeometry at begin of every run
	SetUserAction(new RunAction(physicsManager));

	//Needs physicsManager for sampling of interaction length
	if(gConfig["physics.use_fast_sim"].as<bool>()){
		SetUserAction(new JediSteppingAction(physicsManager));
		SetUserAction(new JediTrackingAction(physicsManager));
	}
	//Needed for reset of physicsManager at begin of every event
	SetUserAction(new EventAction(physicsManager));

}

void UserActionInitialization::BuildForMaster() const {
	auto physicsManager=JediPhysicsManager::GetInstance();

	SetUserAction(new RunAction(physicsManager));
}
