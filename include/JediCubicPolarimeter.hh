/*
 * JediCubicPolarimeter.hh
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDICUBICPOLARIMETER_HH_
#define INCLUDE_JEDICUBICPOLARIMETER_HH_

#include <JediConfigurationManager.hh>
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
	JediCubicPolarimeter(std::string infile="");
	virtual G4VPhysicalVolume* Construct();
protected:

	void DefineCommands();
	virtual G4LogicalVolume* BuildCaloCrystal();
	virtual G4LogicalVolume* MakeDeltaECrystal();
	virtual G4LogicalVolume* MakeDetector(G4String name, G4Material* mat,G4double halfSizeX, G4double halfSizeY, G4double halfSizeZ);
	G4double distanceToEdge(G4double sizeXY, G4ThreeVector direction);
	virtual ~JediCubicPolarimeter();

	void PlaceCalorimeter(G4LogicalVolume* aDetectorElement);
	void PlaceHodoscope(G4LogicalVolume* aDetectorElement);

	G4String fHodoscopeShape;
	G4VSolid* fDetectorSolid;

private:
	void ReadFromFile();
};


#endif /* INCLUDE_JEDICUBICPOLARIMETER_HH_ */
