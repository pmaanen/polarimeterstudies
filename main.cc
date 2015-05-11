
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
#include "SandwichCalorimeter.hh"
#include "CosmicSetup.hh"
#include "EventAction.hh"
#include <QGSP_INCLXX.hh>
#include <FTFP_BERT.hh>
#include "G4OpticalPhysics.hh"
#include <G4RadioactiveDecayPhysics.hh>
#include <PrimaryGeneratorAction.hh>
#include "Analysis.hh"
#include <signal.h>
namespace CLHEP {}
using namespace CLHEP; 


void    Interrupt(int signum) { (G4RunManager::GetRunManager())->AbortRun() ; }



int main(int argc,char** argv) {

	signal(SIGTERM,&Interrupt) ;
	signal(SIGINT ,&Interrupt) ;
	signal(SIGPIPE,&Interrupt) ;


	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
				("help,h", "Display this help message")
				("general.config_file,c", po::value<std::string>(), "config file")
				("general.num_threads,n", po::value<int>()->default_value(1), "number of threads.")
				("general.macro_file,m", po::value<std::string>()->default_value("scripts/vis_T0.mac"), "macro file")
				("general.batch_mode,b", po::bool_switch()->default_value(false), "batch mode")
				("detector.geometry,g", po::value<std::string>()->default_value(""), "geometry file")
				("generator.beam_particle,p", po::value<int>()->default_value(0), "PDG id of beam")
				("generator.target_particle,t", po::value<int>()->default_value(0), "PDG id of target")
				("generator.beam_energy,e", po::value<double>()->default_value(1),"energy of beam in MeV")
				("detector.positions", po::value<std::vector<double> >()->multitoken(),"positions");

	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), vm);
	notify(vm);
	if(vm.count("general.config_file")){
		cfg.open(vm["general.config_file"].as<std::string>().c_str(),std::ifstream::in);
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
	runManager->SetNumberOfThreads(vm["general.num_threads"].as<int>());
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	// set mandatory initialization classes
	//DetectorConstruction* detector = new DetectorConstruction;
	auto geometry=vm["detector.geometry"].as<std::string>();
	std::string cubic("cubic:");
	std::string hexagonal("hexagonal:");
	std::string gdml("gdml:");
	std::string single("single:");
	std::string sandwich("sandwich:");
	std::string cosmic("cosmic:");
	G4VUserDetectorConstruction* jedi=0;
	if(!geometry.compare(0,cubic.size(),cubic)){
		jedi=new JediCubicPolarimeter(geometry.substr(cubic.size(),geometry.size()));
	}
	if(!geometry.compare(0,hexagonal.size(),hexagonal)){
		jedi=new JediHexagonalPolarimeter;
	}
	if(!geometry.compare(0,gdml.size(),gdml)){
		jedi= new DetectorConstruction();
	}
	if(!geometry.compare(0,single.size(),single)){
		jedi= new SingleCrystal();
	}
	if(!geometry.compare(0,sandwich.size(),sandwich)){
		jedi= new SandwichCalorimeter();
	}
	if(!geometry.compare(0,cosmic.size(),cosmic)){
		jedi= new CosmicSetup();
	}

	if(!jedi)
		G4Exception("main","Geom001",FatalException,"No geometry chosen and no default geometry.");
	runManager->SetUserInitialization(jedi);

	// set physics list
	G4VModularPhysicsList* the_physics =new FTFP_BERT(0);//new QGSP_INCLXX();
	the_physics->SetVerboseLevel(0);
	the_physics->RegisterPhysics(new G4RadioactiveDecayPhysics);
	runManager->SetUserInitialization(the_physics);
	if(vm.count("detector.positions")){
		auto positions=vm["detector.positions"].as<std::vector<double> >();
		if(positions.size()%3){
			G4cout<<"Error: positions vector not valid."<<G4endl;
			for(auto i:positions){
				G4cout<<i<<" ";
			}
			G4cout<<G4endl;
			return 0;
		}
		for(size_t iPos=0; iPos<size_t(positions.size()/3);iPos+=3){
			G4cout<<"Position="<<positions[iPos]<<" "<<positions[iPos+1]<<" "<<positions[iPos+2]<<G4endl;
		}
		return 0;
	}
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



	if (!vm["general.batch_mode"].as<bool>())   // Define UI session for interactive mode
	{

#ifdef G4UI_USE
		G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
		G4cout<<"Interactive mode"<<G4endl;
		G4cout <<vm["general.macro_file"].as<std::string>()<<G4endl;
		std::stringstream o;
		o<<"/control/execute "<<vm["general.macro_file"].as<std::string>().c_str();
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
		if(vm.count("general.macro_file")){
			o<<"/control/execute "<<vm["general.macro_file"].as<std::string>().c_str();
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

