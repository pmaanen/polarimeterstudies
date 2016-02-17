/*
 * JediCubicPolarimeter.cc
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#include <JediCubicPolarimeter.hh>

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
#include "G4PVParameterised.hh"

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
#include "G4ios.hh"
#include <G4UnitsTable.hh>


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

static G4Colour
white   (1.0, 1.0, 1.0),  // white
gray    (0.5, 0.5, 0.5), // gray
black   (0.0, 0.0, 0.0), // black
red     (1.0, 0.0, 0.0), // red
green   (0.0, 1.0, 0.0), // green
blue    (0.0, 0.0, 1.0), // blue
cyan    (0.0, 1.0, 1.0), // cyan
magenta (1.0, 0.0, 1.0), // magenta
yellow  (1.0, 1.0, 0.0); // yellow
JediCubicPolarimeter::JediCubicPolarimeter(std::string infile):JediPolarimeter(infile),fHodoscopeShape("pizza") {
	DefineCommands();
}

G4LogicalVolume* JediCubicPolarimeter::MakeCaloCrystal() {
	auto logicDetector=MakeDetector("Detector",fScintillatorMaterial,fCrystalWidth,fCrystalWidth,fCrystalLength);
	auto detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;
}

G4LogicalVolume* JediCubicPolarimeter::MakeDetector(G4String name, G4Material* mat,G4double width, G4double height, G4double length) {
	auto solidDetector= new G4Box("Detector",width/2,height/2,length/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,mat,name);
	return logicDetector;
}


G4LogicalVolume* JediCubicPolarimeter::MakeDeltaECrystal() {
	auto logicDetector=MakeDetector("deltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
	auto detectorVisAttr=new G4VisAttributes(cyan);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;

}

G4VPhysicalVolume* JediCubicPolarimeter::Construct() {
	JediPolarimeter::Construct();
	if(fInfileName!=""){
		std::ifstream ifile(fInfileName);
		std::string line;
		double placementX,placementY,placementZ,x,y,z;
		G4String name="",matName;
		int copyNo=0;
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			std::istringstream(line)>>copyNo>>name>>matName>>x>>y>>z>>placementX>>placementY>>placementZ;
			if(!fCaloSDVolumes[name]){
				fCaloSDVolumes[name]=MakeDetector(name,G4NistManager::Instance()->FindOrBuildMaterial(matName),x,y,z);;
			}
			new G4PVPlacement (0, G4ThreeVector(placementX*CLHEP::mm,placementY*CLHEP::mm,placementZ*CLHEP::mm),fCaloSDVolumes[name] , name, fLogicWorld, false, copyNo, false);
		}
		return fPhysiWorld;
	}

	fGeomCache.clear();
	auto aCrystal=MakeDetector("Calorimeter",G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName),fCrystalWidth,fCrystalWidth,fCrystalLength);
	aCrystal->SetVisAttributes(new G4VisAttributes(green));
	fCaloSDVolumes["Calorimeter"]=aCrystal;

	if(fHodoscopeShape=="pizza"){
		auto solidSlice=new G4Tubs("DeltaE",fInnerDetectorRadius,fOuterDetectorRadius,fDeltaELength,0,10*CLHEP::deg);
		auto aDetectorElement=new G4LogicalVolume(solidSlice,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"deltaE");
		aDetectorElement->SetVisAttributes(new G4VisAttributes(cyan));
		auto rot=new G4RotationMatrix();
		auto placement=G4ThreeVector(0,0,DetectorZ-fDeltaELength/2);
		for(int iSlice=0;iSlice<36;iSlice++){
		  auto solidSlice=new G4Tubs("DeltaE",fInnerDetectorRadius,fOuterDetectorRadius,fDeltaELength/4,iSlice*10*CLHEP::deg,10*CLHEP::deg);
		  auto aDetectorElement=new G4LogicalVolume(solidSlice,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"deltaE");
		  new G4PVPlacement (0, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, iSlice, false);
		}
		placement=G4ThreeVector(0,0,DetectorZ-fDeltaELength);
		for(int iSlice=0;iSlice<36;iSlice++){
		  auto solidSlice=new G4Tubs("DeltaE",fInnerDetectorRadius,fOuterDetectorRadius,fDeltaELength/4,iSlice*10*CLHEP::deg+5*CLHEP::deg,10*CLHEP::deg);
		  auto aDetectorElement=new G4LogicalVolume(solidSlice,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"deltaE");
		  new G4PVPlacement (0, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, iSlice+36, false);
		}
	}
	else if(fHodoscopeShape=="square"){
		auto aDeltaETile=MakeDetector("DeltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
		aDeltaETile->SetVisAttributes(new G4VisAttributes(cyan));
		fCaloSDVolumes["DeltaE"]=aDeltaETile;
		PlaceHodoscope(aDeltaETile);
	}
	else{
		std::stringstream description;
		description<<"hodoscope shape: "<< fHodoscopeShape<<" not found.";
		G4Exception("JediCubicPolarimeter::Construct()","",FatalException,description.str().c_str());
	}

	PlaceCalorimeter(aCrystal);

	return fPhysiWorld;
}


JediCubicPolarimeter::~JediCubicPolarimeter() {
	// TODO Auto-generated destructor stub
}

void JediCubicPolarimeter::DefineCommands() {
	JediPolarimeter::DefineCommands();

	fMessenger->DeclareMethod("update",
			&JediCubicPolarimeter::UpdateGeometry,
			"update geometry");

	auto hodoShapeCmd=fMessenger->DeclareProperty("hodoscope_shape",
			JediCubicPolarimeter::fHodoscopeShape,
			"hodoscope shape");
	hodoShapeCmd.SetCandidates("pizza square");

}

G4double JediCubicPolarimeter::distanceToEdge(G4double a, G4double b,
		G4ThreeVector direction) {
	auto direction2d=G4TwoVector(-direction.getX(),-direction.getY());
	auto alpha=direction2d.angle(G4TwoVector(1,0))*CLHEP::rad;
	while(alpha>45*CLHEP::deg)
		alpha-=90*CLHEP::deg;
	return a/cos(alpha)/2.;

}

void JediCubicPolarimeter::PlaceCalorimeter(G4LogicalVolume* aDetectorElement) {
	int fx=-999,fy=-999;
	std::stringstream buf;
	buf.clear();
	buf.str(std::string());
	G4int copyNo=0;
	for(int iCrystalX=-fMaxCrystal; iCrystalX<fMaxCrystal+1;iCrystalX++){
		if(fx!=-999)
			fx++;
		for(int iCrystalY=-fMaxCrystal; iCrystalY<fMaxCrystal+1;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*fCrystalWidth,iCrystalY*fCrystalWidth,DetectorZ+0.5*fCrystalLength);
			if((placement.perp()-distanceToEdge(fCrystalWidth,fCrystalWidth,placement))<fInnerDetectorRadius or (placement.perp()-distanceToEdge(fCrystalWidth,fCrystalWidth,placement))>fOuterDetectorRadius)
				continue;
			if(fy!=-999)
				fy++;
			if(fx==-999){
				fx=iCrystalX;
			}
			if(fy==-999){
				fy=iCrystalY;
			}
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;
			new G4PVPlacement (0, placement, aDetectorElement, "Crystal", fLogicWorld, false, copyNo++, false);
			buf<<std::setfill('0')<<std::setw(6)<<copyNo<<" "<<aDetectorElement->GetName()<<" "<<aDetectorElement->GetMaterial()->GetName()<<" "<<fCrystalWidth<<" "<<fCrystalWidth<<" "<<fCrystalLength<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
		}
	}

}

void JediCubicPolarimeter::PlaceHodoscope(G4LogicalVolume* aDetectorElement) {
	int fx=-999,fy=-999;
	std::stringstream buf;
	buf.clear();
	buf.str(std::string());
	G4int copyNo=0;
	for(int iCrystalX=-fMaxCrystal; iCrystalX<fMaxCrystal+1;iCrystalX++){
		if(fx!=-999)
			fx++;
		for(int iCrystalY=-fMaxCrystal; iCrystalY<fMaxCrystal+1;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*fCrystalWidth,iCrystalY*fCrystalWidth,DetectorZ+0.5*fDeltaELength);
			if((placement.perp()-distanceToEdge(fCrystalWidth,fCrystalWidth,placement))<fInnerDetectorRadius or (placement.perp()-distanceToEdge(fCrystalWidth,fCrystalWidth,placement))>fOuterDetectorRadius)
				continue;
			if(fy!=-999)
				fy++;
			if(fx==-999){
				fx=iCrystalX;
			}
			if(fy==-999){
				fy=iCrystalY;
			}
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;
			new G4PVPlacement (0, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, copyNo++, false);
			buf<<std::setfill('0')<<std::setw(6)<<copyNo<<" "<<aDetectorElement->GetName()<<" "<<aDetectorElement->GetMaterial()->GetName()<<" "<<fDeltaEWidth<<" "<<fDeltaEWidth<<" "<<fDeltaELength<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
		}
	}
}
