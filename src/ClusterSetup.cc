/*
 * ClusterSetup.cc
 *
 *  Created on: 10.12.2015
 *      Author: pmaanen
 */

#include <ClusterSetup.hh>

ClusterSetup::ClusterSetup() {
	// TODO Auto-generated constructor stub

}

G4VPhysicalVolume* ClusterSetup::Construct() {
	if(fChangedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	G4LogicalVolume* aCrystal=MakeCaloCrystal();

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	G4int iCrystal=0;
for(int x=-1;x<2;x++){
	for(int y=-1;y<2;y++){
		new G4PVPlacement (rot, G4ThreeVector(x*fCrystalWidth,y*fCrystalWidth,fCrystalLength/2), aCrystal, "Crystal", fLogicWorld, false, iCrystal++, false);
	}
}
	return fPhysiWorld;
}

ClusterSetup::~ClusterSetup() {
	// TODO Auto-generated destructor stub
}

