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
class SensitiveDetectorMap;
EXTERN int gVerbose;
/*
 * Verbosity
 * 0: Only fatal errors
 * 1: Errors
 * 2: Warning
 * 3: Info
 * 4: Debug
 */
#ifdef MAIN
void initializeConfiguration(int argc,char** argv){
    namespace po = boost::program_options;
    po::options_description description("Usage");
    description.add_options()
    ("help,h", "Display this help message")

	//General
	("general.output_file,o",po::value<std::string>(), "output filename")
	("general.verbosity,v", po::value<int>()->default_value(0), "verbosity")
    ("general.config_file,c", po::value<std::string>(), "config file")
    ("general.physics", po::value<std::string>()->default_value("QGSP_BERT"), "physics list")
    ("general.num_events,i", po::value<int>()->default_value(1), "number of events.")
	("general.num_threads,n", po::value<int>()->default_value(1), "number of threads.")
    ("general.macro_file,m", po::value<std::string>(), "macro file")
    ("general.batch_mode,b", po::bool_switch()->default_value(false), "batch mode")
    ("general.seed", po::value<int>(), "random seed")
    ("no-merge-output", po::bool_switch()->default_value(false), "merge output files")

	//Geometry
	("detector.geometry,g", po::value<std::string>()->default_value(""), "geometry file")
    ("detector.positions", po::value<std::vector<double> >()->multitoken(),"positions")
    ("detector.beampipeRadius", po::value<double>()->default_value(50),"beampipe radius in mm")
    ("detector.beampipeThickness", po::value<double>()->default_value(2),"beampipe thickness in mm")
    ("detector.scintillatorMaterial", po::value<std::string>()->default_value("LYSO"),"scintillator material name")
    ("detector.crystalLength", po::value<double>()->default_value(100),"crystal length(z) in mm")
    ("detector.crystalWidth", po::value<double>()->default_value(30),"crystal width(x,y)  in mm")
    ("detector.thetamin", po::value<double>()->default_value(5),"theta min in deg")
    ("detector.thetamax", po::value<double>()->default_value(19.5),"theta max in deg")

	//Generator
    ("generator.thetamin", po::value<double>(),"theta min in deg")
    ("generator.thetamax", po::value<double>(),"theta max in deg")
    ("generator.generator,e", po::value<std::string>(), "generator name.")
    ("generator.beam_particle", po::value<int>()->default_value(0), "PDG id of beam")
    ("generator.target_particle", po::value<int>()->default_value(0), "PDG id of target")
    ("generator.beam_energy", po::value<double>()->default_value(270),"energy of beam in MeV")
	("generator.beam_polarization", po::value<double>()->default_value(0),"energy of beam in MeV")
;

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
    gVerbose=gConfig["general.verbosity"].as<int>();
}
#endif
#endif /* GLOBAL_HH_ */
