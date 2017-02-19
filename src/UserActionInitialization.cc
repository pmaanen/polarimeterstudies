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
#include <TROOT.h>
#include "Rtypes.h"
#include "global.hh"
UserActionInitialization::UserActionInitialization(): G4VUserActionInitialization() {}

void UserActionInitialization::Build() const {
	ROOT::EnableThreadSafety();
	auto physicsManager=std::make_shared<JediPhysicsManager>();

	SetUserAction(new PrimaryGeneratorAction);

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
	SetUserAction(new RunAction(std::make_shared<JediPhysicsManager>()));
}
