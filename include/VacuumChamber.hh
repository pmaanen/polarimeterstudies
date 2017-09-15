/*
 * VacuumChamber.hh
 *
 *  Created on: 14.09.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_VACUUMCHAMBER_HH_
#define INCLUDE_VACUUMCHAMBER_HH_

#include <G4PVPlacement.hh>
class JediPolarimeter;
class VacuumChamber: public G4PVPlacement {
public:
	VacuumChamber(G4RotationMatrix *pRot,
			const G4ThreeVector &tlate,
			G4LogicalVolume *pMotherLogical,
			G4bool pMany,
			G4int pCopyNo,
			JediPolarimeter* dc);



	virtual ~VacuumChamber();

private:
	void CopyValues();

	G4double fBeampipeRadius,fBeampipeThickness,fTargetChamberZ2,fThetaMax,fTargetChamberZ1;
	JediPolarimeter* fPolarimeter;
};

#endif /* INCLUDE_VACUUMCHAMBER_HH_ */
