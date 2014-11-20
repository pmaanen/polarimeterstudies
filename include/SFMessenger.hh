/*
 * SFMessenger.hh
 *
 *  Created on: Jan 23, 2014
 *      Author: maanen
 */

#ifndef SFMESSENGER_HH_
#define SFMESSENGER_HH_
#include "G4UImessenger.hh"
#include "globals.hh"
class SFEventGenerator;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithBool;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class SFMessenger: public G4UImessenger {
public:
	SFMessenger(SFEventGenerator*);
	void SetNewValue(G4UIcommand*, G4String);
	virtual ~SFMessenger();
private:
	SFEventGenerator* myGenerator;
	G4UIdirectory* gunDir;
	G4UIcmdWithAnInteger* modeCmd;
	G4UIcmdWithAString* fileCmd;
};

#endif /* SFMESSENGER_HH_ */
