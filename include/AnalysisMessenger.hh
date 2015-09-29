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
#include "G4AnalysisMessenger.hh"
class Analysis;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcommand;
class AnalysisMessenger: public G4AnalysisMessenger{
public:
	AnalysisMessenger(Analysis*);
	virtual ~AnalysisMessenger();
	void SetNewValue(G4UIcommand*, G4String);

private:

	Analysis* ana;
	G4UIcommand* enableCmd;
	G4UIcommand* disableCmd;

};

#endif /* POLARIMETERSTUDIESAMESSENGER_HH_ */
