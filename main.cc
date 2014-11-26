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


	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
										("help,h", "Display this help message")
										("General.config_file,c", po::value<std::string>(), "config file")
										("General.macro_file,m", po::value<std::string>()->default_value("scripts/vis_T0.mac"), "macro file")
										("General.batch_mode,b", po::bool_switch()->default_value(false), "batch mode")
										("Detector.geometry,g", po::value<std::string>()->default_value(""), "geometry file")
										("Generator.beam_particle,p", po::value<int>()->default_value(0), "PDG id of beam")
										("Generator.target_particle,t", po::value<int>()->default_value(0), "PDG id of target")
										("Generator.energy,e", po::value<double>()->default_value(1),"energy of beam in GeV");

	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), vm);
	notify(vm);
	if(vm.count("config_file")){
		cfg.open(vm["config_file"].as<std::string>().c_str(),std::ifstream::in);
		po::store(po::parse_config_file(cfg, description), vm);
		notify(vm);
	}
	// choose the Random engine
	RanecuEngine* theEngine=new RanecuEngine;
	if(argc>2){
		theEngine->setSeed(strtol(argv[2], NULL, 10));
	}
	HepRandom::setTheEngine(theEngine);
#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(2);
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	// set mandatory initialization classes
	DetectorConstruction* detector = new DetectorConstruction;  
	runManager->SetUserInitialization(detector);

	// set physics list
	G4VModularPhysicsList* the_physics = new QGSP_BIC;
	runManager->SetUserInitialization(the_physics);


	//User action initialization
	runManager->SetUserInitialization(new UserActionInitialization);
	Analysis::Instance();

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



	if (!vm["General.batch_mode"].as<bool>())   // Define UI session for interactive mode
	{

#ifdef G4UI_USE
		G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
		G4cout<<"Interactive mode"<<G4endl;
		G4cout <<vm["General.macro_file"].as<std::string>()<<G4endl;
		std::stringstream o;
		o<<"/control/execute "<<vm["General.macro_file"].as<std::string>().c_str();
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
		o<<"/control/execute "<<vm["General.macro_file"].as<std::string>().c_str();
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

