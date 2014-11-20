/*
 * StripROGeometry.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: maanen
 */

#include "StripROGeometry.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "DummySD.hh"
#include "SensitiveDetector.hh"
StripROGeometry::StripROGeometry() {
	// TODO Auto-generated constructor stub

}

StripROGeometry::~StripROGeometry() {
	// TODO Auto-generated destructor stub
}

StripROGeometry::StripROGeometry(G4String aString,G4double length,G4double width ,G4double thickness, G4double pitch):G4VReadOutGeometry(aString),Length(length),Width(width),Thickness(thickness),Pitch(pitch) {
	G4RunManager* runManager = G4RunManager::GetRunManager();
	detector =( DetectorConstruction*)(runManager->GetUserDetectorConstruction());
}
G4VPhysicalVolume* StripROGeometry::Build(){

	G4Box* solidPlane=new G4Box("Plane",Thickness/2,Length/2,Width/2);
	G4Material* dummyMat  = new G4Material(name="dummyMat", 1., 1.*g/mole, 1.*g/cm3);
	G4LogicalVolume* logicPlane=new G4LogicalVolume(solidPlane,dummyMat, "SiliconPlane");
	G4PVPlacement* ROWorldPhys = new G4PVPlacement(0,G4ThreeVector(),"ROWorldPhysical",	logicPlane,0,false,0);
	G4double StripX=Thickness;
	G4double StripY=Pitch;
	G4double StripZ=Length;
	G4Box* solidFiberY= new G4Box("Strip",StripX/2,StripY/2,StripZ/2);
	G4int nStripsY=floor(Width/Pitch);
	G4LogicalVolume* logicStripY=new G4LogicalVolume(solidFiberY,dummyMat,"logicSiliconY");
	for(G4int i=0;i<nStripsY;i++){
		new G4PVPlacement(0,G4ThreeVector(0,i*StripY-(nStripsY*StripY)/2,0),logicStripY,"Silicon",logicPlane,false,i);
	}

	DummySD* dummySD=new DummySD("Orpo");
	logicStripY->SetSensitiveDetector(dummySD);

	return ROWorldPhys;
}
