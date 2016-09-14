/*
 * ExternalBeampipe.hh
 *
 *  Created on: 14.09.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_EXTERNALBEAMPIPE_HH_
#define INCLUDE_EXTERNALBEAMPIPE_HH_
#include <G4PVPlacement.hh>
class JediPolarimeter;
class ExternalBeampipe : public G4PVPlacement {
public:
	ExternalBeampipe(G4RotationMatrix *pRot, const G4ThreeVector &tlate,
			G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo,
			JediPolarimeter* dc);
	virtual ~ExternalBeampipe();

private:

	JediPolarimeter* fPolarimeter;
	void CopyValues();
};

#endif /* INCLUDE_EXTERNALBEAMPIPE_HH_ */
