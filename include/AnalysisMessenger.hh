/*
 * polarimeterStudiesAMessenger.hh
 *
 *  Created on: Jun 19, 2013
 *      Author: maanen
 */
#ifndef POLARIMETERSTUDIESAMESSENGER_HH_
#define POLARIMETERSTUDIESAMESSENGER_HH_

#include "globals.hh"
#include "G4UImessenger.hh"

class Analysis;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcommand;
class AnalysisMessenger: public G4UImessenger{
public:
	AnalysisMessenger(Analysis*);
	virtual ~AnalysisMessenger();
	void SetNewValue(G4UIcommand*, G4String);

private:

	Analysis* ana;
	G4UIdirectory* anaDir;
	G4UIcmdWithAString* dirCmd;
	G4UIcmdWithAString* fileCmd;
	G4UIcommand* enableCmd;
	G4UIcommand* disableCmd;
	G4UIcmdWithAString* auxCmd;

};

#endif /* POLARIMETERSTUDIESAMESSENGER_HH_ */
