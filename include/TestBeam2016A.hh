/*
 * TestBeamSetup.hh
 *
 *  Created on: 21.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_TESTBEAM2016A_HH_
#define INCLUDE_TESTBEAM2016A_HH_
#include "SingleCrystal.hh"
class TestBeam2016A: public SingleCrystal {
public:
	TestBeam2016A();
	virtual ~TestBeam2016A();

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

		void setCalorimeterPosition(G4ThreeVector calorimeterPosition) {
			fCalorimeterPosition = calorimeterPosition;
			fChangedParameters=true;
		}



protected:
	virtual G4VPhysicalVolume* Construct();
	G4LogicalVolume* MakeDetector(G4String name, G4Material* mat,G4double width, G4double height, G4double thickness);

	void MakeSetup();
	void Make2016ADetector();
	void MakeEffectiveDetector();
	void MakeSandwichDetector();

	virtual void DefineCommands();
	G4Cache<CaloSensitiveDetector*> fTriggerSD;
	G4LogicalVolume* fLogicTrigger;
	std::vector<int> fMyTupleId;
	G4double fTriggerHeight,fTriggerWidth,fTriggerThickness,fTriggerOffsetX,fTriggerOffsetY,fTriggerOffsetZ;
	G4ThreeVector fCalorimeterPosition;
	G4bool fTrigger;
	G4String fDetectorName;

};

#endif /* INCLUDE_TESTBEAM2016A_HH_ */
