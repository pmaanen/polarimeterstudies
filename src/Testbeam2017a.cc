/*
 * Testbeam2017a.cc
 *
 *  Created on: 29.09.2017
 *      Author: pmaanen
 */

#include <Testbeam2017a.hh>

Testbeam2017a::Testbeam2017a():Testbeam2016b() {

	fNx=4;
	fNy=3;

}

G4VPhysicalVolume* Testbeam2017a::Construct() {

	if(fGeometryHasBeenChanged)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial(fWorldMaterialName),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	BuildSetup();
	BuildDetector();
	return fPhysiWorld;
}

void Testbeam2017a::BuildDetector() {


	auto logicRight=BuildScintillatorMatrix("Right");
	auto logicLeft=BuildScintillatorMatrix("Left");
	auto logicUp=BuildScintillatorMatrix("Up");
	auto logicDown=BuildScintillatorMatrix("Down");
	//auto trigVisAttr=new G4VisAttributes(cyan);

	auto rotLeftRight=new G4RotationMatrix();
	rotLeftRight->rotateZ(0*CLHEP::deg);

	rotLeftRight->rotateZ(fPhi);
	auto rotUpDown=new G4RotationMatrix();
	rotUpDown->rotateZ(90*CLHEP::deg);
	rotUpDown->rotateZ(fPhi);

	new G4PVPlacement(rotUpDown,G4ThreeVector(0,fDetectorHeight+fNx*fHCalSizeXY/2+fNy*fHCalSizeXY/2,fDistance+fHCalSizeZ/2).rotateZ(-fPhi),logicUp,"Up",fLogicWorld,0,0,false);
	new G4PVPlacement(rotUpDown,G4ThreeVector(0,fDetectorHeight-(fNx*fHCalSizeXY/2+fNy*fHCalSizeXY/2),fDistance+fHCalSizeZ/2).rotateZ(-fPhi),logicDown,"Down",fLogicWorld,0,0,false);

	new G4PVPlacement(rotLeftRight,G4ThreeVector(fNx*fHCalSizeXY/2+fNy*fHCalSizeXY/2,fDetectorHeight,fDistance+fHCalSizeZ/2).rotateZ(-fPhi),logicRight,"Right",fLogicWorld,0,0,false);
	new G4PVPlacement(rotLeftRight,G4ThreeVector(-(fNx*fHCalSizeXY/2+fNy*fHCalSizeXY/2),fDetectorHeight,fDistance+fHCalSizeZ/2).rotateZ(-fPhi),logicLeft,"Left",fLogicWorld,0,0,false);


}
