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

#ifndef EddaDetectorConstruction_h
#define EddaDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"


class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4MaterialPropertiesTable;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EddaDetectorConstruction : public G4VUserDetectorConstruction
{
	public:
		EddaDetectorConstruction();
		~EddaDetectorConstruction();

	G4VPhysicalVolume* Construct();

	private:
	//***********  World

	G4LogicalVolume* logicWorld;
	G4VPhysicalVolume* physiWorld;
		
	// Edda
	G4LogicalVolume* LogicalEdda;
	G4VPhysicalVolume* PhysicalEdda;


	// Barrel1
	G4LogicalVolume* LogicalBarrel_1;
	G4VPhysicalVolume* PhysicalBarrel_1;

	G4LogicalVolume* LogicalTorus1;
	G4LogicalVolume* LogicalTubs1;

	// Barrel2
	G4LogicalVolume* LogicalBarrel_2;
	G4VPhysicalVolume* PhysicalBarrel_2;

	G4LogicalVolume* LogicalTorus2;
	G4LogicalVolume* LogicalTubs2;


	// Fibers
	G4LogicalVolume* LogicalTube;
	G4VPhysicalVolume* PhysicalTube;
	
	// SiPms
	G4LogicalVolume* LogicalSiPM;
	G4VPhysicalVolume* PhysicalSiPM;

	//Target
	G4LogicalVolume* LogicalTarget;
	G4VPhysicalVolume* PhysicalTarget;

	//***********  Material
	G4Material* Vacuum;
	G4Material* pScint;
	G4Material* Air;
	G4Material* PMMA;
	G4Material* Pstyrene;
	G4Material* Pethylene;
	G4Material* fPethylene;
	G4Material* SiPMMaterial;
	G4Material* MirrorMaterial;
	G4Material* TargetMaterial;

	//*********** Material properties
	G4MaterialPropertiesTable* MPTPStyrene;

	private:
		void DefineMaterials();
		void DefineMaterialProperties();
		G4VPhysicalVolume* ConstructSetUp();
		G4LogicalVolume* ConstructFiber(G4double Rmax, G4double Lz, G4double RCladding);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
