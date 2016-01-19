
#define MAIN
#include "global.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "UserActionInitialization.hh"
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "DetectorConstruction.hh"
#include "JediCubicPolarimeter.hh"
#include "JediHexagonalPolarimeter.hh"
#include "SingleCrystal.hh"
#include <JediSandwichCalorimeter.hh>
#include <Testbench.hh>
#include "EventAction.hh"
#include <QGSP_INCLXX.hh>
#include <QGSP_BIC.hh>
#include <FTFP_BERT.hh>
#include "G4OpticalPhysics.hh"
#include <G4RadioactiveDecayPhysics.hh>
#include <PrimaryGeneratorAction.hh>
#include "Analysis.hh"
#include "G4StateManager.hh"

#include <signal.h>
#include <ctime>
namespace CLHEP {}
using namespace CLHEP; 


void    Interrupt(int signum) {
	auto state=G4StateManager::GetStateManager()->GetCurrentState();
	if(state==G4ApplicationState::G4State_EventProc
			or state==G4ApplicationState::G4State_GeomClosed)
		G4RunManager::GetRunManager()->AbortRun();
	exit(signum);
}

int main(int argc,char** argv) {

	signal(SIGTERM,&Interrupt) ;
	signal(SIGINT ,&Interrupt) ;
	signal(SIGPIPE,&Interrupt) ;
	try{
		initializeConfiguration(argc,argv);
	}
	catch(const std::exception& e){
		std::cout<<"uncaught exception in main: "<<e.what()<<std::endl;
		throw e;
	}
	// choose the Random engine
	RanecuEngine* theEngine=new RanecuEngine;
	if(gConfig.count("general.seed")){
		theEngine->setSeed(gConfig["general.seed"].as<int>());
	}
	else{
		theEngine->setSeed(std::time(0));
	}

	HepRandom::setTheEngine(theEngine);
#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(gConfig["general.num_threads"].as<int>());
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	// set mandatory initialization classes
	DetectorConstruction* detector = new DetectorConstruction;
	runManager->SetUserInitialization(detector);

	// set physics list
	G4VModularPhysicsList* the_physics =new QGSP_BIC;//new QGSP_INCLXX();//new FTFP_BERT(0);
	the_physics->SetVerboseLevel(0);
	the_physics->RegisterPhysics(new G4RadioactiveDecayPhysics);
	runManager->SetUserInitialization(the_physics);
	//User action initialization
	runManager->SetUserInitialization(new UserActionInitialization);
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



	if (!gConfig["general.batch_mode"].as<bool>())   // Define UI session for interactive mode
	{

#ifdef G4UI_USE
		G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
		G4cout<<"Interactive mode"<<G4endl;
		G4cout <<gConfig["general.macro_file"].as<std::string>()<<G4endl;
		std::stringstream o;
		o<<"/control/execute "<<gConfig["general.macro_file"].as<std::string>().c_str();
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
		if(gConfig.count("general.macro_file")){
			o<<"/control/execute "<<gConfig["general.macro_file"].as<std::string>().c_str();
			UImanager->ApplyCommand(o.str().c_str());
		}
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

