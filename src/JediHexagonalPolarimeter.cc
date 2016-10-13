/*
 * JediHexagonalPolarimeter.cc
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#include "JediHexagonalPolarimeter.hh"
#include "InternalBeampipe.hh"
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
using namespace CLHEP;

JediHexagonalPolarimeter::JediHexagonalPolarimeter(std::string infile):JediPolarimeter(infile) {
	DefineCommands();
}
G4LogicalVolume* JediHexagonalPolarimeter::MakeCaloCrystal() {
	G4double rOuter[]={0,fCrystalWidth/sqrt(3),fCrystalWidth/sqrt(3),0};
	G4double z[]={0*CLHEP::mm,0*CLHEP::mm,fCrystalLength,fCrystalLength};
	G4Polyhedra* solidDetector= new G4Polyhedra("Detector", 0*CLHEP::deg, 360*CLHEP::deg, 6, 4, rOuter, z);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,G4NistManager::Instance()->FindOrBuildMaterial("G4_PbWO4"),"Detector");
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;
}

G4VPhysicalVolume* JediHexagonalPolarimeter::Construct() {



	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	new InternalBeampipe(0,G4ThreeVector(0,0,fBeampipeLength/2),fLogicWorld,false,0,this);
	G4cout<<"Beampipe Length="<<G4BestUnit(fBeampipeLength,"Length")<<G4endl;
	G4cout<<"Detector Z="<<G4BestUnit(fDetectorZ,"Length")<<G4endl;
	G4cout<<"Calo Length="<<G4BestUnit(fCrystalLength,"Length")<<G4endl;
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);

/*
	//TODO: Extract into class
	auto carbon=G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
	auto solidTarget=new G4Box("Target",fTargetWidth/2,fTargetWidth/2,fTargetThickness/2);
	auto logicTarget=new G4LogicalVolume(solidTarget,carbon,"CarbonTarget");
	new G4PVPlacement(0,G4ThreeVector(0,0,targetThickness/2),logicTarget,"Target",logicWorld,0,false,0);
*/







	int ii=0;
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	G4cout<<"Geometry START"<<G4endl;
	for(int iCrystalX=-fMaxCrystal-20; iCrystalX<fMaxCrystal+20;iCrystalX++){
		for(int iCrystalY=-fMaxCrystal-20; iCrystalY<fMaxCrystal+20;iCrystalY++){
			G4ThreeVector placement;
			if(iCrystalX % 2 == 0)
				placement=G4ThreeVector(iCrystalX*fCrystalWidth*sqrt(3)/2.,iCrystalY*fCrystalWidth,fDetectorZ+0.5*fCrystalLength);
			else
				placement=G4ThreeVector(iCrystalX*fCrystalWidth*sqrt(3)/2.,(iCrystalY+0.5)*fCrystalWidth,fDetectorZ+0.5*fCrystalLength);
			if((placement.perp()-fCrystalWidth/CLHEP::mm/2)<fInnerDetectorRadius or (placement.perp()-fCrystalWidth/CLHEP::mm/2)>fOuterDetectorRadius)
				continue;
			G4double phi=placement.phi();
			if(phi<0)
				phi+=360*CLHEP::deg;
			G4cout<<"Crystal "<<ii+1<<" "<<phi/CLHEP::deg<<G4endl;
			new G4PVPlacement (0, placement, aCrystal, "Crystal", fLogicWorld, false, ++ii, false);
		}
	}
	/*
	G4cout<<"Geometry END"<<G4endl;
	G4cout<<"----------------"<<G4endl;
	G4cout<<"number of crystals: "<<ii<<G4endl;
	G4cout<<"----------------"<<G4endl;
	 */
	return fPhysiWorld;
}

void JediHexagonalPolarimeter::DefineCommands() {
	fMessenger->DeclareMethod("update",
			&JediHexagonalPolarimeter::UpdateGeometry,
			"update geometry");
}

JediHexagonalPolarimeter::~JediHexagonalPolarimeter() {
}
