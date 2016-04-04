/*
 * JediHodoscope.hh
 *
 *  Created on: 04.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIHODOSCOPE_HH_
#define INCLUDE_JEDIHODOSCOPE_HH_

#include <G4LogicalVolume.hh>

class JediHodoscope: public G4LogicalVolume {
public:
	JediHodoscope();
	virtual ~JediHodoscope();
};

#endif /* INCLUDE_JEDIHODOSCOPE_HH_ */
