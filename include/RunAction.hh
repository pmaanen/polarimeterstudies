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

#include <vector>
#include <memory>

class JediPhysicsManager;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;

class RunAction: public G4UserRunAction
{
public:

	RunAction(JediPhysicsManager* physicsManager);
	virtual ~RunAction()=default;
	G4int fNEvents;

	void BeginOfRunAction(const G4Run*);
	void EndOfRunAction(const G4Run*);
	G4Run* GenerateRun();
	inline G4int GetNEvents() { return fNEvents; };

private:
	JediPhysicsManager* fPhysicsManager;
	G4bool fSaveRndm;
	G4int fLuxury;
	G4int fSeed;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
