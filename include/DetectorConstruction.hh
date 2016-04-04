// ********************************************************************
// *                                                                  *
// *                          EDM Polarimetry                         *
// *                                                                  *
// * Detector physics                                                 *
// * RWTH Aachen 24.10.2012                                           *
// * Fabian Hinder                                                    *
// ********************************************************************
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef polarimeterStudiesDetectorConstruction_h
#define polarimeterStudiesDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "global.hh"
#include "JediPolarimeter.hh"
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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
	DetectorConstruction();
	virtual ~DetectorConstruction();

	virtual G4VPhysicalVolume* Construct(){return fGeometry->Construct();};
	virtual void ConstructSDandField() {
		fGeometry->ConstructSDandField();
	}
	/*
	void setGeometryName(const std::string& geometryName) {
		fGeometryName = geometryName;
		InitializeGeometry();
		auto geom=dynamic_cast<JediPolarimeter*>(fGeometry);
		if(geom)
			geom->UpdateGeometry();
	}
	*/

	;
private:
	G4VUserDetectorConstruction* fGeometry;
	std::string fGeometryName;
	G4GenericMessenger* fMessenger;
	void InitializeGeometry();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
