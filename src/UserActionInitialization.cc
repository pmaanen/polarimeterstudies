/*
 * UserActionInitialization.cc
 *
 *  Created on: 21.11.2014
 *      Author: pmaanen
 */

#include <PrimaryGeneratorAction.hh>
#include <UserActionInitialization.hh>
#include "RunAction.hh"
#include "EventAction.hh"
UserActionInitialization::UserActionInitialization(): G4VUserActionInitialization() {}

void UserActionInitialization::Build() const {
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new RunAction());
	SetUserAction(new EventAction);
}

void UserActionInitialization::BuildForMaster() const {
	SetUserAction(new RunAction);
}
