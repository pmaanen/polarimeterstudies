/*
 * Testbeam2016B.hh
 *
 *  Created on: 18.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_TESTBEAM2016B_HH_
#define INCLUDE_TESTBEAM2016B_HH_

#include <TestBeam2016A.hh>

class TestBeam2016B: public TestBeam2016A {
public:
	TestBeam2016B();
	virtual ~TestBeam2016B();

	virtual G4VPhysicalVolume* Construct();
	void MakeSetup();
	G4LogicalVolume* Make2016BDetector(G4String name);
	void MakeEffectiveDetector();
	void MakeSandwichDetector();
	void MakeTarget();
	virtual void DefineCommands();


private:

	G4double fArmLength,fArmWidth,fArmAngle,fDetectorHeight;
	G4int fNx,fNy;
};

#endif /* INCLUDE_TESTBEAM2016B_HH_ */
