/*
 * SensitiveDetectorMessenger.hh
 *
 *  Created on: Jul 3, 2013
 *      Author: maanen
 */

#ifndef SENSITIVEDETECTORMESSENGER_HH_
#define SENSITIVEDETECTORMESSENGER_HH_
#include "globals.hh"
#include "G4UImessenger.hh"
class G4UIcmdWithADoubleAndUnit;
class SensitiveDetector;

class SensitiveDetectorMessenger: public G4UImessenger {
public:
	SensitiveDetectorMessenger(SensitiveDetector* sensDet);
	virtual ~SensitiveDetectorMessenger();
	void SetNewValue(G4UIcommand*, G4String);
private:
	SensitiveDetector* mydet;
	G4UIdirectory* sensDir;
	G4UIcmdWithADoubleAndUnit* sensCmd;
};

#endif /* SENSITIVEDETECTORMESSENGER_HH_ */
