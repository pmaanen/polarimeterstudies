/*
 * JediCubicPolarimeter.hh
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDICUBICPOLARIMETER_HH_
#define INCLUDE_JEDICUBICPOLARIMETER_HH_

#include "G4VUserDetectorConstruction.hh"
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


class JediHexagonalPolarimeter: public G4VUserDetectorConstruction {
public:
	JediHexagonalPolarimeter();
	void ConstructSDandField();
	G4VPhysicalVolume* Construct();
	void WriteWorldToFile(G4String filename);
	void UpdateGeometry();
private:

	void DefineCommands();
	void ComputeParameters();


	G4LogicalVolume* MakeDetector();
	G4LogicalVolume* MakeBeampipe();

	G4VPhysicalVolume* physiWorld;
	G4GenericMessenger* fMessenger;

	G4double thetaMin, thetaMax;
	G4double beampipeRadius, beampipeThickness;
	G4double crystalLength, crystalWidth;
	G4double innerDetectorRadius, outerDetectorRadius;
	G4double DetectorZ;

	G4bool changedParameters;
	G4int MinCrystal,MaxCrystal;

	virtual ~JediHexagonalPolarimeter();
};

#endif /* INCLUDE_JEDICUBICPOLARIMETER_HH_ */
