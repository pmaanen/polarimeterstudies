/*
 * DetectorMessenger.hh
 *
 *  Created on: Jul 15, 2013
 *      Author: maanen
 */

#ifndef DETECTORMESSENGER_HH_
#define DETECTORMESSENGER_HH_

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;
class DetectorMessenger:public G4UImessenger {
public:
	DetectorMessenger(DetectorConstruction*);
	virtual ~DetectorMessenger();
	void SetNewValue(G4UIcommand*, G4String);

private:

	DetectorConstruction* dc;
	G4UIdirectory* dcDir;
	G4UIcmdWithAString* tgtMtCmd;
	G4UIcmdWithoutParameter* updateCmd;
	G4UIcmdWithADoubleAndUnit* pelletSizeCmd;
};

#endif /* DETECTORMESSENGER_HH_ */
