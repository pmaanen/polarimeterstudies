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

	G4double getTargetSizeX() const {
		return fTargetSizeX;
	}

	G4double getTargetSizeY() const {
		return fTargetSizeY;
	}

	G4double getTargetSizeZ() const {
		return fTargetSizeZ;
	}

	const G4String& getTargetMaterialName() const {
		return fTargetMaterialName;
	}

private:

	G4double fArmLength,fArmWidth,fArmAngle,fDetectorHeight,fMinDistance,fTargetSizeX,fTargetSizeY,fTargetSizeZ,fVetoSizeXY,fVetoSizeZ,fHoleSizeXY;;
	G4int fNx,fNy;
	G4bool fSupport,fRightDetector,fLeftDetector,fTarget,fMonitor, fBeampipe, fTrigger, fVeto;
	std::unique_ptr<G4GenericMessenger> fTargetMessenger;
	G4String fTargetMaterialName;
};

#endif /* INCLUDE_E22_HH_ */
