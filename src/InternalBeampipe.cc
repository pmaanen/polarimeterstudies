/*
 * InternalBeampipe.cc
 *
 *  Created on: 07.09.2016
 *      Author: pmaanen
 */

#include <InternalBeampipe.hh>
#include <G4NistManager.hh>
#include <G4Tubs.hh>
#include "JediPolarimeter.hh"
#include "VacuumChamber.hh"
#include "Colors.hh"
InternalBeampipe::InternalBeampipe(G4RotationMatrix *pRot,
		const G4ThreeVector &tlate, G4LogicalVolume *pMotherLogical,
		G4bool pMany, G4int pCopyNo, JediPolarimeter* dc,
		G4bool BuildVacuumChamber) :
		G4PVPlacement(pRot, tlate,
				new G4LogicalVolume(new G4Tubs("temp", 0, 1, 1, 1, 1.),
						G4Material::GetMaterial("G4_Galactic"), "temp", 0, 0, 0),
				"Beampipe", pMotherLogical, pMany, pCopyNo), fBuildVacuumChamber(
				BuildVacuumChamber),fPolarimeter(dc) {

	CopyValues();
	auto al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	auto mylar = G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR");
	auto uhv = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

	auto solidBeampipe = new G4Tubs("Beampipe", fRadius - fThickness, fRadius,
			fLength/2, 0 * CLHEP::deg, 360 * CLHEP::deg);
	auto logicBeampipe = new G4LogicalVolume(solidBeampipe, al, "Beampipe");

	if (fBuildVacuumChamber) {
		G4double windowThickness = 0.1 * CLHEP::mm;
		G4double foilThickness = 0.1 * CLHEP::mm;
		new VacuumChamber(0,
				G4ThreeVector(0, 0,
						fTargetChamberZ1
								+ 0.5 * (fTargetChamberZ2 - fTargetChamberZ1)),
				this->GetMotherLogical(), false, 0, fPolarimeter);
		G4Tubs* solidMylarFoil = new G4Tubs("MylarFoil",
				fRadius - fThickness + windowThickness,
				fRadius - fThickness + foilThickness + windowThickness,
				(fTargetChamberZ2 - fTargetChamberZ1) / 2, 0 * CLHEP::deg,
				360 * CLHEP::deg);
		G4LogicalVolume* logicMylarFoil = new G4LogicalVolume(solidMylarFoil,
				mylar, "MylarFoil");
		new G4PVPlacement(0,
				G4ThreeVector(0, 0,
						fTargetChamberZ1
								+ 0.5 * (fTargetChamberZ2 - fTargetChamberZ1)),
				logicMylarFoil, "MylarFoil", logicBeampipe, 0, false, 0);
		G4Tubs* solidGap = new G4Tubs("Gap",
				fRadius - fThickness + foilThickness + windowThickness, fRadius,
				(fTargetChamberZ2 - fTargetChamberZ1) / 2, 0 * CLHEP::deg,
				360 * CLHEP::deg);
		G4LogicalVolume* logicGap = new G4LogicalVolume(solidGap, uhv, "Gap");
		new G4PVPlacement(0,
				G4ThreeVector(0, 0,
						fTargetChamberZ1
								+ 0.5 * (fTargetChamberZ2 - fTargetChamberZ1)),
				logicGap, "Gap", logicBeampipe, 0, false, 0);
		logicGap->SetVisAttributes(G4VisAttributes::Invisible);

		G4VisAttributes* beampipeVisAttr = new G4VisAttributes(gray);
		logicBeampipe->SetVisAttributes(beampipeVisAttr);
	}
	SetLogicalVolume(logicBeampipe);
}

InternalBeampipe::~InternalBeampipe() {
	// TODO Auto-generated destructor stub
}

void InternalBeampipe::CopyValues() {
	fRadius = fPolarimeter->getBeampipeRadius();
	fLength = fPolarimeter->getBeampipeLength();
	fThickness = fPolarimeter->getBeampipeThickness();
	auto DetectorZ = (fRadius + fPolarimeter->getSafetyDistance())
			/ tan(fPolarimeter->getThetaMin());
	fTargetChamberZ1 = fRadius / tan(fPolarimeter->getThetaMax())
			- 1 * CLHEP::cm;
	fTargetChamberZ2 = DetectorZ - fPolarimeter->getDeltaELength()
			- 1 * CLHEP::cm;
}
