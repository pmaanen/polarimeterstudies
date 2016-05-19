/*
 * CosmicSetup.hh
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_TESTBENCH_HH_
#define INCLUDE_TESTBENCH_HH_

#include <SingleCrystal.hh>

class Testbench: public SingleCrystal {
public:
	Testbench();
	virtual ~Testbench();

	void setTriggerLength(G4double triggerLength) {
		this->fTriggerHeight = triggerLength;
		fChangedParameters=true;
	}

	void setTriggerThickness(G4double triggerThickness) {
		this->fTriggerThickness = triggerThickness;
		fChangedParameters=true;
	}

	void setTriggerWidth(G4double triggerWidth) {
		this->fTriggerWidth = triggerWidth;
		fChangedParameters=true;
	}

	void setTriggerOffsetX(G4double triggerOffsetX) {
		this->fTriggerOffsetX = triggerOffsetX;
		fChangedParameters=true;
	}

	void setTriggerOffsetY(G4double triggerOffsetY) {
		this->fTriggerOffsetY = triggerOffsetY;
		fChangedParameters=true;
	}

	void setTriggerOffsetZ(G4double triggerOffsetZ) {
		this->fTriggerOffsetZ = triggerOffsetZ;
		fChangedParameters=true;
	}

protected:
	virtual G4VPhysicalVolume* Construct();
	virtual void DefineCommands();
	G4Cache<CaloSensitiveDetector*> fTriggerSD;
	G4LogicalVolume* fLogicTrigger;
	std::vector<int> fMyTupleId;
	G4double fTriggerHeight,fTriggerWidth,fTriggerThickness,fTriggerOffsetX,fTriggerOffsetY,fTriggerOffsetZ;
	G4bool fTrigger;
};

#endif /* INCLUDE_TESTBENCH_HH_ */
