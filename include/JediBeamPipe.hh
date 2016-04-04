/*
 * JediBeamPipe.hh
 *
 *  Created on: 04.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIBEAMPIPE_HH_
#define INCLUDE_JEDIBEAMPIPE_HH_

#include "JediMainVolume.hh"
#include <G4LogicalVolume.hh>

class JediBeamPipe: public G4LogicalVolume {
public:
	JediBeamPipe(JediMainVolume* mainVol);
	virtual ~JediBeamPipe();

private:
	void CopyValues();
	JediMainVolume* fMainVol;
	G4double fBeampipeRadius, fBeampipeThickness,fTargetChamberZ1,fTargetChamberZ2,fWorldSizeXY,fWorldSizeZ;
	G4bool fBuildTargetChamber;

};

#endif /* INCLUDE_JEDIBEAMPIPE_HH_ */
