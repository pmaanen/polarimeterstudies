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



#include "Testbench.hh"
#include "DetectorConstruction.hh"
#include "JediCubicPolarimeter.hh"
#include "JediHexagonalPolarimeter.hh"
#include "SingleCrystal.hh"
#include "JediSandwichCalorimeter.hh"
#include "SingleSandwichModule.hh"
#include "EddaDetectorConstruction.hh"
#include "global.hh"
#include <G4GenericMessenger.hh>
#include <map>
using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction(),fGeometry(nullptr),fGeometryName(""),fMessenger(nullptr)
{

	fGeometryName=gConfig["detector.geometry"].as<std::string>();

	InitializeGeometry();
	/*
	 * Does not work right now.
	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/geometry/",
			"geometry control");

	fMessenger->DeclareMethod("setGeometry",&DetectorConstruction::setGeometryName,"");
	 */
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
	delete fMessenger;
	delete fGeometry;
}

void DetectorConstruction::InitializeGeometry() {


	if(fGeometry){
		delete fGeometry;
		fGeometry=nullptr;
	}
	std::string cubic("cubic");
	std::string hexagonal("hexagonal");
	std::string gdml("gdml");
	std::string single("single");
	std::string sandwich("sandwich");
	std::string testbench("testbench");
	std::string singlesandwich("singlesandwich");
	std::string edda("edda");
	if(fGeometryName==cubic){
		fGeometry=new JediCubicPolarimeter();
	}
	else if(fGeometryName==hexagonal){
		fGeometry=new JediHexagonalPolarimeter;
	}
	else if(fGeometryName==single){
		fGeometry= new SingleCrystal();
	}
	else if(fGeometryName==sandwich){
		fGeometry= new JediSandwichCalorimeter();
	}
	else if(fGeometryName==testbench){
		fGeometry= new Testbench();
	}
	else if(fGeometryName==singlesandwich){
		fGeometry= new SingleSandwichModule();
	}
	else if(fGeometryName==edda){
		fGeometry= new EddaDetectorConstruction();
	}
	if(!fGeometry){
		G4Exception("main","Geom001",JustWarning,"No geometry chosen. Loading default geometry.");
		fGeometry= new SingleCrystal();
	}
}
