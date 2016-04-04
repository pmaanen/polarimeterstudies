/*
 * JediPolarimeter.hh
 *
 *  Created on: 20.01.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIMAINVOLUME_HH_
#define INCLUDE_JEDIMAINVOLUME_HH_

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
#include "PerfectDetector.hh"
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

#include <map>
class JediMainVolume: public G4VUserDetectorConstruction {
public:
	JediMainVolume(std::string _infile="");
	virtual ~JediMainVolume();
	virtual G4VPhysicalVolume* Construct();

	//Dump world to gdml file.
	void WriteWorldToFile(G4String filename="");
	virtual void ConstructSDandField();
	//Setters for properties
	void setBeampipeRadius(G4double beampipeRadius) {this->fBeampipeRadius = beampipeRadius;	fChangedParameters=true;	}
	void setCrystalLength(G4double crystalLength) {	this->fCrystalLength = crystalLength;fChangedParameters=true;	}
	void setCrystalWidth(G4double crystalWidth) {this->fCrystalWidth = crystalWidth;fChangedParameters=true;}
	void setThetaMax(G4double thetaMax) {this->fThetaMax = thetaMax; fChangedParameters=true;}
	void setThetaMin(G4double thetaMin) {this->fThetaMin = thetaMin;fChangedParameters=true;}
	void setDeltaElength(G4double deltaElength){this->fDeltaELength = deltaElength;fChangedParameters=true;}
	void setDeltaEwidth(G4double deltaEwidth){this->fDeltaEWidth = deltaEwidth;fChangedParameters=true;}
	virtual void UpdateGeometry();
	void setCaloMaterialName(const G4String& scintillatorMaterialName) {
		auto oldName=fScintillatorMaterial->GetName();
		auto newMat=G4NistManager::Instance()->FindOrBuildMaterial(scintillatorMaterialName);
		if(!newMat){
			G4Exception("JediPolarimeter::setScintillatorMaterialName","MatNotFound",G4ExceptionSeverity::JustWarning,"Material not found! Material not changed.");
			return;
		}
		fScintillatorMaterial=newMat;
		fScintillatorMaterialName=scintillatorMaterialName;
		return;
	}

	void setInfile(const std::string& infile) {this->fInfileName = infile;}

	void setTargetThickness(G4double targetThickness) {
		this->fTargetThickness = targetThickness;
		fChangedParameters=true;
	}

	void setTargetWidth(G4double targetWidth) {
		this->fTargetWidth = targetWidth;
		fChangedParameters=true;
	}

	 void checkGeometry(){
		fPhysiWorld->CheckOverlaps(2000);
	}

	G4double getDetectorZ() const {
		return DetectorZ;
	}

	G4double getBeampipeRadius() const {
		return fBeampipeRadius;
	}

	G4double getBeampipeThickness() const {
		return fBeampipeThickness;
	}

	G4bool isChangedParameters() const {
		return fChangedParameters;
	}

	G4double getCrystalLength() const {
		return fCrystalLength;
	}

	G4double getCrystalWidth() const {
		return fCrystalWidth;
	}

	G4double getDeltaELength() const {
		return fDeltaELength;
	}

	G4double getDeltaEWidth() const {
		return fDeltaEWidth;
	}

	G4double getDeltaEz() const {
		return fDeltaEZ;
	}

	G4double getInnerDetectorRadius() const {
		return fInnerDetectorRadius;
	}

	G4double getOuterDetectorRadius() const {
		return fOuterDetectorRadius;
	}

	G4double getSafetyDistance() const {
		return fSafetyDistance;
	}

	const G4String& getScintillatorMaterialName() const {
		return fScintillatorMaterialName;
	}

	G4double getTargetChamberThickness() const {
		return fTargetChamberThickness;
	}

	G4double getTargetChamberZ1() const {
		return fTargetChamberZ1;
	}

	G4double getTargetChamberZ2() const {
		return fTargetChamberZ2;
	}

	G4double getTargetThickness() const {
		return fTargetThickness;
	}

	G4double getTargetWidth() const {
		return fTargetWidth;
	}

	G4double getThetaMax() const {
		return fThetaMax;
	}

	G4double getThetaMin() const {
		return fThetaMin;
	}

	G4double getWorldSizeXy() const {
		return fWorldSizeXY;
	}

	G4double getWorldSizeZ() const {
		return fWorldSizeZ;
	}

	G4double getWrappingThickness() const {
		return fWrappingThickness;
	}

	G4bool getBuildBeampipe() const {
		return fBuildBeampipe;
	}

	G4bool getBuildCalorimeter() const {
		return fBuildCalorimeter;
	}

	G4bool getBuildHodoscope() const {
		return fBuildHodoscope;
	}

	G4bool getBuildTargetChamber() const {
		return fBuildTargetChamber;
	}

protected:
	virtual G4LogicalVolume* MakeCaloCrystal()=0;


	G4LogicalVolume* fLogicWorld;
	G4VPhysicalVolume* fPhysiWorld;
	G4GenericMessenger* fMessenger;

	//Geometry parameters
	G4double fThetaMin, fThetaMax;
	G4double fBeampipeRadius, fBeampipeThickness, fCrystalLength, fCrystalWidth,
	fInnerDetectorRadius, fOuterDetectorRadius,DetectorZ,fWrappingThickness, fTargetChamberThickness, fTargetChamberZ1, fTargetChamberZ2,
	fWorldSizeXY,fWorldSizeZ,fDeltaELength,fDeltaEWidth,fDeltaEZ,fTargetThickness,fTargetWidth,fSafetyDistance;
	G4String fScintillatorMaterialName;
	G4Material* fScintillatorMaterial;

	G4bool fChangedParameters,fBuildTarget,fBuildTargetChamber,fBuildBeampipe,fBuildHodoscope,fBuildCalorimeter;
	G4int fMinCrystal,fMaxCrystal;

	virtual void DefineCommands();
	virtual void ComputeParameters();

	std::vector<std::string> fGeomCache;

	//Sensitive detectors
	std::map<G4String,G4Cache<CaloSensitiveDetector*> > fCaloSD;
	std::map<G4String,G4Cache<TrackerSensitiveDetector*> >fTrackerSD;
	std::map<G4String,G4Cache<PerfectDetector*> >fPerfectSD;
	std::map<G4String,G4LogicalVolume*> fCaloSDVolumes;
	std::map<G4String,G4LogicalVolume*> fTrackerSDVolumes;
	std::map<G4String,G4LogicalVolume*> fPerfectSDVolumes;

	//Input file if constructed from file
	std::string fInfileName;
};

#endif /* INCLUDE_JEDIMAINVOLUME_HH_ */
