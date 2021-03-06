/*
 * TestBeamSetup.hh
 *
 *  Created on: 21.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_TESTBEAM2016A_HH_
#define INCLUDE_TESTBEAM2016A_HH_
#include "SingleCrystal.hh"
class Testbeam2016a: public SingleCrystal {
public:
	Testbeam2016a();
	virtual ~Testbeam2016a()=default;

	void setTriggerSizeX(G4double x) {
			this->fTriggerSizeX = x;
			fGeometryHasBeenChanged=true;
		}

		void setTriggerSizeY(G4double y) {
			this->fTriggerSizeY = y;
			fGeometryHasBeenChanged=true;
		}

		void setTriggerSizeZ(G4double z) {
			this->fTriggerSizeZ = z;
			fGeometryHasBeenChanged=true;
		}

		void setTriggerOffsetX(G4double triggerOffsetX) {
			this->fTriggerOffsetX = triggerOffsetX;
			fGeometryHasBeenChanged=true;
		}

		void setTriggerOffsetY(G4double triggerOffsetY) {
			this->fTriggerOffsetY = triggerOffsetY;
			fGeometryHasBeenChanged=true;
		}

		void setTriggerOffsetZ(G4double triggerOffsetZ) {
			this->fTriggerOffsetZ = triggerOffsetZ;
			fGeometryHasBeenChanged=true;
		}

		void setCalorimeterPosition(G4ThreeVector calorimeterPosition) {
			fCalorimeterPosition = calorimeterPosition;
			fGeometryHasBeenChanged=true;
		}



protected:
	virtual G4VPhysicalVolume* Construct();
	void MakeSetup();
	void Make2016ADetector();
	void MakeEffectiveDetector();
	void MakeSandwichDetector();
	virtual void DefineCommands();
	G4Cache<CaloSensitiveDetector*> fTriggerSD;
	G4LogicalVolume* fLogicTrigger;
	G4double fTriggerSizeX,fTriggerSizeY,fTriggerSizeZ,fTriggerOffsetX,fTriggerOffsetY,fTriggerOffsetZ;
	G4ThreeVector fCalorimeterPosition;
	G4bool fTrigger;
	G4String fDetectorName;

};

#endif /* INCLUDE_TESTBEAM2016A_HH_ */
