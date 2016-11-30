/*
 * TestBeamSetup.hh
 *
 *  Created on: 21.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_E21_HH_
#define INCLUDE_E21_HH_
#include "SingleCrystal.hh"
class E21: public SingleCrystal {
public:
	E21();
	virtual ~E21();

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
	void MakeSandwichDetector() {
		G4RotationMatrix* rot = new G4RotationMatrix();
		rot->set(fPhi, fTheta, fPsi);
		auto logicHodoscope = MakeDetector("Hodoscope",
				G4NistManager::Instance()->FindOrBuildMaterial(
						"G4_PLASTIC_SC_VINYLTOLUENE"), fHCalSizeZ, fHCalSizeZ,
				1 * CLHEP::cm);
		fSensitiveDetectors.Update("Hodoscope", SDtype::kCalorimeter,
				logVolVector { logicHodoscope });
		auto logicAbsorber = MakeDetector("Detector",
				G4NistManager::Instance()->FindOrBuildMaterial("G4_W"),
				fHCalSizeZ, fHCalSizeZ, fHCalSizeXY);
		fSensitiveDetectors.Update("Detector", SDtype::kTracker, logVolVector {
				logicAbsorber });
		new G4PVPlacement(rot,
				fCalorimeterPosition + G4ThreeVector(0, 0, -.5 * CLHEP::cm),
				logicHodoscope, "Trigger", fLogicWorld, false, 0, false);
		new G4PVPlacement(rot,
				fCalorimeterPosition + G4ThreeVector(0, 0, fHCalSizeXY / 2),
				logicAbsorber, "Detector", fLogicWorld, false, 0, false);
		return;
	}

	virtual void DefineCommands();
	G4Cache<CaloSensitiveDetector*> fTriggerSD;
	G4LogicalVolume* fLogicTrigger;
	G4double fTriggerHeight,fTriggerWidth,fTriggerThickness,fTriggerOffsetX,fTriggerOffsetY,fTriggerOffsetZ;
	G4ThreeVector fCalorimeterPosition;
	G4bool fTrigger;
	G4String fDetectorName;

};

#endif /* INCLUDE_E21_HH_ */
