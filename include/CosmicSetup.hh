/*
 * CosmicSetup.hh
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_COSMICSETUP_HH_
#define INCLUDE_COSMICSETUP_HH_

#include <SingleCrystal.hh>

class CosmicSetup: public SingleCrystal {
public:
	CosmicSetup();
	virtual ~CosmicSetup();

protected:
	virtual G4VPhysicalVolume* Construct();
};

#endif /* INCLUDE_COSMICSETUP_HH_ */
