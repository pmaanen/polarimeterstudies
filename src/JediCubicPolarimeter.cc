/*
 * JediCubicPolarimeter.cc
 *
 *  Created on: 15.01.2015
 *      Author: pmaanen
 */

#include <JediCubicPolarimeter.hh>
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

G4LogicalVolume* JediCubicPolarimeter::BuildCaloCrystal() {
	auto logicDetector=MakeDetector("Detector",fHCalMaterial,fHCalSizeXY/2,fHCalSizeXY/2,fHCalSizeZ);
	auto detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;
}

G4LogicalVolume* JediCubicPolarimeter::MakeDetector(G4String name, G4Material* mat,G4double halfSizeX, G4double halfSizeY, G4double halfSizeZ) {
	auto solidDetector= new G4Box("Detector",halfSizeX/2,halfSizeY/2,halfSizeZ/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,mat,name);
	return logicDetector;
}


G4LogicalVolume* JediCubicPolarimeter::MakeDeltaECrystal() {
	auto logicDetector=MakeDetector("deltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
	auto detectorVisAttr=new G4VisAttributes(cyan);
	logicDetector->SetVisAttributes(detectorVisAttr);
	return logicDetector;

}

void JediCubicPolarimeter::ReadFromFile() {
	std::ifstream ifile(fInfileName);
	std::string line;
	double placementX, placementY, placementZ, x, y, z;
	G4String name = "", matName;
	int copyNo = 0;
	while (std::getline(ifile, line)) {
		if (line[0] == '#')
			continue;

		std::istringstream(line) >> copyNo >> name >> matName >> x >> y >> z
				>> placementX >> placementY >> placementZ;
		if (fSensitiveDetectors.getMap().count(name) == 0) {
			fSensitiveDetectors.Update(name, SDtype::kCalorimeter,
					logVolVector { MakeDetector(name,
							G4NistManager::Instance()->FindOrBuildMaterial(
									matName), x, y, z) });
		}
		new G4PVPlacement(0,
				G4ThreeVector(placementX * CLHEP::mm, placementY * CLHEP::mm,
						placementZ * CLHEP::mm),
						fSensitiveDetectors.getMap().at(name).fLogVol[0], name,
						fLogicWorld, false, copyNo, false);
	}
}

G4VPhysicalVolume* JediCubicPolarimeter::Construct() {
	if(fInfileName!=""){
		ReadFromFile();
		return fPhysiWorld;
	}

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	new InternalBeampipe(0,G4ThreeVector(0,0,0),fLogicWorld,false,0,this);
	auto worldVisAttributes=new G4VisAttributes(cyan);
	worldVisAttributes->SetForceWireframe(true);
	fLogicWorld->SetVisAttributes(worldVisAttributes);


	//TODO: Extract into class
	auto carbon=G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
	G4Box* solidTarget=new G4Box("Target",fTargetWidth/2,fTargetWidth/2,fTargetThickness/2);
	fTarget=new G4LogicalVolume(solidTarget,carbon,"CarbonTarget");
	//new G4PVPlacement(0,G4ThreeVector(0,0,targetThickness/2),logicTarget,"Target",logicWorld,0,false,0);

	fGeomCache.clear();
	auto aCrystal=MakeDetector("Calorimeter",G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName),fHCalSizeXY,fHCalSizeXY,fHCalSizeZ);
	aCrystal->SetVisAttributes(new G4VisAttributes(green));
	fDetectorSolid=aCrystal->GetSolid();

	fSensitiveDetectors.Update("Calorimeter",SDtype::kCalorimeter,logVolVector{aCrystal});
	if(fHodoscopeShape=="pizza"){
		auto placement=G4ThreeVector(0,0,fDetectorZ-fDeltaELength+fDeltaELength/4);
		auto solidSlice=new G4Tubs("DeltaE",fInnerDetectorRadius,fOuterDetectorRadius,fDeltaELength/4,10*CLHEP::deg,10*CLHEP::deg);
		auto aDetectorElement=new G4LogicalVolume(solidSlice,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Hodoscope");
		fSensitiveDetectors.Update("Hodoscope",SDtype::kCalorimeter,logVolVector{aDetectorElement});
		aDetectorElement->SetVisAttributes(new G4VisAttributes(cyan));
		for(int iSlice=0;iSlice<36;iSlice++){
			auto rot1=new G4RotationMatrix();
			rot1->rotateZ(iSlice*10*CLHEP::deg);
			new G4PVPlacement (rot1, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, iSlice);

		}
		placement=G4ThreeVector(0,0,fDetectorZ-fDeltaELength/4);
		for(int iSlice=0;iSlice<36;iSlice++){
			auto rot2=new G4RotationMatrix();
			rot2->rotateZ(iSlice*10*CLHEP::deg+5*CLHEP::deg);
			new G4PVPlacement (rot2, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, iSlice+36, false);
		}

	}
	else if(fHodoscopeShape=="square"){
		auto aDeltaETile=MakeDetector("DeltaE",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fDeltaEWidth,fDeltaEWidth,fDeltaELength);
		aDeltaETile->SetVisAttributes(new G4VisAttributes(cyan));
		fSensitiveDetectors.Update("Hodoscope",SDtype::kCalorimeter,logVolVector{aDeltaETile});
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
			&JediCubicPolarimeter::GeometryHasChanged,
			"update geometry");

	auto hodoShapeCmd=fMessenger->DeclareProperty("hodoscope_shape",
			JediCubicPolarimeter::fHodoscopeShape,
			"hodoscope shape");
	hodoShapeCmd.SetCandidates("pizza square");

}

G4double JediCubicPolarimeter::distanceToEdge(G4double sizeXY,
		G4ThreeVector direction) {


	G4double dX=0,dY=0;
	if(fabs(direction.x())>=fabs(direction.y())){
		dX=sizeXY/2;
		dY=dX*fabs(direction.y())/fabs(direction.x());
	}
	else {
		dY=sizeXY/2;
		dX=dY*fabs(direction.x())/fabs(direction.y());
	}
	return sqrt(dX*dX+dY*dY);

	/*
	G4double magnitude=0;
	auto angle=direction.getPhi();
	float abs_cos_angle= fabs(cos(angle));
	float abs_sin_angle= fabs(sin(angle));
	if (a/2*abs_sin_angle <= b/2*abs_cos_angle)
	{
		magnitude= a/2/abs_cos_angle;
	}
	else
	{
		magnitude= b/2/abs_sin_angle;
	}

	return magnitude;

	 */
}

void JediCubicPolarimeter::PlaceCalorimeter(G4LogicalVolume* aDetectorElement) {
	std::stringstream buf;
	buf.clear();
	buf.str(std::string());
	G4int index=0;
	for(int iCrystalX=-fMaxCrystal; iCrystalX<fMaxCrystal+1;iCrystalX++){
		for(int iCrystalY=-fMaxCrystal; iCrystalY<fMaxCrystal+1;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*fHCalSizeXY,iCrystalY*fHCalSizeXY,fDetectorZ+0.5*fHCalSizeZ);
			if((placement.perp()-distanceToEdge(fHCalSizeXY,placement))<=fInnerDetectorRadius or (placement.perp()-distanceToEdge(fHCalSizeXY,placement)>=fOuterDetectorRadius))
				continue;
			if(iCrystalX==iCrystalY and iCrystalX==0)
				continue;
			index=iCrystalX+fMaxCrystal+(iCrystalY+fMaxCrystal)*1000;
			new G4PVPlacement (0, placement, aDetectorElement, "Crystal", fLogicWorld, false, index, false);
			buf<<std::setfill('0')<<std::setw(6)<<index<<" "<<aDetectorElement->GetName()<<" "<<aDetectorElement->GetMaterial()->GetName()<<" "<<fHCalSizeXY<<" "<<fHCalSizeXY<<" "<<fHCalSizeZ<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
		}
	}

}

void JediCubicPolarimeter::PlaceHodoscope(G4LogicalVolume* aDetectorElement) {
	std::stringstream buf;
	buf.clear();
	buf.str(std::string());
	G4int index=0;
	for(int iCrystalX=-fMaxCrystal; iCrystalX<fMaxCrystal+1;iCrystalX++){
		for(int iCrystalY=-fMaxCrystal; iCrystalY<fMaxCrystal+1;iCrystalY++){
			auto placement=G4ThreeVector(iCrystalX*fHCalSizeXY,iCrystalY*fHCalSizeXY,fDetectorZ-0.5*fDeltaELength);
			if((placement.perp()-distanceToEdge(fHCalSizeXY,placement))<=fInnerDetectorRadius or (placement.perp()-distanceToEdge(fHCalSizeXY,placement))>fOuterDetectorRadius)
				continue;
			if(iCrystalX==iCrystalY and iCrystalX==0)
				continue;
			index=iCrystalX+fMaxCrystal+(iCrystalY+fMaxCrystal)*1000;
			new G4PVPlacement (0, placement, aDetectorElement, "Hodoscope", fLogicWorld, false, index, false);
			buf<<std::setfill('0')<<std::setw(6)<<index<<" "<<aDetectorElement->GetName()<<" "<<aDetectorElement->GetMaterial()->GetName()<<" "<<fDeltaEWidth<<" "<<fDeltaEWidth<<" "<<fDeltaELength<<" "<<placement.x()<<" "<<placement.y()<<" "<<placement.z();
			fGeomCache.push_back(buf.str());
			buf.clear();
			buf.str(std::string());
		}
	}
}
