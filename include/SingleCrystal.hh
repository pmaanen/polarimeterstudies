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
	void ConstructSDandField();

private:
	virtual void DefineCommands();
	G4String material;
	static G4ThreadLocal TrackerSensitiveDetector* trkSD;
	virtual G4LogicalVolume* MakeDetector();
	virtual G4VPhysicalVolume* Construct();
	virtual void UpdateGeometry(){G4RunManager::GetRunManager()->ReinitializeGeometry(false);};
};
#endif
