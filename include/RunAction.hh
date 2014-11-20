// ********************************************************************
// *                                                                  *
// *                          EDM Polarimetry                         *
// *                                                                  *
// * Detector physics                                                 *
// * RWTH Aachen 24.10.2012                                           *
// * Fabian Hinder                                                    *
// ********************************************************************

#ifndef polarimeterStudiesRunAction_h
#define polarimeterStudiesRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "TString.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;

class RunAction: public G4UserRunAction
{
	private:
	public:
		RunAction();
		~RunAction();
		G4int fNEvents;

	public:
		void BeginOfRunAction(const G4Run*);
		void EndOfRunAction(const G4Run*);
		inline G4int GetNEvents() { return fNEvents; };


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
