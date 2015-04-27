/*
 * CosmicSetup.hh
 *
 *  Created on: 23.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_COSMICSETUP_HH_
#define INCLUDE_COSMICSETUP_HH_

#include <SingleCrystal.hh>

class CosmicSetup: public SingleCrystal {
public:
	CosmicSetup();
	virtual ~CosmicSetup();

	void setTriggerLength(G4double triggerLength) {
		this->triggerLength = triggerLength;
		changedParameters=true;
	}

	void setTriggerThickness(G4double triggerThickness) {
		this->triggerThickness = triggerThickness;
		changedParameters=true;
	}

	void setTriggerWidth(G4double triggerWidth) {
		this->triggerWidth = triggerWidth;
		changedParameters=true;
	}

protected:
	virtual G4VPhysicalVolume* Construct();
	virtual void DefineCommands();
	virtual void ConstructSDandField();
	G4Cache<CaloSensitiveDetector*> triggerSD;
	G4LogicalVolume* logicTrigger;
	std::vector<int> myTupleId;
	G4double triggerLength,triggerWidth,triggerThickness;
};

#endif /* INCLUDE_COSMICSETUP_HH_ */
