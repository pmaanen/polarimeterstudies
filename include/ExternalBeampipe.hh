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
class G4LogicalVolume;
class ExternalBeampipe : public G4PVPlacement {
public:
	ExternalBeampipe(G4RotationMatrix *pRot, const G4ThreeVector &tlate,
			G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo,
			JediPolarimeter* dc);
	virtual ~ExternalBeampipe()=default;

	G4LogicalVolume* getExitWindow();
	G4LogicalVolume* getLateral();
private:

	G4double fRadius;
	JediPolarimeter* fPolarimeter;
	G4LogicalVolume *fExitWindow,*fLateral;
	void CopyValues();

};

inline G4LogicalVolume* ExternalBeampipe::getExitWindow(){
	return fExitWindow;
}

inline G4LogicalVolume* ExternalBeampipe::getLateral(){
	return fLateral;
}

#endif /* INCLUDE_EXTERNALBEAMPIPE_HH_ */
