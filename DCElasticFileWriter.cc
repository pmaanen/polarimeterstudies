
#define FILEWRITER
#define MAIN
#include "global.hh"
#include "DetectorConstruction.hh"
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

#include <FileWriterPrimaryGeneratorAction.hh>

#include <signal.h>
#include <exception>
namespace CLHEP {}
using namespace CLHEP;


void Interrupt(int signum) { (G4RunManager::GetRunManager())->AbortRun() ; exit(1);}

class FileWriterActionInitialization: public G4VUserActionInitialization {
public:
	FileWriterActionInitialization(G4int nEvents,G4String generator="",G4String filename=""){this->nEvents=nEvents;this->filename=filename;this->generator=generator;};
	virtual void Build() const{
		SetUserAction(new FileWriterPrimaryGeneratorAction(nEvents,generator,filename));
	};
	virtual void BuildForMaster() const{
	};

private:
	G4int nEvents;
	G4String filename;
	G4String generator;
};

class DummyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
	virtual G4VPhysicalVolume* Construct(){
		auto solidWorld=new G4Box("World",1*CLHEP::cm,1*CLHEP::cm,1*CLHEP::cm);
		auto logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
		return new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);
	};
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
("generator.generator,g", po::value<std::string>(), "generator name")
("general.num_events,n", po::value<int>()->default_value(1), "number of events to be generated.")
("generator.beam_energy,e", po::value<double>()->default_value(270),"energy of beam in MeV")
("generator.beam_polarization,p", po::value<double>()->default_value(1),"polarization of beam")
("generator.output_file,o", po::value<std::string>(),"output file"),
("general.macro_file,m", po::value<std::string>(), "macro file");


	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), gConfig);
	notify(gConfig);
	if(gConfig.count("general.config_file")){
		cfg.open(gConfig["general.config_file"].as<std::string>().c_str(),std::ifstream::in);
		po::store(po::parse_config_file(cfg, description), gConfig);
		notify(gConfig);
	}
	// choose the Random engine
	RanecuEngine* theEngine=new RanecuEngine;
	if(argc>2){
		theEngine->setSeed(strtol(argv[2], NULL, 10));
	}
	HepRandom::setTheEngine(theEngine);
#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(gConfig["general.num_threads"].as<int>());
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	runManager->SetVerboseLevel(0);

	//dummy geometry - not used
	runManager->SetUserInitialization(new DummyDetectorConstruction);

	// dummy physics list - no actual events are generated in g4
	FTFP_BERT* the_physics =new FTFP_BERT(0);
	runManager->SetUserInitialization(the_physics);
	//User action initialization
	if(gConfig.count("generator.output_file"))
		runManager->SetUserInitialization(
				new FileWriterActionInitialization(gConfig["general.num_events"].as<int>(),
						G4String(gConfig["generator.generator"].as<std::string>()),
						G4String(gConfig["generator.output_file"].as<std::string>()) )
		);
	else
		runManager->SetUserInitialization(new FileWriterActionInitialization(gConfig["general.num_events"].as<int>(),G4String(gConfig["generator.generator"].as<std::string>())));
	// Initialize G4 kernel
	runManager->Initialize();
	auto UImanager = G4UImanager::GetUIpointer();
	std::stringstream o;
	if(gConfig.count("general.macro_file")){
		o<<"/control/execute "<<gConfig["general.macro_file"].as<std::string>().c_str();
		UImanager->ApplyCommand(o.str().c_str());
	}
	runManager->BeamOn(gConfig["general.num_threads"].as<int>());

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	//                 owned and deleted by the run manager, so they should not
	//                 be deleted in the main() program !

	delete runManager;


	return 0;
}
