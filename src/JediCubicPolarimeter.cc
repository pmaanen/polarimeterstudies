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
#include "TMath.h"
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
JediCubicPolarimeter::JediCubicPolarimeter() {

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;

	beampipeRadius=10*CLHEP::cm;
	beampipeThickness=2*CLHEP::mm;

	crystalLength=10*CLHEP::cm;
	crystalWidth=8.5*CLHEP::cm;

	changedParameters=true;
	ComputeParameters();
	DefineCommands();
}

void JediCubicPolarimeter::ConstructSDandField() {
	CaloSensitiveDetector* caloSD=new CaloSensitiveDetector("Calorimeter");
	SetSensitiveDetector("Detector",caloSD);
}

G4LogicalVolume* JediCubicPolarimeter::MakeDetector() {
	G4Box* solidDetector= new G4Box("Detector",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,G4NistManager::Instance()->FindOrBuildMaterial("G4_PbWO4"),"Detector");
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;
	/*
	 * 	<polyhedra name= "Crystal" startphi="0" deltaphi="360" numsides="6" aunit="deg" lunit= "mm">
   		<zplane  rmax="25.9807" z="0"/>
   		<zplane  rmax="25.9807" z="100" />
	G4Box* solidDetector=new G4Box("Detector",crystalWidth/2,crystalWidth/2,crystalLength/2);

	 */
}

G4VPhysicalVolume* JediCubicPolarimeter::Construct() {

	G4Box* solidWorld=new G4Box("World",3*CLHEP::m,3*CLHEP::m,3*CLHEP::m);
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

	physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);
	if(changedParameters)
		ComputeParameters();
	int ii=0;
	G4LogicalVolume* aCrystal=MakeDetector();
	G4cout<<"Geometry START"<<G4endl;
	for(int iCrystalX=-MaxCrystal-20; iCrystalX<MaxCrystal+20;iCrystalX++){
		for(int iCrystalY=-MaxCrystal-20; iCrystalY<MaxCrystal+20;iCrystalY++){
			G4ThreeVector placement;
				placement=G4ThreeVector(iCrystalX*crystalWidth,iCrystalY*crystalWidth,DetectorZ+0.5*crystalLength);
			if((placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))<innerDetectorRadius or (placement.perp()-crystalWidth/CLHEP::mm/sqrt(2))>outerDetectorRadius)
				continue;
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;
			G4cout<<"Crystal "<<ii+1<<" "<<phi/CLHEP::deg<<G4endl;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", logicWorld, false, ++ii, false);
		}
	}

	G4cout<<"Geometry END"<<G4endl;
	G4cout<<"----------------"<<G4endl;
	G4cout<<"number of crystals: "<<ii<<G4endl;
	G4cout<<"----------------"<<G4endl;
	new G4PVPlacement(0,G4ThreeVector(0,0,0),MakeBeampipe(),"Beampipe",logicWorld,false,0,false);
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	return physiWorld;
}

void JediCubicPolarimeter::UpdateGeometry() {
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
