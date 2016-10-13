
#define MAIN

//global configuration
#include "global.hh"

//Geometries
#include "DetectorConstruction.hh"
#include "JediCubicPolarimeter.hh"
#include "JediHexagonalPolarimeter.hh"
#include "SingleCrystal.hh"
#include "SingleSandwichModule.hh"
#include "JediSandwichCalorimeter.hh"
#include "Testbench.hh"

//User defined actions
#include "Analysis.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

//Geant4 stuff
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
	G4VModularPhysicsList* the_physics=nullptr;
	if(gConfig["general.physics"].as<string>()=="QGSP_BERT")
		the_physics=new QGSP_BERT(0);
	if(gConfig["general.physics"].as<string>()=="QGSP_BIC")
		the_physics=new QGSP_BIC(0);
	if(gConfig["general.physics"].as<string>()=="QGSP_INCLXX")
		the_physics=new QGSP_INCLXX(0);
	if(gConfig["general.physics"].as<string>()=="QBBC")
		the_physics=new QBBC(0);
	if(!the_physics)
		G4Exception("main","PHYS01", FatalException, "no physics list");



	//Low energy electromagnetic options
	/*
	 * opt.SetVerbose(1) ;
   // Multiple Coulomb scattering
   //
   opt.SetMscStepLimitation(fUseDistanceToBoundary) ;
   opt.SetMscRangeFactor(0.02) ;
   // Physics tables
   //
   opt.SetMinEnergy(1000*eV) ; // default 100*eV
   opt.SetMaxEnergy(5*GeV) ; // default 100*TeV
   opt.SetDEDXBinning(400) ; // default 12*7
   opt.SetLambdaBinning(250) ; // default 12*7
   opt.SetSplineFlag(true) ; // default true
   // Ionization
   //
   opt.SetSubCutoff(true) ; // default false
   // ...
	 */
	//the_physics->RegisterPhysics(new G4RadioactiveDecayPhysics(0));
	the_physics->RegisterPhysics(new G4StepLimiterPhysics);
	auto em=G4EmParameters::Instance();
	em->SetMscStepLimitType(fUseDistanceToBoundary);
	em->SetMscRangeFactor(0.02);

	em->SetMinEnergy(1000*CLHEP::eV);
	em->SetMaxEnergy(5*CLHEP::GeV);
	em->SetNumberOfBins(400);
	em->SetSpline(true);
	G4HadronicProcessStore::Instance()->SetVerbose(0);
	runManager->SetUserInitialization(the_physics);
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
	G4cout<<UImanager->GetMacroSearchPath()<<G4endl;
	if (!gConfig["general.batch_mode"].as<bool>())   // Define UI session for interactive mode
	{

#ifdef G4UI_USE
		G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
		UImanager->ApplyCommand("/run/initialize");

		std::stringstream o;
		if(gConfig.count("general.macro_file")){
			o<<"/control/execute "<<gConfig["general.macro_file"].as<std::string>().c_str();

		}
		else{
			o<<"/control/execute "<<"scripts/vis_T0.mac";
		}
		UImanager->ApplyCommand(o.str().c_str());
#endif
		ui->SessionStart();
		runManager->Initialize();
		delete ui;
#endif
	}
	else           // Batch mode
	{
		UImanager->ApplyCommand("/run/initialize");
		std::stringstream o;
		if(gConfig.count("general.macro_file")){
			o<<"/control/execute "<<gConfig["general.macro_file"].as<std::string>().c_str();

		}
		else{
			o<<"/control/execute "<<"scripts/vis_T0.mac";
		}
		UImanager->ApplyCommand(o.str().c_str());
	}

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;
	return 0;
}

