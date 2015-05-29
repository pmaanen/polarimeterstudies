/*
 * JediCubicPolarimeter.hh
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDICUBICPOLARIMETER_HH_
#define INCLUDE_JEDICUBICPOLARIMETER_HH_

#include "JediPolarimeter.hh"
#include "globals.hh"
#include <map>
#include <string>
#include "G4ThreeVector.hh"
#include "G4GDMLParser.hh"
#include "global.hh"
class SensitiveDetector;
class TrackerSensitiveDetector;
class G4MultiFunctionalDetector;
class G4VPrimitiveScorer;
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4MaterialPropertiesTable;
class G4GenericMessenger;


class JediCubicPolarimeter: public JediPolarimeter {
public:
	JediCubicPolarimeter(std::string infile="");
	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSDandField();
protected:

	void DefineCommands();
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4LogicalVolume* MakeDeltaECrystal();
	virtual ~JediCubicPolarimeter();
};


#endif /* INCLUDE_JEDICUBICPOLARIMETER_HH_ */
