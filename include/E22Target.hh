/*
 * Target.hh
 *
 *  Created on: 20.10.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_E22TARGET_HH_
#define INCLUDE_E22TARGET_HH_
#include "JediPolarimeter.hh"
#include <G4PVPlacement.hh>
#include <memory>
class G4GenericMessenger;
class E22Target: public G4PVPlacement {
public:
	E22Target(G4RotationMatrix *pRot, const G4ThreeVector &tlate,
			G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo,
			JediPolarimeter* dc);

	virtual ~E22Target();
private:

	enum class shape{kDisc,kSphere,kBox} fShape;
	G4Material* fMaterial;
	G4String fMaterialName;
	JediPolarimeter* fPolarimeter;
	G4double fSizeX,fSizeY,fSizeZ;

	G4LogicalVolume* BuildBox();
	G4LogicalVolume* BuildDisc();
	G4LogicalVolume* BuildSphere();


};

#endif /* INCLUDE_E22TARGET_HH_ */
