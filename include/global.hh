/*
 * global.hh *
 *  Created on: 29.10.2014
 *      Author: pmaanen
 */

#ifndef GLOBAL_HH_
#define GLOBAL_HH_
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <fstream>
#include <iostream>
#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN boost::program_options::variables_map gConfig;
#ifdef MAIN
void initializeConfiguration(int argc,char** argv){
	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
			("help,h", "Display this help message")
			("general.config_file,c", po::value<std::string>(), "config file")
			("general.num_threads,n", po::value<int>()->default_value(1), "number of threads.")
			("general.macro_file,m", po::value<std::string>()->default_value("scripts/vis_T0.mac"), "macro file")
			("general.batch_mode,b", po::bool_switch()->default_value(false), "batch mode")
			("general.seed,s", po::value<int>(), "random seed")
			("detector.geometry,g", po::value<std::string>()->default_value(""), "geometry file")
			("generator.beam_particle,p", po::value<int>()->default_value(0), "PDG id of beam")
			("generator.target_particle,t", po::value<int>()->default_value(0), "PDG id of target")
			("generator.beam_energy,e", po::value<double>()->default_value(270),"energy of beam in MeV")
			("detector.positions", po::value<std::vector<double> >()->multitoken(),"positions")
			("detector.beampipeRadius", po::value<double>()->default_value(50),"beampipe radius in mm")
			("detector.beampipeThickness", po::value<double>()->default_value(2),"beampipe thickness in mm")
			("detector.scintillatorMaterial", po::value<std::string>()->default_value("LYSO"),"scintillator material name")
			("detector.crystalLength", po::value<double>()->default_value(100),"crystal length(z) in mm")
			("detector.crystalWidth", po::value<double>()->default_value(30),"crystal width(x,y)  in mm")
			("detector.thetamin", po::value<double>()->default_value(5),"theta min in deg")
			("detector.thetamax", po::value<double>()->default_value(20),"theta max in deg");

	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), gConfig);
	if(gConfig.count("help")){
		std::cout<<description<<"\n";
		exit(1);
	}
	notify(gConfig);
	if(gConfig.count("general.config_file")){
		cfg.open(gConfig["general.config_file"].as<std::string>().c_str(),std::ifstream::in);
		po::store(po::parse_config_file(cfg, description), gConfig);
		notify(gConfig);
	}
}
#endif
#endif /* GLOBAL_HH_ */
