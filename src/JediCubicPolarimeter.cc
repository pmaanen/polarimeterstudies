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
	G4Box* solidWrapping= new G4Box("Wrapping",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Wrapping",(crystalWidth-1*wrappingThickness)/2,(crystalWidth-1*wrappingThickness)/2,(crystalLength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	G4Box* solidDetector= new G4Box("Detector",(crystalWidth-2*wrappingThickness)/2,(crystalWidth-2*wrappingThickness)/2,(crystalLength-2*wrappingThickness)/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,scintillatorMaterial,"Detector");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);

	logicCaloCrystal=logicDetector;
	return logicWrapping;
	/*
	 * 	<polyhedra name= "Crystal" startphi="0" deltaphi="360" numsides="6" aunit="deg" lunit= "mm">
   		<zplane  rmax="25.9807" z="0"/>
   		<zplane  rmax="25.9807" z="100" />
	G4Box* solidDetector=new G4Box("Detector",crystalWidth/2,crystalWidth/2,crystalLength/2);

	 */
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

	logicDeltaE=logicDetector;
	return logicWrapping;

}

G4VPhysicalVolume* JediCubicPolarimeter::Construct() {
	JediPolarimeter::Construct();

	auto carbon=G4NistManager::Instance()->FindOrBuildMaterial("G4_C");

	G4Box* solidTarget=new G4Box("Target",1./2*CLHEP::cm,1./2*CLHEP::cm,3./2*CLHEP::cm);
	G4LogicalVolume* logicTarget=new G4LogicalVolume(solidTarget,carbon,"CarbonTarget");
	new G4PVPlacement(0,G4ThreeVector(0,0,5/2.*CLHEP::cm),logicTarget,"Target",logicWorld,0,false,0);

	if(infile!=""){
		std::ifstream ifile(infile);
		std::string line;
		//read until first non-comment line, then break loop
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			else{
				std::istringstream(line)>>crystalWidth>>crystalWidth>>crystalLength;
				break;
			}
		}
		G4LogicalVolume* aCrystal=MakeCaloCrystal();
		double placementX,placementY,placementZ;
		int copyNo=0;
		while(std::getline(ifile, line)){
			if(line[0]=='#') continue;
			std::istringstream(line)>>copyNo>>placementX>>placementY>>placementZ;
			new G4PVPlacement (0, G4ThreeVector(placementX*CLHEP::mm,placementY*CLHEP::mm,placementZ*CLHEP::mm), aCrystal, "Crystal", logicWorld, false, copyNo, false);
		}
		return physiWorld;
	}

	int ii=0;

	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4LogicalVolume* aDeltaETile=MakeDeltaECrystal();
	for(int iCrystalX=-MaxCrystal-20; iCrystalX<MaxCrystal+20;iCrystalX++){
		for(int iCrystalY=-MaxCrystal-20; iCrystalY<MaxCrystal+20;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,detectorZ+0.5*crystalLength);
			auto dePlacement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,detectorZ-0.5*deltaELength);
			if((placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))<innerDetectorRadius or (placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))>outerDetectorRadius)
				continue;
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", logicWorld, false, ++ii, false);
			G4cout<<ii<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z()<<G4endl;
			new G4PVPlacement (0, dePlacement, aDeltaETile, "Tile", logicWorld, false, ii, false);
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

	if(CaloSD.Get()==0 and logicCaloCrystal){
		CaloSensitiveDetector* SD=new CaloSensitiveDetector("Calorimeter");
		CaloSD.Put(SD);
	}

	if(deltaESD.Get()==0 and logicDeltaE){
		CaloSensitiveDetector* SD=new CaloSensitiveDetector("dE");
		deltaESD.Put(SD);
	}

}
