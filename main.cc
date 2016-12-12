#define MAIN

//global configuration
#include "global.hh"

//Geometries
#include "DetectorConstruction.hh"

//User defined actions
#include "UserActionInitialization.hh"

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

//Physics Lists
#include <QGSP_INCLXX.hh>
#include <QGSP_BIC.hh>
#include <QGSP_BERT.hh>
#include <QBBC.hh>

//Additional Physics
#include <G4EmParameters.hh>
#include <G4OpticalPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4HadronicProcessStore.hh>
#include <TROOT.h>
namespace CLHEP {}
using namespace CLHEP;
G4VModularPhysicsList* choosePhysList();
extern void initializeConfiguration(int,char**);
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {
	try{
		initializeConfiguration(argc,argv);
	}
	catch(const std::exception& e){
		std::cout<<"uncaught exception in main: "<<e.what()<<std::endl;
		throw e;
	}

#ifdef G4MULTITHREADED
	ROOT::EnableThreadSafety();
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
	G4VModularPhysicsList* the_physics=choosePhysList();

	runManager->SetUserInitialization(the_physics);
	runManager->SetUserInitialization(new UserActionInitialization);
	runManager->Initialize();
	// get the pointer to the UI manager and set verbosities
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	#ifdef G4VIS_USE
	G4VisManager* visManager = nullptr;
#endif



	if(!gConfig.count("general.macro_file")){
#ifdef G4VIS_USE
		//visualization manager
		visManager = new G4VisExecutive;
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
		G4String filename = gConfig["general.macro_file"].as<std::string>().c_str();
		UImanager->ApplyCommand(command+filename);
	}

#ifdef G4VIS_USE
	delete visManager;
#endif

	// job termination
	delete runManager;
	return 0;
}

G4VModularPhysicsList* choosePhysList(){
	G4VModularPhysicsList* the_physics=nullptr;
	if(gConfig["general.physics"].as<string>()=="QGSP_BERT")
		the_physics=new QGSP_BERT(0);
	if(gConfig["general.physics"].as<string>()=="QGSP_BIC")
		the_physics=new QGSP_BIC(0);
	if(gConfig["general.physics"].as<string>()=="QGSP_INCLXX")
		the_physics=new QGSP_INCLXX(0);
	if(gConfig["general.physics"].as<string>()=="QBBC")
		the_physics=new QBBC(0);
	the_physics->RegisterPhysics(new G4StepLimiterPhysics);
	/*
	auto em=G4EmParameters::Instance();
	em->SetMscStepLimitType(fUseDistanceToBoundary);
	em->SetMscRangeFactor(0.02);
	em->SetMinEnergy(1000*CLHEP::eV);
	em->SetMaxEnergy(5*CLHEP::GeV);
	em->SetNumberOfBins(400);
	em->SetSpline(true);
	*/
	return the_physics;
}
