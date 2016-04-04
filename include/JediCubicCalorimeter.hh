/*
 * JediCubicPolarimeter.hh
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDICUBICCALORIMETER_HH_
#define INCLUDE_JEDICUBICCALORIMETER_HH_

#include <JediMainVolume.hh>
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


class JediCubicCalorimeter: public JediMainVolume {
public:
	JediCubicCalorimeter(std::string infile="");
	virtual G4VPhysicalVolume* Construct();
protected:

	void DefineCommands();
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4LogicalVolume* MakeDetector(G4String name, G4Material* mat,G4double width, G4double height, G4double length);
	virtual G4LogicalVolume* MakeDeltaECrystal();
	G4double distanceToEdge(G4double a, G4double b, G4ThreeVector direction);
	virtual ~JediCubicCalorimeter();

	void PlaceCalorimeter(G4LogicalVolume* aDetectorElement);
	void PlaceHodoscope(G4LogicalVolume* aDetectorElement);

	G4String fHodoscopeShape;
};


#endif /* INCLUDE_JEDICUBICCALORIMETER_HH_ */
