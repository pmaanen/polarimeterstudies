/*
 * JediBeamPipe.cc
 *
 *  Created on: 04.04.2016
 *      Author: pmaanen
 */

#include "JediBeamPipe.hh"
#include "Colors.hh"
#include <G4NistManager.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>



JediBeamPipe::JediBeamPipe(JediMainVolume* mainVol):fMainVol(mainVol),G4LogicalVolume(nullptr,nullptr,"Beampipe") {
	CopyValues();

	G4double windowThickness=0.1*CLHEP::mm;
	G4double foilThickness=0.1*CLHEP::mm;

	auto al=G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	auto mylar=G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR");
	auto uhv=G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

	G4Tubs* solidBeampipe=new G4Tubs("Beampipe",fBeampipeRadius-fBeampipeThickness,fBeampipeRadius,fWorldSizeZ/2-1*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicBeampipe = new G4LogicalVolume(solidBeampipe,al,"Beampipe");
	G4Tubs* solidMylarFoil=new G4Tubs("MylarFoil",fBeampipeRadius-fBeampipeThickness+windowThickness,fBeampipeRadius-fBeampipeThickness+foilThickness+windowThickness,(fTargetChamberZ2-fTargetChamberZ1)/2,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicMylarFoil=new G4LogicalVolume(solidMylarFoil,mylar,"MylarFoil");
	new G4PVPlacement(0,G4ThreeVector(0,0,fTargetChamberZ1+0.5*(fTargetChamberZ2-fTargetChamberZ1)),logicMylarFoil,"MylarFoil",logicBeampipe,0,false,0);
	G4Tubs* solidGap= new G4Tubs("Gap",fBeampipeRadius-fBeampipeThickness+foilThickness+windowThickness,fBeampipeRadius,(fTargetChamberZ2-fTargetChamberZ1)/2,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicGap=new G4LogicalVolume(solidGap,uhv,"Gap");
	new G4PVPlacement(0,G4ThreeVector(0,0,fTargetChamberZ1+0.5*(fTargetChamberZ2-fTargetChamberZ1)),logicGap,"Gap",logicBeampipe,0,false,0);

	logicGap->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* beampipeVisAttr = new G4VisAttributes(gray);
	logicBeampipe->SetVisAttributes(beampipeVisAttr);
}

JediBeamPipe::~JediBeamPipe() {
	// TODO Auto-generated destructor stub
}

void JediBeamPipe::CopyValues() {
	fMainVol->getThetaMax();
	fBuildTargetChamber=fMainVol->getBuildTargetChamber();
	fBeampipeRadius=fMainVol->getBeampipeRadius();
	fBeampipeThickness=fMainVol->getBeampipeThickness();
	fTargetChamberZ1=fMainVol->getTargetChamberZ1();
	fTargetChamberZ2=fMainVol->getTargetChamberZ2();
	fWorldSizeXY=fMainVol->getWorldSizeXy();
	fWorldSizeZ=fMainVol->getWorldSizeZ();
}
