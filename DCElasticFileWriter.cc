#define FILEWRITER
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
#include "G4VUserActionInitialization.hh"
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "SingleCrystal.hh"

#include <FTFP_BERT.hh>

#include "FileWriterPrimaryGeneratorAction.hh"

#include <signal.h>
namespace CLHEP {}
using namespace CLHEP;


void Interrupt(int signum) { (G4RunManager::GetRunManager())->AbortRun() ; exit(1);}

class FileWriterActionInitialization: public G4VUserActionInitialization {
public:
	FileWriterActionInitialization(G4int nEvents,G4String filename){this->nEvents=nEvents;this->filename=filename;};
	virtual void Build() const{
		SetUserAction(new FileWriterPrimaryGeneratorAction(nEvents,filename));
	};
	virtual void BuildForMaster() const{
	};

private:
	G4int nEvents;
	G4String filename;
};


int main(int argc,char** argv) {

	signal(SIGTERM,&Interrupt) ;
	signal(SIGINT ,&Interrupt) ;
	signal(SIGPIPE,&Interrupt) ;


	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
																										("help,h", "Display this help message")
																										("general.config_file,c", po::value<std::string>(), "config file")
																										("general.num_threads,t", po::value<int>()->default_value(1), "number of worker threads")
																										("general.num_events,n", po::value<int>()->default_value(1), "number of events to be generated.")
																										("generator.beam_energy,e", po::value<double>()->default_value(270),"energy of beam in MeV"),
																										("generator.beam_polarization,p", po::value<double>()->default_value(1),"polarization of beam"),
																										("generator.output_file,o", po::value<std::string>()->default_value("events.dat"),"output file");


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

	runManager->SetVerboseLevel(0);

	//dummy geometry - not used
	runManager->SetUserInitialization(new SingleCrystal());

	// dummy physics list - no actual events are generated in g4
	FTFP_BERT* the_physics =new FTFP_BERT(0);
	runManager->SetUserInitialization(the_physics);
	//User action initialization
	runManager->SetUserInitialization(new FileWriterActionInitialization(vm["general.num_events"].as<int>(),G4String("events.dat")));
	// Initialize G4 kernel
	runManager->Initialize();
	runManager->BeamOn(vm["general.num_threads"].as<int>());

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	//                 owned and deleted by the run manager, so they should not
	//                 be deleted in the main() program !

	delete runManager;



	return 0;
}
