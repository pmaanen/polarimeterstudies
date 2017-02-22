/*
 * Testbeam2016B.hh
 *
 *  Created on: 18.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_TESTBEAM2016B_HH_
#define INCLUDE_TESTBEAM2016B_HH_

#include <Testbeam2016a.hh>

class Testbeam2016b: public Testbeam2016a {
public:
	Testbeam2016b();
	virtual ~Testbeam2016b()=default;

	virtual G4VPhysicalVolume* Construct();

	G4double getTargetSizeX() const;
	G4double getTargetSizeY() const;
	G4double getTargetSizeZ() const;
	const G4String& getTargetMaterialName() const;

protected:

	virtual G4LogicalVolume* BuildCaloCrystal(G4String detName);
	virtual void ComputeParameters();

	G4double fDistance,fArmWidth,fAngle,fAngleLeft,fAngleRight,fDetectorHeight,fMinDistance,fTargetSizeX,
	fTargetSizeY,fTargetSizeZ,fVetoSizeXY,fVetoSizeZ,fHoleSizeXY,fApertureSize,fTargetDistance;
	G4int fNx,fNy;
	G4ThreeVector fCollimatorPosition;
	G4bool fBuildSupport,fRightDetector,fLeftDetector,fBuildTarget,fMonitor,
	fBeampipe, fTrigger, fVeto,fStart, fCollimator;
	std::unique_ptr<G4GenericMessenger> fTargetMessenger;
	G4String fTargetMaterialName;
	G4Material* fTedlar;
private:
	G4LogicalVolume* BuildCollimator();
	void BuildSupportElements();
	void MakeSetup();
	G4LogicalVolume* MakeScintillatorMatrix(G4String name);
	void Make2016BDetector();
	void MakeEffectiveDetector();
	void MakeSandwichDetector();
	void MakeTarget();
	virtual void DefineCommands();
	void BuildCalibrationSetup();
};

inline G4double Testbeam2016b::getTargetSizeX() const {
	return fTargetSizeX;
}

inline G4double Testbeam2016b::getTargetSizeY() const {
	return fTargetSizeY;
}

inline G4double Testbeam2016b::getTargetSizeZ() const {
	return fTargetSizeZ;
}

inline const G4String& Testbeam2016b::getTargetMaterialName() const {
	return fTargetMaterialName;
}

#endif /* INCLUDE_TESTBEAM2016B_HH_ */
