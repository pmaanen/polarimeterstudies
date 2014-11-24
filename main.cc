#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif


#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include <QGSP_BIC.hh>
#include "G4OpticalPhysics.hh"
#include "SFEventGenerator.hh"
#include "Analysis.hh"
#define MAIN
#include "global.hh"
namespace CLHEP {}
using namespace CLHEP; 


int main(int argc,char** argv) {


	// choose the Random engine
	RanecuEngine* theEngine=new RanecuEngine;
	if(argc>2){
		theEngine->setSeed(strtol(argv[2], NULL, 10));
	}
	HepRandom::setTheEngine(theEngine);
	// Construct the default run manager
	G4RunManager * runManager = new G4RunManager;

	// set mandatory initialization classes
	DetectorConstruction* detector = new DetectorConstruction;  
	runManager->SetUserInitialization(detector);

	// set physics list
	G4VModularPhysicsList* the_physics = new QGSP_BIC;
	runManager->SetUserInitialization(the_physics);
	SFEventGenerator *primarygeneration = SFEventGenerator::GetInstance();
	runManager->SetUserAction(primarygeneration);
	RunAction* runaction=new RunAction();
	runManager->SetUserAction(runaction);
	EventAction* eventaction=EventAction::GetInstance();
	runManager->SetUserAction(eventaction);

	runManager->SetUserAction(SteppingAction::GetInstance());
	Analysis::GetInstance();

#ifdef G4VIS_USE
	// Visualization manager
	//
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
#endif

	// Initialize G4 kernel
	//
	runManager->Initialize();

	// Get the pointer to the User Interface manager
	//
	G4UImanager* UImanager = G4UImanager::GetUIpointer();



	if (argc==1)   // Define UI session for interactive mode
	{

#ifdef G4UI_USE
		G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
		G4cout<<"Interactive mode"<<G4endl;
		std::stringstream o;
		o<<"/control/execute scripts/vis_T0.mac";
		UImanager->ApplyCommand(o.str().c_str());
#endif
		ui->SessionStart();
		delete ui;
#endif
	}
	else           // Batch mode
	{
		G4cout<<"Batch mode"<<G4endl;
		std::stringstream o;
		o<<"/control/execute "<<argv[1];
		UImanager->ApplyCommand(o.str().c_str());
	}

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	//                 owned and deleted by the run manager, so they should not
	//                 be deleted in the main() program !

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;



	return 0;
}

