/*
 * InternalBeampipe.hh
 *
 *  Created on: 07.09.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_INTERNALBEAMPIPE_HH_
#define INCLUDE_INTERNALBEAMPIPE_HH_

#include <G4PVPlacement.hh>
class JediPolarimeter;
class InternalBeampipe: public G4PVPlacement {
public:
	InternalBeampipe(G4RotationMatrix *pRot, const G4ThreeVector &tlate,
			G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo,
			JediPolarimeter* dc, G4bool BuildVacuumChamber = true);

	virtual ~InternalBeampipe();

private:

	void CopyValues();
	G4double fRadius, fThickness, fLength, fTargetChamberZ2, fTargetChamberZ1;

	G4bool fBuildVacuumChamber;

	JediPolarimeter* fPolarimeter;
};
#endif /* INCLUDE_INTERNALBEAMPIPE_HH_ */

