
#define MAIN
#include "global.hh"
#include "Analysis.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif

#include <G4UImanager.hh>
#include <G4UIterminal.hh>
#include <G4UItcsh.hh>
#include <UserActionInitialization.hh>

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#ifdef G4UI_USE
#include <G4UIExecutive.hh>
#endif

#include <QGSP_INCLXX.hh>
#include <QGSP_BIC.hh>
#include <FTFP_BERT.hh>
#include <G4OpticalPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <ctime>
namespace CLHEP {}
using namespace CLHEP; 

int main(int argc,char** argv) {

	try{
		initializeConfiguration(argc,argv);
	}
	catch(const std::exception& e){
		std::cout<<"uncaught exception in main: "<<e.what()<<std::endl;
		throw e;
	}
	// choose the Random engine
	RanecuEngine* theEngine=new RanecuEngine;
	if(gConfig.count("general.seed"))
		theEngine->setSeed(gConfig["general.seed"].as<int>());
	else
		theEngine->setSeed(std::time(0));

	HepRandom::setTheEngine(theEngine);
#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager();
	runManager->SetNumberOfThreads(gConfig["general.num_threads"].as<int>());
#else
	G4RunManager* runManager = new G4RunManager();
#endif
	runManager->SetVerboseLevel(0);
	// set mandatory initialization classes
	DetectorConstruction* detector = new DetectorConstruction;
	runManager->SetUserInitialization(detector);

	// set physics list
	G4VModularPhysicsList* the_physics =new FTFP_BERT(0);//new QGSP_BIC(0);//new QGSP_INCLXX();//new FTFP_BERT(0);
	the_physics->RegisterPhysics(new G4RadioactiveDecayPhysics(0));
	runManager->SetUserInitialization(the_physics);
	//User action initialization
	runManager->SetUserInitialization(new UserActionInitialization);
#ifdef G4VIS_USE
	// Visualization manager
	//
	G4VisManager* visManager = new G4VisExecutive("quiet");
	visManager->Initialize();
#endif

	// Get the pointer to the User Interface manager
	//
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager->SetVerboseLevel(0);
	// Initialize G4 kernel
	//
	//runManager->Initialize();

	if (!gConfig["general.batch_mode"].as<bool>())   // Define UI session for interactive mode
	{

#ifdef G4UI_USE
		G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
		UImanager->ApplyCommand("/run/initialize");
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
		UImanager->ApplyCommand("/run/initialize");
		std::stringstream o;
		if(gConfig.count("general.macro_file")){
			o<<"/control/execute "<<gConfig["general.macro_file"].as<std::string>().c_str();
			UImanager->ApplyCommand(o.str().c_str());
		}
	}

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;
	return 0;
}

