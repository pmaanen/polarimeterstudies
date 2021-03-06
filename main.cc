
//global configuration
#include <DetectorConstruction.hh>
#include <JediConfigurationManager.hh>

//Geometries
#include "UserActionInitialization.hh"

//Physics
#include "JediPhysicsListFactory.hh"

//Geant4 stuff
#include <G4UImanager.hh>
#include <G4UIterminal.hh>
#include <G4UItcsh.hh>

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#ifdef G4UI_USE
#include <G4UIExecutive.hh>
#endif

#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif

#include <TROOT.h>
#include "Rtypes.h"

#include <memory>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {
	std::unique_ptr<JediConfigurationManager> ConfigurationManager;
	try{
		ConfigurationManager.reset(new JediConfigurationManager(argc,argv));
	}
	catch(const std::exception& e){
		std::cout<<"uncaught exception in main: "<<e.what()<<std::endl;
		throw e;
	}

#ifdef G4MULTITHREADED
	ROOT::EnableThreadSafety();
	G4MTRunManager* runManager = new G4MTRunManager();
	runManager->SetNumberOfThreads(JediConfigurationManager::Instance()->GetMap()["general.num_threads"].as<int>());
#else
	G4RunManager* runManager = new G4RunManager();
#endif
	runManager->SetVerboseLevel(0);


	// set mandatory initialization classes
	G4VUserDetectorConstruction* detector =DetectorConstruction::Create();
	runManager->SetUserInitialization(detector);

	// set physics list
	G4VModularPhysicsList* the_physics=JediPhysicsListFactory::Create();
	runManager->SetUserInitialization(the_physics);

	// all other user action
	runManager->SetUserInitialization(new UserActionInitialization);

	// get the pointer to the UI manager and set verbosities
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager->SetVerboseLevel(0);

#ifdef G4VIS_USE
	G4VisManager* visManager = nullptr;
#endif



	if(!JediConfigurationManager::Instance()->GetMap().count("general.macro_file")){
#ifdef G4VIS_USE
		//visualization manager
		visManager = new G4VisExecutive();
		visManager->SetVerboseLevel(JediConfigurationManager::Instance()->GetVerbose());
		visManager->Initialize();
#endif
#ifdef G4UI_USE
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);

		UImanager->ApplyCommand("/control/execute scripts/gui.mac");
		ui->SessionStart();
		delete ui;
#endif
	}
	else{
		G4String command = "/control/execute ";
		G4String filename = JediConfigurationManager::Instance()->GetMap()["general.macro_file"].as<std::string>().c_str();
		UImanager->ApplyCommand(command+filename);
	}

#ifdef G4VIS_USE
	delete visManager;
#endif

	// job termination
	delete runManager;
	return 0;
}
