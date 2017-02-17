/*
 * Testbeam2016B.hh
 *
 *  Created on: 18.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_E22_HH_
#define INCLUDE_E22_HH_

#include <E21.hh>

class E22: public E21 {
public:
	E22();
	virtual ~E22()=default;

	virtual G4VPhysicalVolume* Construct();
	void MakeSetup();
	G4LogicalVolume* MakeScintillatorMatrix(G4String name);
	void Make2016BDetector();
	void MakeEffectiveDetector();
	void MakeSandwichDetector();
	void MakeTarget();
	virtual void DefineCommands();

	G4double getTargetSizeX() const;
	G4double getTargetSizeY() const;
	G4double getTargetSizeZ() const;
	const G4String& getTargetMaterialName() const;

protected:


	virtual void ComputeParameters();
	G4double fDistance,fArmWidth,fAngle,fDetectorHeight,fMinDistance,fTargetSizeX,
	fTargetSizeY,fTargetSizeZ,fVetoSizeXY,fVetoSizeZ,fHoleSizeXY,fApertureSize;
	G4int fNx,fNy;
	G4ThreeVector fAperturePosition;
	G4bool fBuildSupport,fRightDetector,fLeftDetector,fBuildTarget,fMonitor, fBeampipe, fTrigger, fVeto;
	std::unique_ptr<G4GenericMessenger> fTargetMessenger;
	G4String fTargetMaterialName;
};

inline G4double E22::getTargetSizeX() const {
	return fTargetSizeX;
}

inline G4double E22::getTargetSizeY() const {
	return fTargetSizeY;
}

inline G4double E22::getTargetSizeZ() const {
	return fTargetSizeZ;
}

inline const G4String& E22::getTargetMaterialName() const {
	return fTargetMaterialName;
}

#endif /* INCLUDE_E22_HH_ */
