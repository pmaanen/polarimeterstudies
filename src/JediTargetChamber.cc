/*
 * JediTargetChamber.cc
 *
 *  Created on: 04.04.2016
 *      Author: pmaanen
 */

#include "JediTargetChamber.hh"
#include <G4UnionSolid.hh>
JediTargetChamber::JediTargetChamber(JediMainVolume* mainVol):fMainVol(mainVol),G4LogicalVolume(nullptr,nullptr,"TargetChamber") {
	CopyValues();
	auto al=G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	//Do it with Cons+Tube
	auto windowRadius=fTargetChamberZ2*tan( fThetaMax );
	auto exitWindowThickness=2*CLHEP::mm;
	G4Tubs* solidExitWindow=new G4Tubs("ExitWindow",fBeampipeRadius,windowRadius,exitWindowThickness/2,0,360*CLHEP::deg);
	G4double rInner1=fBeampipeRadius;
	G4double rOuter1=fBeampipeRadius+fBeampipeThickness;
	G4double rInner2=windowRadius;
	G4double rOuter2=windowRadius+fBeampipeThickness;
	G4Cons* solidConicalSection=new G4Cons("ConicalSection",rInner1,rOuter1,rInner2,rOuter2,(fTargetChamberZ2-fTargetChamberZ1)/2,0,360*CLHEP::deg);
	this->SetSolid(new G4UnionSolid("TargetChamber",solidConicalSection,solidExitWindow,0,G4ThreeVector(0,0,(fTargetChamberZ2-fTargetChamberZ1)/2)));
	SetMaterial(al);
	//G4LogicalVolume* logicTargetChamber=new G4LogicalVolume(solidTargetChamber,al,"TargetChamber");
}

JediTargetChamber::~JediTargetChamber() {
	// TODO Auto-generated destructor stub
}

void JediTargetChamber::CopyValues() {
	fThetaMax=fMainVol->getThetaMax();
	fBeampipeRadius=fMainVol->getBeampipeRadius();
	fBeampipeThickness=fMainVol->getBeampipeThickness();
	fTargetChamberZ1=fMainVol->getTargetChamberZ1();
	fTargetChamberZ2=fMainVol->getTargetChamberZ2();
	fWorldSizeXY=fMainVol->getWorldSizeXy();
	fWorldSizeZ=fMainVol->getWorldSizeZ();
}
