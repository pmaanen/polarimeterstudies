/*
 * JediTargetChamber.hh
 *
 *  Created on: 04.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDITARGETCHAMBER_HH_
#define INCLUDE_JEDITARGETCHAMBER_HH_
#include "JediMainVolume.hh"
#include <G4LogicalVolume.hh>
class JediTargetChamber: public G4LogicalVolume {
public:
	JediTargetChamber(JediMainVolume* mainVol);
	virtual ~JediTargetChamber();

private:
	void CopyValues();
	JediMainVolume* fMainVol;
	G4double fThetaMax,fBeampipeRadius, fBeampipeThickness,fTargetChamberZ1,fTargetChamberZ2,fWorldSizeXY,fWorldSizeZ;
};

#endif /* INCLUDE_JEDITARGETCHAMBER_HH_ */
