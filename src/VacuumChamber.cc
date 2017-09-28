/*
 * VacuumChamber.cc
 *
 *  Created on: 14.09.2016
 *      Author: pmaanen
 */

#include <VacuumChamber.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4UnionSolid.hh>
#include "JediPolarimeter.hh"
VacuumChamber::VacuumChamber(G4RotationMatrix *pRot,
		const G4ThreeVector &tlate,
		G4LogicalVolume *pMotherLogical,
		G4bool pMany,
		G4int pCopyNo,
		JediPolarimeter* dc):G4PVPlacement(pRot,tlate,new G4LogicalVolume(new G4Box("temp",1,1,1),G4Material::GetMaterial("Vacuum"),"temp",0,0,0),"VacuumChamber",pMotherLogical,pMany,pCopyNo),fPolarimeter(dc)
{


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
	G4UnionSolid* solidTargetChamber= new G4UnionSolid("TargetChamber",solidConicalSection,solidExitWindow,0,G4ThreeVector(0,0,(fTargetChamberZ2-fTargetChamberZ1)/2));
	G4LogicalVolume* logicTargetChamber=new G4LogicalVolume(solidTargetChamber,al,"TargetChamber");
	SetLogicalVolume(logicTargetChamber);

}

VacuumChamber::~VacuumChamber() {
	// TODO Auto-generated destructor stub
}

void VacuumChamber::CopyValues() {

	fThetaMax=fPolarimeter->getThetaMax();
	fBeampipeRadius=fPolarimeter->getBeampipeRadius();
	fBeampipeThickness=fPolarimeter->getBeampipeThickness();
	auto DetectorZ = (fBeampipeRadius+fPolarimeter->getSafetyDistance()) / tan( fPolarimeter->getThetaMin() );
	fTargetChamberZ1=fBeampipeRadius/ tan(fPolarimeter->getThetaMax())-1*CLHEP::cm;
	fTargetChamberZ2=DetectorZ-fPolarimeter->getDeltaELength()-1*CLHEP::cm;
}
