// ********************************************************************
// *                                                                  *
// *                          EDM Polarimetry                         *
// *                                                                  *
// * Detector physics                                                 *
// * RWTH Aachen 24.10.2012                                           *
// * Fabian Hinder                                                    *
// ********************************************************************


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "JediPhysicsManager.hh"


#include "EventAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4VVisManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "Analysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(JediPhysicsManager* physicsManager):G4UserEventAction(),fPhysicsManager(physicsManager)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{;}


//*********************************************************************************
//******************  Begin Of Event Action                 ***********************
//*********************************************************************************

void EventAction::BeginOfEventAction(const G4Event* /*evt*/)
{
	Analysis::Instance()->BeginOfEvent();
	//fPhysicsManager->Reset
}


//*********************************************************************************
//******************  End Of Event Action                 ***********************
//*********************************************************************************
void EventAction::EndOfEventAction(const G4Event* evt)
{
	Analysis::Instance()->EndOfEvent(evt);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int EventAction::GetEventno()
{
	G4int evno = fpEventManager->GetConstCurrentEvent()->GetEventID() ;
	return evno ;
}
