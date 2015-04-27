/*
 * SandwichCalorimeter.hh
 *
 *  Created on: 21.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_SANDWICHCALORIMETER_HH_
#define INCLUDE_SANDWICHCALORIMETER_HH_

#include <SingleCrystal.hh>

class SandwichCalorimeter: public SingleCrystal {
public:
	SandwichCalorimeter();
	virtual ~SandwichCalorimeter();

	virtual void DefineCommands();
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4VPhysicalVolume* Construct();
protected:
	G4Material* absorberMaterial;
};

#endif /* INCLUDE_SANDWICHCALORIMETER_HH_ */
