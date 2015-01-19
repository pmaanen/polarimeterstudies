#include "DCElasticEventGenerator.hh"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "G4ParticleDefinition.hh"
#include <G4Deuteron.hh>
#include "G4IonTable.hh"
#include "G4ios.hh"
#include "TCanvas.h"
#include "TH1.h"
#include "Randomize.hh"
#define MAIN
#include "global.hh"
int main(int argc, char** argv){

	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
																									("help,h", "Display this help message")
																									("energy,e", po::value<double>()->default_value(235),"energy of beam in MeV")
																									("polarization,p", po::value<double>()->default_value(1),"beam polarization")
																									("nevents,n", po::value<int>()->default_value(100),"number of events")
																									("xprime,x", po::value<double>()->default_value(0),"x' in mrad")
																									("yprime,p", po::value<double>()->default_value(0),"y' in mrad")
																									("general.config_file,c", po::value<std::string>(), "config file")
																									("general.num_threads,n", po::value<int>()->default_value(1), "number of threads.")
																									("general.macro_file,m", po::value<std::string>()->default_value("scripts/vis_T0.mac"), "macro file")
																									("general.batch_mode,b", po::bool_switch()->default_value(false), "batch mode")
																									("detector.geometry,g", po::value<std::string>()->default_value(""), "geometry file")
																									("generator.beam_particle,p", po::value<int>()->default_value(0), "PDG id of beam")
																									("generator.target_particle,t", po::value<int>()->default_value(0), "PDG id of target")
																									("generator.beam_energy,e", po::value<double>()->default_value(1),"energy of beam in MeV");


	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), vm);
	notify(vm);

	DCElasticEventGenerator dc(235*CLHEP::MeV);
	int nevents=vm["nevents"].as<int>();
	ParticleMomentumVector ievent;
	for(int ii=0;ii<nevents;ii++){
		ievent=dc.GenerateEvent();
		for(auto imom : ievent){
			G4cout<<imom.first->GetParticleName()<< " "<<imom.second<<G4endl;
		}
	}
	return 0;
}
