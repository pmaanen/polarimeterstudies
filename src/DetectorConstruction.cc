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



#include "DetectorConstruction.hh"

//***** include basic geometry classes
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Para.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"
#include "G4CutTubs.hh"
#include "G4Torus.hh"
#include "G4Polyhedra.hh"
//***** End of include basic geometry classes

#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"
#include "G4RunManager.hh"
#include "TMath.h"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"


#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4SDManager.hh"
#include "CaloSensitiveDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"

#include "TrackerSensitiveDetector.hh"
#include "DetectorMessenger.hh"
#include "G4GDMLParser.hh"
#include "TNtuple.h"
#include "Analysis.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "global.hh"
using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:physiWorld(0),targetSize(0.001*mm),detSD(0)
{
	dcMessenger=new DetectorMessenger(this);
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
	// 	delete TargetMaterial;
}

void DetectorConstruction::UpdateGeometry()
{
	G4cout <<"Updating Geometry...";
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout<<"done!"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

	//Clean Up the Old Geometry

	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	// World
		G4Box* solidWorld=new G4Box("World",1*m,1*m,1*m);
		G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
		logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
		physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);
		G4Box* solidTarget=new G4Box("Target",targetSize/2,targetSize/2,targetSize/2);
		G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget,G4NistManager::Instance()->FindOrBuildMaterial("G4_C"),"logicTarget");
		new G4PVPlacement(0,G4ThreeVector(0,0,targetSize/2),logicTarget,"Target",logicWorld,0,0,0);

		G4Sphere* solidDet=new G4Sphere("solidDet",2*targetSize,2.01*targetSize,0*deg,360*deg,0*deg,179*deg);
		G4LogicalVolume* logicDet = new G4LogicalVolume(solidDet,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"logicDet");
		new G4PVPlacement(0,G4ThreeVector(0,0,targetSize/2),logicDet,"Det",logicWorld,0,0,0);


	//------------------------------------------------
	// Sensitive detectors
	//------------------------------------------------

	G4SDManager* man=G4SDManager::GetSDMpointer();
	if(detSD!=0)
		delete detSD;
	detSD=new TrackerSensitiveDetector("tracker","trackercollection");
	logicDet->SetSensitiveDetector(detSD);
	man->AddNewDetector(detSD);
	return physiWorld;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
