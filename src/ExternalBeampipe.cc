/*
 * ExternalBeampipe.cc
 *
 *  Created on: 14.09.2016
 *      Author: pmaanen
 */

#include <ExternalBeampipe.hh>
#include <G4NistManager.hh>
#include <G4Tubs.hh>
#include "JediPolarimeter.hh"
#include "VacuumChamber.hh"
#include "Colors.hh"
ExternalBeampipe::ExternalBeampipe(G4RotationMatrix *pRot, const G4ThreeVector &tlate,
		G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo,
		JediPolarimeter* dc):G4PVPlacement(pRot, tlate,
				new G4LogicalVolume(new G4Tubs("temp", 0, 1, 1, 1, 1.),
						G4Material::GetMaterial("G4_Galactic"), "temp", 0, 0, 0),
						"Beampipe", pMotherLogical, pMany, pCopyNo), fPolarimeter(dc) {


	auto al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	auto uhv = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
	auto iron = G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");

	auto solidBeampipe=new G4Tubs("Beampipe",0,5*CLHEP::cm,10*CLHEP::cm,0,2*CLHEP::pi*CLHEP::rad);
	auto logicBeampipe=new G4LogicalVolume(solidBeampipe,al,"Beampipe");

	auto windowThickness=0.1*CLHEP::mm;


	auto solidVacuum=new G4Tubs("Vacuum",0,5*CLHEP::cm-2*CLHEP::mm,10*CLHEP::cm-windowThickness,0,2*CLHEP::pi*CLHEP::rad);
	auto logicVacuum=new G4LogicalVolume(solidVacuum,uhv,"Vacuum");
	logicVacuum->SetVisAttributes(new G4VisAttributes(tyellow));
	new G4PVPlacement(0,G4ThreeVector(0,0,-windowThickness),logicVacuum,"Vacuum",logicBeampipe,false,0,false);

	auto solidExitWindow=new G4Tubs("exitWindow",0,5*CLHEP::cm-2*CLHEP::mm,windowThickness,0,2*CLHEP::pi*CLHEP::rad);
	auto logicExitWindow=new G4LogicalVolume(solidExitWindow,iron,"exitWindow");
	logicExitWindow->SetVisAttributes(new G4VisAttributes(tgray));
	new G4PVPlacement(0,G4ThreeVector(0,0,10*CLHEP::cm-windowThickness),logicExitWindow,"exitWindow",logicBeampipe,false,0,false);

	SetLogicalVolume(logicBeampipe);
}

ExternalBeampipe::~ExternalBeampipe() {
	// TODO Auto-generated destructor stub
}

