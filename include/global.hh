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
#ifdef MAIN
    #define EXTERN
  #else
    #define EXTERN extern
  #endif
EXTERN boost::program_options::variables_map vm;
#endif /* GLOBAL_HH_ */
