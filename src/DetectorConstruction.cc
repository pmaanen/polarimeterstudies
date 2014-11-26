// ********************************************************************
// *                                                                  *
// *                          EDM Polarimetry                         *
// *                                                                  *
// * Detector physics                                                 *
// * RWTH Aachen 24.10.2012                                           *
// * Fabian Hinder                                                    *
// ********************************************************************
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#include "DetectorConstruction.hh"

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
#include "DetectorMessenger.hh"
#include "G4GDMLParser.hh"
#include "TNtuple.h"
#include "Analysis.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "global.hh"
using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:physiWorld(0),geomfile(""),parser(G4GDMLParser())
{
	dcMessenger=new DetectorMessenger(this);
	if(vm.count("Detector.geometry"))
		this->geomfile=vm["Detector.geometry"].as<std::string>().c_str();
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
	// 	delete TargetMaterial;
}

void DetectorConstruction::UpdateGeometry()
{
	G4cout <<"Updating Geometry...";
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout<<"done!"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

	//Clean Up the Old Geometry

	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	// World
	if(geomfile==""){
		/*
		G4double z[]={0,150*mm};
		G4double rInner={0*mm,0*mm};
		G4double rOuter[]={60*mm,60*mm};
		G4Polyhedra* solidCrystal= new G4Polyhedra("Crystal",0*deg,360*deg,6,z,rInner,rOuter);
		 */

		G4Box* solidWorld=new G4Box("World",1*m,1*m,1*m);
		G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
		logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
		physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);

		/*
		G4LogicalVolume* logicCrystal=parser.GetVolume("CrystalLV");
		G4cout<<logicCrystal->GetName()<<G4endl;
		new G4PVPlacement(0,G4ThreeVector(0,0,0),logicCrystal,"Crystal",logicWorld,0,0,0);
		G4ThreeVector pos(0,2*mm,0);
		for(G4int ii=1;ii<7;ii++){
			pos.rotateZ(60*deg);
			new G4PVPlacement(0,pos,logicCrystal,"Crystal",logicWorld,0,ii,0);
		}
		 */
	}
	else{
		parser.Read(geomfile);
		physiWorld=parser.GetWorldVolume();
	}

	return physiWorld;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::WriteWorldToFile(G4String filename) {
	G4GDMLParser parser;
	if(!physiWorld){
		std::stringstream o;
		o<<"physiWorld pointer is NULL.";
		G4Exception("DetectorConstruction::WriteWorldToFile()","PointerError",JustWarning,o.str().c_str());
		return;
	}
	parser.Write(filename,physiWorld);
}

void DetectorConstruction::ConstructSDandField() {
	//------------------------------------------------
	// Sensitive detectors
	//------------------------------------------------

	const G4GDMLAuxMapType* auxmap = parser.GetAuxMap();
	G4SDManager* SDman=G4SDManager::GetSDMpointer();
	for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
			iter!=auxmap->end(); iter++)
	{
		G4String SDtype="";
		G4String SDname="";
		for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
				vit!=(*iter).second.end();vit++)
		{
			G4cout<<(*vit).type<<" "<<(*vit).value<<G4endl;
			if ((*vit).type=="SensDetType")
			{
				SDtype=(*vit).value;
			}
			if ((*vit).type=="SensDetName")
			{
				SDname=(*vit).value;
			}

		}
		if(SDtype=="Tracker"){
			G4VSensitiveDetector* mydet = SDman->FindSensitiveDetector(SDname);
			if(!mydet){
				mydet = new TrackerSensitiveDetector(SDname,SDname+"Collection");
				SDman->AddNewDetector( mydet );
			}
			G4LogicalVolume* myvol = (*iter).first;
			G4cout<<"Attaching sensitive detector at "<<mydet<<" to detector "<<myvol->GetName()<<G4endl;
			SetSensitiveDetector(myvol,mydet);
		}
		else if(SDtype=="Calo"){
			G4LogicalVolume* myvol = (*iter).first;
			CaloSensitiveDetector* mymfd = new CaloSensitiveDetector(SDname);
			G4cout<<"Creating Sensitive Detector "<<SDname<<G4endl;
			SetSensitiveDetector(myvol,mymfd);
		}
	}
}
