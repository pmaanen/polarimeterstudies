/*
 * SandwichCalorimeter.hh
 *
 *  Created on: 21.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISANDWICHCALORIMETER_HH_
#define INCLUDE_JEDISANDWICHCALORIMETER_HH_

#include <SingleCrystal.hh>
#include "JediCubicPolarimeter.hh"
class JediSandwichCalorimeter: public JediCubicPolarimeter {
public:
	JediSandwichCalorimeter();
	virtual ~JediSandwichCalorimeter();

	virtual void DefineCommands();
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4VPhysicalVolume* Construct();
protected:
	G4Material* fAbsorberMaterial;
};

#endif /* INCLUDE_JEDISANDWICHCALORIMETER_HH_ */
