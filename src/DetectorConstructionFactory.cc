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



#include <DetectorConstructionFactory.hh>
#include "Testbench.hh"
#include "JediCubicPolarimeter.hh"
#include "JediHexagonalPolarimeter.hh"
#include "SingleCrystal.hh"
#include "JediSandwichCalorimeter.hh"
#include "SingleSandwichModule.hh"
#include "EddaDetectorConstruction.hh"
#include <G4GenericMessenger.hh>
#include <JediCommon.hh>
#include <Testbeam2016a.hh>
#include <Testbeam2016b.hh>
#include <map>

using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VUserDetectorConstruction* DetectorConstructionFactory::Create() {

	auto GeometryName=gConfig["detector.geometry_file"].as<std::string>();
	G4VUserDetectorConstruction* Geometry=nullptr;

	auto name=GeometryName.substr(0,GeometryName.find(":"));

	if(name=="cubic"){
		Geometry=new JediCubicPolarimeter();
	}
	else if(name=="hexagonal"){
		Geometry=new JediHexagonalPolarimeter;
	}
	else if(name=="single"){
		Geometry= new SingleCrystal();
	}
	else if(name=="sandwich"){
		Geometry= new JediSandwichCalorimeter();
	}
	else if(name=="testbench"){
		Geometry= new Testbench();
	}
	else if(name=="singlesandwich"){
		Geometry= new SingleSandwichModule();
	}
	else if(name=="edda"){
		Geometry= new EddaDetectorConstruction();
	}
	else if(name=="testbeam2016a")
	{
		Geometry= new Testbeam2016a();
	}
	else if(name=="testbeam2016b")
	{
		Geometry= new Testbeam2016b();
	}
	if(!Geometry){
		G4Exception("main","Geom001",JustWarning,"No geometry chosen. Loading default geometry.");
		Geometry= new SingleCrystal();
	}

	return Geometry;
}
