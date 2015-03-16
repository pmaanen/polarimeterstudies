/*
 * JediPolarimeter.hh
 *
 *  Created on: 20.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIPOLARIMETER_HH_
#define INCLUDE_JEDIPOLARIMETER_HH_

#include <G4VUserDetectorConstruction.hh>



//***** include basic geometry classes
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Para.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"
#include "G4CutTubs.hh"
#include "G4Torus.hh"
#include "G4Polyhedra.hh"
//***** End of include basic geometry classes

#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"


#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4SDManager.hh"
#include "CaloSensitiveDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"

#include "TrackerSensitiveDetector.hh"
#include "G4GDMLParser.hh"
#include "TNtuple.h"
#include "Analysis.hh"
#include "G4GenericMessenger.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "global.hh"
#include "globals.hh"
#include "G4ios.hh"
#include <G4UnitsTable.hh>
#include "G4RegionStore.hh"
#include "G4Cache.hh"
class JediPolarimeter: public G4VUserDetectorConstruction {
public:
	JediPolarimeter();
	virtual ~JediPolarimeter();
	virtual G4VPhysicalVolume* Construct();
	void WriteWorldToFile(G4String filename);
	virtual void ConstructSDandField();
	void setBeampipeRadius(G4double beampipeRadius) {
		this->beampipeRadius = beampipeRadius;
		changedParameters=true;
	}

	void setCrystalLength(G4double crystalLength) {
		this->crystalLength = crystalLength;
		changedParameters=true;
	}

	void setCrystalWidth(G4double crystalWidth) {
		this->crystalWidth = crystalWidth;
		changedParameters=true;
	}

	void setThetaMax(G4double thetaMax) {
		this->thetaMax = thetaMax;
		changedParameters=true;
	}

	void setThetaMin(G4double thetaMin) {
		this->thetaMin = thetaMin;
		changedParameters=true;
	}

	void setScintillatorMaterialName(const G4String& scintillatorMaterialName) {
		auto oldName=scintillatorMaterial->GetName();
		auto newMat=G4NistManager::Instance()->FindOrBuildMaterial(scintillatorMaterialName);
		if(!newMat){
			//G4Exception("JediPolarimeter::setScintillatorMaterialName","MatNotFound",G4ExceptionSeverity::JustWarning,"Material not found! Material not changed.");
			G4cerr<<"Material not found"<<G4endl;
			return;
		}
		scintillatorMaterial=newMat;
		logicCaloCrystal->SetMaterial(scintillatorMaterial);
		G4cout<<"Changing Material from "<<oldName<<" to "<<scintillatorMaterial->GetName()<<G4endl;
	}

protected:
	G4LogicalVolume* MakeBeampipe();
	G4LogicalVolume*  MakeTargetChamber();
	virtual G4LogicalVolume* MakeDetector()=0;

	G4LogicalVolume* logicWorld;
	G4VPhysicalVolume* physiWorld;
	G4GenericMessenger* fMessenger;

	G4double thetaMin, thetaMax;
	G4double beampipeRadius, beampipeThickness, crystalLength, crystalWidth,
	innerDetectorRadius, outerDetectorRadius,detectorZ,wrappingThickness, targetChamberThickness, targetChamberZ1, targetChamberZ2,
	worldSizeXY,worldSizeZ;
	G4String scintillatorMaterialName;
	G4Material* scintillatorMaterial;

	G4bool changedParameters;
	G4int MinCrystal,MaxCrystal;

	void DefineCommands();
	void ComputeParameters();
	virtual void UpdateGeometry();
	G4Cache<CaloSensitiveDetector*> CaloSD;
	G4Cache<TrackerSensitiveDetector*> TrackerSD;
	G4LogicalVolume* logicCaloCrystal;
};

#endif /* INCLUDE_JEDIPOLARIMETER_HH_ */
