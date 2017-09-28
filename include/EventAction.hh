// ********************************************************************
// *                                                                  *
// *                          EDM Polarimetry                         *
// *                                                                  *
// * Detector physics                                                 *
// * RWTH Aachen 24.10.2012                                           *
// * Fabian Hinder                                                    *
// ********************************************************************

#ifndef polarimeterStudiesEventAction_h
#define polarimeterStudiesEventAction_h 1


#include "RunAction.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include <memory>
class JediPhysicsManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
	public:
		virtual ~EventAction();
		EventAction(JediPhysicsManager* physicsManager);
		virtual void   BeginOfEventAction(const G4Event*);
		virtual void   EndOfEventAction(const G4Event* evt);

		G4int GetEventno();
	private:

		JediPhysicsManager* fPhysicsManager;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
