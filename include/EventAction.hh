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



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
	public:
		virtual ~EventAction();
		//! Singleton
			static EventAction* GetInstance() {
				if ( EventAction::_singleton == NULL ) EventAction::_singleton = new EventAction();
				return EventAction::_singleton;
			}

		virtual void   BeginOfEventAction(const G4Event*);
		virtual void   EndOfEventAction(const G4Event*);

		G4int GetEventno();
	private:
		static EventAction* _singleton;
		EventAction();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
