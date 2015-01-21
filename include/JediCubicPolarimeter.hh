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
	JediCubicPolarimeter();
	void ConstructSDandField();
	G4VPhysicalVolume* Construct();
	void DefineCommands();
	void UpdateGeometry();
protected:

	G4LogicalVolume* MakeDetector();
	virtual ~JediCubicPolarimeter();
};


#endif /* INCLUDE_JEDICUBICPOLARIMETER_HH_ */
