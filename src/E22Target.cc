/*
 * Target.cc
 *
 *  Created on: 20.10.2016
 *      Author: pmaanen
 */

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include "G4GenericMessenger.hh"
#include <assert.h>
#include <E22Target.hh>
#include "E22.hh"

E22Target::E22Target(G4RotationMatrix *pRot,
		const G4ThreeVector &tlate, G4LogicalVolume *pMotherLogical,
		G4bool pMany, G4int pCopyNo, JediPolarimeter* dc) :
		G4PVPlacement(pRot, tlate,
				new G4LogicalVolume(new G4Tubs("temp", 0, 1, 1, 1, 1.),
						G4Material::GetMaterial("G4_Galactic"), "temp", 0, 0, 0),
						"Target", pMotherLogical, pMany, pCopyNo),
						fPolarimeter(dc),fMaterial(nullptr),fShape(shape::kDisc),fSizeX(1*CLHEP::cm),fSizeY(1*CLHEP::cm),fSizeZ(1*CLHEP::cm){

	auto pol=dynamic_cast<E22*>(fPolarimeter);
	fSizeX=pol->getTargetSizeX();
	fSizeY=pol->getTargetSizeY();
	fSizeZ=pol->getTargetSizeZ();
	fMaterial=G4NistManager::Instance()->FindOrBuildMaterial(pol->getTargetMaterialName());

	switch (fShape){
	case shape::kBox:
		SetLogicalVolume(BuildBox());
		break;
	case shape::kDisc:
		SetLogicalVolume(BuildDisc());
		break;
	case shape::kSphere:
		SetLogicalVolume(BuildSphere());
		break;
	default:
		assert(false);
	}
	return;

}

E22Target::~E22Target() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* E22Target::BuildBox() {
	auto solidTarget=new G4Box("Target",fSizeX/2,fSizeY/2,fSizeZ/2);
	return new G4LogicalVolume(solidTarget,fMaterial,"Target");
}

G4LogicalVolume* E22Target::BuildDisc() {
	auto solidTarget=new G4Tubs("Target",0,fSizeX,fSizeZ,0,2*CLHEP::pi);
	return new G4LogicalVolume(solidTarget,fMaterial,"Target");

}

G4LogicalVolume* E22Target::BuildSphere() {
	auto solidTarget=new G4Sphere("Target",0,fSizeX,0,2*CLHEP::pi,0,CLHEP::pi);
	return new G4LogicalVolume(solidTarget,fMaterial,"Target");
}
