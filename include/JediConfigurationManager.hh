/*
 * JediConfigurationManager.hh
 *
 *  Created on: 05.03.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDICONFIGURATIONMANAGER_HH_
#define INCLUDE_JEDICONFIGURATIONMANAGER_HH_
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "globals.hh"

class JediConfigurationManager {
public:
	JediConfigurationManager(int argc,char** argv);
	virtual ~JediConfigurationManager()=default;
	static JediConfigurationManager* Instance() {
		if(theInstance==nullptr)
			G4Exception(" JediConfigurationManager::Instance()","",FatalException,
					"Configuration manager must be explicitly instantiated ONCE in main!");
		return theInstance;
	}

	boost::program_options::variables_map& GetMap();
	G4int GetVerbose() const;
private:

	static JediConfigurationManager* theInstance;
	boost::program_options::variables_map fConfig;
	G4int fVerbose;
};


inline boost::program_options::variables_map& JediConfigurationManager::GetMap(){
	return fConfig;
}

inline G4int JediConfigurationManager::GetVerbose() const {
	return fVerbose;
}

#endif /* INCLUDE_JEDICONFIGURATIONMANAGER_HH_ */
