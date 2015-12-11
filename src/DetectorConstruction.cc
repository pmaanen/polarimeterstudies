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
#include "JediCubicPolarimeter.hh"
#include "JediHexagonalPolarimeter.hh"
#include "SingleCrystal.hh"
#include "JediSandwichCalorimeter.hh"
#include "CosmicSetup.hh"
#include "ClusterSetup.hh"
#include "global.hh"
#include <map>
using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction(),fGeometry(nullptr)
{
	auto geometry=gConfig["detector.geometry"].as<std::string>();
	std::string cubic("cubic:");
	std::string hexagonal("hexagonal:");
	std::string gdml("gdml:");
	std::string single("single:");
	std::string sandwich("sandwich:");
	std::string cosmic("cosmic:");
	std::string cluster("cluster:");
	if(!geometry.compare(0,cubic.size(),cubic)){
		fGeometry=new JediCubicPolarimeter(geometry.substr(cubic.size(),geometry.size()));
	}
	if(!geometry.compare(0,hexagonal.size(),hexagonal)){
		fGeometry=new JediHexagonalPolarimeter;
	}
//	if(!geometry.compare(0,gdml.size(),gdml)){
//		jedi= new DetectorConstruction();
//	}
	if(!geometry.compare(0,single.size(),single)){
		fGeometry= new SingleCrystal();
	}
	if(!geometry.compare(0,sandwich.size(),sandwich)){
		fGeometry= new JediSandwichCalorimeter();
	}
	if(!geometry.compare(0,cosmic.size(),cosmic)){
		fGeometry= new CosmicSetup();
	}
	if(!geometry.compare(0,cluster.size(),cluster)){
		fGeometry= new ClusterSetup();
	}
	if(!fGeometry)
		G4Exception("main","Geom001",FatalException,"No geometry chosen and no default geometry.");
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
	delete fGeometry;
}
