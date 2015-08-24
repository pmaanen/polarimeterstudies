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
	G4Box* solidDetector= new G4Box("Detector",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,scintillatorMaterial,"Detector");
	/*
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	 */
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);

	caloSDVolumes["CaloCrystal"]=logicDetector;
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

	G4Box* solidWrapping= new G4Box("Wrapping",deltaEWidth/2,deltaEWidth/2,deltaELength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Reflector",(deltaEWidth-1*wrappingThickness)/2,(deltaEWidth-1*wrappingThickness)/2,(deltaELength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	G4Box* solidDetector= new G4Box("Detector",(deltaEWidth-2*wrappingThickness)/2,(deltaEWidth-2*wrappingThickness)/2,(deltaELength-2*wrappingThickness)/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"DeltaE");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"deltaECrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(cyan);
	logicDetector->SetVisAttributes(detectorVisAttr);

	caloSDVolumes["DeltaE"]=logicDetector;
	return logicWrapping;

}

G4VPhysicalVolume* JediCubicPolarimeter::Construct() {
	JediPolarimeter::Construct();
	if(infile!=""){
		std::ifstream ifile(infile);
		std::string line;
		double placementX,placementY,placementZ,x,y,z;
		G4String name="",matName;
		int copyNo=0;
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			std::istringstream(line)>>copyNo>>name>>matName>>x>>y>>z>>placementX>>placementY>>placementZ;
			if(!caloSDVolumes[name]){
				caloSDVolumes[name]=MakeDetector(name,G4NistManager::Instance()->FindOrBuildMaterial(matName),x,y,z);;
			}
			new G4PVPlacement (0, G4ThreeVector(placementX*CLHEP::mm,placementY*CLHEP::mm,placementZ*CLHEP::mm),caloSDVolumes[name] , name, logicWorld, false, copyNo, false);
		}
		return physiWorld;
	}

	int fx=-999,fy=-999;
	geomCache.clear();
	std::stringstream buf;
	buf.clear();
	buf.str(std::string());
	G4LogicalVolume* aCrystal=MakeDetector("Calorimeter",G4NistManager::Instance()->FindOrBuildMaterial(scintillatorMaterialName),crystalWidth,crystalWidth,crystalLength);
	caloSDVolumes["Calorimeter"]=aCrystal;
	G4LogicalVolume* aDeltaETile=MakeDetector("DeltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),deltaEWidth,deltaEWidth,deltaELength);
	caloSDVolumes["DeltaE"]=aDeltaETile;
	for(int iCrystalX=-MaxCrystal; iCrystalX<MaxCrystal+1;iCrystalX++){
		if(fx!=-999)
			fx++;
		for(int iCrystalY=-MaxCrystal; iCrystalY<MaxCrystal+1;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,detectorZ+0.5*crystalLength);
			auto dePlacement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,detectorZ-0.5*deltaELength);
			if((placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))<innerDetectorRadius or (placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))>outerDetectorRadius)
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
			G4int copyNo=1000*(iCrystalX+MaxCrystal)+iCrystalY+MaxCrystal;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", logicWorld, false, copyNo, false);
			buf<<std::setfill('0')<<std::setw(6)<<copyNo<<" "<<aCrystal->GetName()<<" "<<aCrystal->GetMaterial()->GetName()<<" "<<crystalWidth<<" "<<crystalWidth<<" "<<crystalLength<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			geomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
			new G4PVPlacement (0, dePlacement, aDeltaETile, "Tile", logicWorld, false, copyNo, false);
			buf<<std::setw(6)<<copyNo<<" "<<aDeltaETile->GetName()<<" "<<aDeltaETile->GetMaterial()->GetName()<<" "<<deltaEWidth<<" "<<deltaEWidth<<" "<<deltaELength<<" "<<dePlacement.x()<<" "<<dePlacement.y()<<" "<<dePlacement.z();
			geomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
		}
	}
	return physiWorld;
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
	for(auto iVol: caloSDVolumes){
		if (CaloSD[iVol.first].Get()==0)
			CaloSD[iVol.first].Put(new CaloSensitiveDetector(iVol.first));
		SetSensitiveDetector(iVol.second,CaloSD[iVol.first].Get());
	}
}
