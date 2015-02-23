/*
 * SingleCrystal.hh
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_SINGLECRYSTAL_HH_
#define INCLUDE_SINGLECRYSTAL_HH_

#include <JediPolarimeter.hh>

class SingleCrystal: public JediPolarimeter {
public:
	SingleCrystal();
	virtual ~SingleCrystal();


private:
	virtual void DefineCommands();
	G4String material;

	virtual G4LogicalVolume* MakeDetector();
	virtual G4VPhysicalVolume* Construct();

};
#endif
