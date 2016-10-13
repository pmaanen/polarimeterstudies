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
#include <TestBeam2016A.hh>
#include <TestBeam2016B.hh>
#include <map>
using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction(),fGeometry(nullptr),fGeometryName("")
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
	delete fGeometry;
}

void DetectorConstruction::InitializeGeometry() {


	if(fGeometry){
		delete fGeometry;
		fGeometry=nullptr;
	}
	auto name=fGeometryName.substr(0,fGeometryName.find(":"));

	if(name=="cubic"){
		fGeometry=new JediCubicPolarimeter();
	}
	else if(name=="hexagonal"){
		fGeometry=new JediHexagonalPolarimeter;
	}
	else if(name=="single"){
		fGeometry= new SingleCrystal();
	}
	else if(name=="sandwich"){
		fGeometry= new JediSandwichCalorimeter();
	}
	else if(name=="testbench"){
		fGeometry= new Testbench();
	}
	else if(name=="singlesandwich"){
		fGeometry= new SingleSandwichModule();
	}
	else if(name=="edda"){
		fGeometry= new EddaDetectorConstruction();
	}
	else if(name=="testbeam")
	{
		fGeometry= new TestBeam2016A();
	}
	else if(name=="testbeam2016b")
	{
		fGeometry= new TestBeam2016B();
	}
		if(!fGeometry){
			G4Exception("main","Geom001",JustWarning,"No geometry chosen. Loading default geometry.");
			fGeometry= new SingleCrystal();
		}
}
