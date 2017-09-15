/*
 * UserActionInitialization.hh
 *
 *  Created on: 21.11.2014
 *      Author: pmaanen
 */

#ifndef USERACTIONINITIALIZATION_HH_
#define USERACTIONINITIALIZATION_HH_

#include <G4VUserActionInitialization.hh>

class UserActionInitialization: public G4VUserActionInitialization {
public:
	UserActionInitialization();
	virtual void Build() const;
	virtual void BuildForMaster() const;
};

#endif /* USERACTIONINITIALIZATION_HH_ */
