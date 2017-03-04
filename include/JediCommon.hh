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
#ifdef GLOBAL
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN boost::program_options::variables_map gConfig;
EXTERN int gVerbose;
/*
 * Verbosity
 * 0: Only fatal errors
 * 1: Errors
 * 2: Warning
 * 3: Info
 * 4: Debug
 */

#endif /* GLOBAL_HH_ */
