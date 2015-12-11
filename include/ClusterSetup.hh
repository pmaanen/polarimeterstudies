/*
 * ClusterSetup.hh
 *
 *  Created on: 10.12.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_CLUSTERSETUP_HH_
#define INCLUDE_CLUSTERSETUP_HH_

#include <SingleCrystal.hh>

class ClusterSetup: public SingleCrystal {
public:
	ClusterSetup();
	virtual G4VPhysicalVolume* Construct();
	virtual void DefineCommands(){};
	virtual ~ClusterSetup();
};

#endif /* INCLUDE_CLUSTERSETUP_HH_ */
