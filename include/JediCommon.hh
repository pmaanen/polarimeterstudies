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
class SensitiveDetectorManager;
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
    po::store(po::parse_command_line(argc, argv, description), gConfig);
    if(gConfig.count("help")){
        std::cout<<description<<"\n";
        exit(1);
    }
    notify(gConfig);

    std::ifstream f(gConfig["general.config_file"].as<std::string>());
    if(f.good()){
        cfg.open(gConfig["general.config_file"].as<std::string>().c_str(),std::ifstream::in);
        po::store(po::parse_config_file(cfg, description), gConfig);
        notify(gConfig);
    }
    gVerbose=gConfig["general.verbosity"].as<int>();
}
#endif
#endif /* GLOBAL_HH_ */
