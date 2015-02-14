/*
 * JediCubicPolarimeter.hh
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIHEXAGONALPOLARIMETER_HH_
#define INCLUDE_JEDIHEXAGONALPOLARIMETER_HH_

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include <map>
#include <string>
#include "G4ThreeVector.hh"
#include "G4GDMLParser.hh"
#include "JediPolarimeter.hh"
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


class JediHexagonalPolarimeter: public JediPolarimeter {
public:
	JediHexagonalPolarimeter();
	void ConstructSDandField();
	G4VPhysicalVolume* Construct();

private:

	void DefineCommands();
	virtual G4LogicalVolume* MakeDetector();
	virtual void UpdateGeometry();
	virtual ~JediHexagonalPolarimeter();
};

#endif /* INCLUDE_JEDIHEXAGONALPOLARIMETER_HH_ */
