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
JediCubicPolarimeter::JediCubicPolarimeter(std::string infile):JediPolarimeter(infile) {
	DefineCommands();
}

G4LogicalVolume* JediCubicPolarimeter::MakeCaloCrystal() {
	/*
	G4Box* solidWrapping= new G4Box("Wrapping",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Wrapping",(crystalWidth-1*wrappingThickness)/2,(crystalWidth-1*wrappingThickness)/2,(crystalLength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	 */
	G4Box* solidDetector= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Detector");
	/*
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	 */
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);

	fCaloSDVolumes["CaloCrystal"]=logicDetector;
	return logicDetector;
}

G4LogicalVolume* JediCubicPolarimeter::MakeDetector(G4String name, G4Material* mat,G4double width, G4double height, G4double length) {
	/*
	G4Box* solidWrapping= new G4Box("Wrapping",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Wrapping",(crystalWidth-1*wrappingThickness)/2,(crystalWidth-1*wrappingThickness)/2,(crystalLength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	 */
	G4Box* solidDetector= new G4Box("Detector",width/2,height/2,length/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,mat,name);
	/*
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	 */
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;
}


G4LogicalVolume* JediCubicPolarimeter::MakeDeltaECrystal() {

	G4Box* solidWrapping= new G4Box("Wrapping",fDeltaEWidth/2,fDeltaEWidth/2,fDeltaELength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Reflector",(fDeltaEWidth-1*fWrappingThickness)/2,(fDeltaEWidth-1*fWrappingThickness)/2,(fDeltaELength-1*fWrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	G4Box* solidDetector= new G4Box("Detector",(fDeltaEWidth-2*fWrappingThickness)/2,(fDeltaEWidth-2*fWrappingThickness)/2,(fDeltaELength-2*fWrappingThickness)/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"DeltaE");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"deltaECrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(cyan);
	logicDetector->SetVisAttributes(detectorVisAttr);

	fCaloSDVolumes["DeltaE"]=logicDetector;
	return logicWrapping;

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

	int fx=-999,fy=-999;
	fGeomCache.clear();
	std::stringstream buf;
	buf.clear();
	buf.str(std::string());
	G4LogicalVolume* aCrystal=MakeDetector("Calorimeter",G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName),fCrystalWidth,fCrystalWidth,fCrystalLength);
	fCaloSDVolumes["Calorimeter"]=aCrystal;
	G4LogicalVolume* aDeltaETile=MakeDetector("DeltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
	fCaloSDVolumes["DeltaE"]=aDeltaETile;
	for(int iCrystalX=-fMaxCrystal; iCrystalX<fMaxCrystal+1;iCrystalX++){
		if(fx!=-999)
			fx++;
		for(int iCrystalY=-fMaxCrystal; iCrystalY<fMaxCrystal+1;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*fCrystalWidth,iCrystalY*fCrystalWidth,DetectorZ+0.5*fCrystalLength);
			auto dePlacement=G4ThreeVector(iCrystalX*fCrystalWidth,iCrystalY*fCrystalWidth,DetectorZ-0.5*fDeltaELength);
			if((placement.perp()-distanceToEdge(fCrystalWidth,fCrystalWidth,placement))<fInnerDetectorRadius or (placement.perp()-distanceToEdge(fCrystalWidth,fCrystalWidth,placement))>fOuterDetectorRadius)
				continue;
			if(fy!=-999)
				fy++;
			if(fx==-999){
				fx=iCrystalX;
				G4cout<<"fx="<<fx<<G4endl;
			}
			if(fy==-999){
				fy=iCrystalY;
				G4cout<<"fy="<<fy<<G4endl;
			}
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;
			G4int copyNo=1000*(iCrystalX+fMaxCrystal)+iCrystalY+fMaxCrystal;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", fLogicWorld, false, copyNo, false);
			buf<<std::setfill('0')<<std::setw(6)<<copyNo<<" "<<aCrystal->GetName()<<" "<<aCrystal->GetMaterial()->GetName()<<" "<<fCrystalWidth<<" "<<fCrystalWidth<<" "<<fCrystalLength<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
			new G4PVPlacement (0, dePlacement, aDeltaETile, "Tile", fLogicWorld, false, copyNo, false);
			buf<<std::setw(6)<<copyNo<<" "<<aDeltaETile->GetName()<<" "<<aDeltaETile->GetMaterial()->GetName()<<" "<<fDeltaEWidth<<" "<<fDeltaEWidth<<" "<<fDeltaELength<<" "<<dePlacement.x()<<" "<<dePlacement.y()<<" "<<dePlacement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
		}
	}
	return fPhysiWorld;
}


JediCubicPolarimeter::~JediCubicPolarimeter() {
	// TODO Auto-generated destructor stub
}

void JediCubicPolarimeter::DefineCommands() {
	JediPolarimeter::DefineCommands();

	G4GenericMessenger::Command& updateCmd
	= fMessenger->DeclareMethod("update",
			&JediCubicPolarimeter::UpdateGeometry,
			"update geometry");
}

void JediCubicPolarimeter::ConstructSDandField() {
	for(auto iVol: fCaloSDVolumes){
		if (fCaloSD[iVol.first].Get()==0)
			fCaloSD[iVol.first].Put(new CaloSensitiveDetector(iVol.first));
		SetSensitiveDetector(iVol.second,fCaloSD[iVol.first].Get());
	}
}

G4double JediCubicPolarimeter::distanceToEdge(G4double a, G4double b,
		G4ThreeVector direction) {
	auto direction2d=G4TwoVector(direction.getX(),direction.getY());
	auto alpha=direction.angle(G4ThreeVector(0,1,0))*CLHEP::rad;
	while(alpha>45*CLHEP::deg)
		alpha-=90*CLHEP::deg;
	return a*sqrt(1+tan(alpha)*tan(alpha));
}
