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



#include <DetectorConstruction.hh>
#include "Testbench.hh"
#include "JediCubicPolarimeter.hh"
#include "JediHexagonalPolarimeter.hh"
#include "SingleCrystal.hh"
#include "JediSandwichCalorimeter.hh"
#include "SingleSandwichModule.hh"
#include "EddaDetectorConstruction.hh"
#include <G4GenericMessenger.hh>
#include <JediConfigurationManager.hh>
#include <Testbeam2016a.hh>
#include <Testbeam2016b.hh>
#include <Testbeam2017a.hh>
#include <GDMLReader.hh>
#include <map>

using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VUserDetectorConstruction* DetectorConstruction::Create() {

	auto GeometryName=JediConfigurationManager::Instance()->GetMap()["detector.geometry_file"].as<std::string>();
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
	else if(name=="testbeam2017a")
		{
			Geometry= new Testbeam2017a();
		}
	else if(name=="gdml")
	{
		auto filename=GeometryName.substr(GeometryName.find(":")+1,GeometryName.size());
		if(filename=="")
			G4Exception("main","Geom002",FatalException,"GDML geometry chosen but no filename given.");
		Geometry= new GDMLReader(filename);
	}
	if(!Geometry){
		G4Exception("main","Geom001",JustWarning,"No geometry chosen. Loading default geometry.");
		Geometry= new SingleCrystal();
	}

	return Geometry;
}
