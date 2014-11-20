/*
 * SteppingAction.hh
 *
 *  Created on: Oct 20, 2014
 *      Author: maanen
 */

#ifndef STEPPINGACTION_HH_
#define STEPPINGACTION_HH_
#include <stddef.h>
#include <G4UserSteppingAction.hh>
class G4Step;
class mytree;
class SteppingAction: public G4UserSteppingAction {
public:

	static SteppingAction* GetInstance() {
		if ( SteppingAction::singleton == NULL ) SteppingAction::singleton = new SteppingAction();
		return SteppingAction::singleton;
	}



	virtual ~SteppingAction();

	virtual void UserSteppingAction(const G4Step*);


private:
	SteppingAction();
	static SteppingAction* singleton;
};

#endif /* STEPPINGACTION_HH_ */
