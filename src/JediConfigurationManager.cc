/*
 * JediConfigurationManager.cc
 *
 *  Created on: 05.03.2017
 *      Author: pmaanen
 */

#include <JediConfigurationManager.hh>
#include <fstream>
#include <iostream>
JediConfigurationManager* JediConfigurationManager::theInstance=nullptr;

JediConfigurationManager::JediConfigurationManager(int argc,char** argv){
	if(theInstance)
		G4Exception("JediConfigurationManager::JediConfigurationManager() ", "",
				FatalException, "JediConfigurationManager constructed twice");
	theInstance=this;

	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
		    		("help,h", "Display this help message")

					//General
					("general.output_file,o",po::value<std::string>(), "output filename")
					("general.verbosity,v", po::value<int>()->default_value(0), "verbosity")
					("general.config_file,c", po::value<std::string>()->default_value(".geantrc"), "config file")
					("general.physics", po::value<std::string>()->default_value("QGSP_BERT"), "physics list")
					("general.num_events,i", po::value<int>()->default_value(1), "number of events.")
					("general.num_threads,n", po::value<int>()->default_value(1), "number of threads.")
					("general.macro_file,m", po::value<std::string>(), "macro file")
					("general.merge_output", po::bool_switch()->default_value(true), "merge output files")

					//Random
					("random.seed", po::value<int>(), "random seed")
					("random.save_random", po::value<bool>()->default_value(false), "save random engine status")
					("random.luxury", po::value<int>()->default_value(3), "ranlux luxury")

					//Geometry
					("detector.geometry_file,g", po::value<std::string>()->default_value(""), "geometry file")
					("detector.positions", po::value<std::vector<double> >()->multitoken(),"positions")
					("detector.beampipe_radius", po::value<double>()->default_value(50),"beampipe radius in mm")
					("detector.beampipe_thickness", po::value<double>()->default_value(2),"beampipe thickness in mm")
					("detector.scintillator_material", po::value<std::string>()->default_value("LYSO"),"scintillator material name")
					("detector.hcal_size_z", po::value<double>()->default_value(80.),"hcal element length(z) in mm")
					("detector.hcal_size_xy", po::value<double>()->default_value(30.),"hcal element size(x,y)  in mm")
					("detector.theta_min", po::value<double>()->default_value(5),"acceptance theta min in deg")
					("detector.theta_max", po::value<double>()->default_value(20),"acceptance theta max in deg")

					//Generator
					("generator.theta_min", po::value<double>(),"theta min in deg")
					("generator.theta_max", po::value<double>(),"theta max in deg")
					("generator.generator,e", po::value<std::string>(), "generator name.")
					("generator.beam_particle", po::value<int>()->default_value(0), "PDG id of beam")
					("generator.target_particle", po::value<int>()->default_value(0), "PDG id of target")
					("generator.beam_energy", po::value<double>()->default_value(270),"energy of beam in MeV")
					("generator.beam_polarization", po::value<double>()->default_value(0),"polarization of beam")

					//Physics
					("physics.theta_min", po::value<double>(),"theta min in deg")
					("physics.theta_max", po::value<double>(),"theta max in deg")
					("physics.use_fast_sim", po::value<bool>()->default_value(false)->implicit_value(true),"use fast sim")
					;

	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), fConfig);
	if(fConfig.count("help")){
		std::cout<<description<<"\n";
		exit(1);
	}
	notify(fConfig);

	std::ifstream f(fConfig["general.config_file"].as<std::string>());
	if(f.good()){
		cfg.open(fConfig["general.config_file"].as<std::string>().c_str(),std::ifstream::in);
		po::store(po::parse_config_file(cfg, description), fConfig);
		notify(fConfig);
	}
	fVerbose=fConfig["general.verbosity"].as<int>();
}
