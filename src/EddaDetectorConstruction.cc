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



#include "G4Material.hh"

//***** include basic geometry classes
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Para.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"
#include "G4Torus.hh"
//***** End of include basic geometry classes

#include "TMath.h"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SDManager.hh"
#include "EddaDetectorConstruction.hh"

using namespace CLHEP;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EddaDetectorConstruction::EddaDetectorConstruction()
  :logicWorld(0), physiWorld(0), LogicalTube(0), PhysicalTube(0), LogicalSiPM(0), PhysicalSiPM(0), LogicalTarget(0), PhysicalTarget(0)
{;}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EddaDetectorConstruction::~EddaDetectorConstruction()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* EddaDetectorConstruction::Construct()
{
	DefineMaterials();
	DefineMaterialProperties();
	return ConstructSetUp();
}

void EddaDetectorConstruction::DefineMaterials()
{
	G4NistManager* man = G4NistManager::Instance();
	man->SetVerbose(2);

	G4String name, symbol;
	G4double A, Z, density, a, z;
	G4double temperature, pressure;
	G4int ncomponents, natoms;

	// Define Elements
	a = 12.01*g/mole;
	G4Element* C = new G4Element(name = "Carbon", symbol = "C", z = 6, a);
	a = 1.01*g/mole;
	G4Element* H = new G4Element(name = "Hydrogen", symbol = "H", z = 1, a);
	G4Element* Si = man->FindOrBuildElement("Si");

	// Vaccum material
	name        = "Vaccum";
	density     = universe_mean_density;
	pressure    = 3.e-18*pascal;
	temperature = 2.73*kelvin;
	Z=1.;
	A=1.01*g/mole;
	Vacuum = new G4Material(name, Z, A , density, kStateGas,temperature,pressure);

	//Air
	Air = man->FindOrBuildMaterial("G4_AIR");

	//Polystyrene 
 	Pstyrene = man->FindOrBuildMaterial("G4_POLYSTYRENE");

	//Fiber(PMMA)
	G4int polyPMMA = 1;
	std::vector<G4String> PMMA_elm; 
	PMMA_elm.push_back("H");
	PMMA_elm.push_back("C");
	PMMA_elm.push_back("O");
	std::vector<G4int> PMMA_nbAtoms; 
	PMMA_nbAtoms.push_back(3+2*polyPMMA);
	PMMA_nbAtoms.push_back(6+2*polyPMMA);
	PMMA_nbAtoms.push_back(2);

	PMMA = man->ConstructNewMaterial("PMMA", PMMA_elm, PMMA_nbAtoms, density=1190*kg/m3);

	//Cladding(polyethylene)   G4_POLYETHYLENE
	Pethylene = man->FindOrBuildMaterial("G4_POLYETHYLENE");
	
	//Double cladding(fluorinated polyethylene)
	fPethylene = new G4Material("fPethylene", density=1400*kg/m3,1);
	fPethylene->AddMaterial(Pethylene,1.);

	//SiPm material
	SiPMMaterial = new G4Material("SiPM", density=2.33*g/m3,1);
	SiPMMaterial->AddElement(Si,1);

	MirrorMaterial = man->FindOrBuildMaterial("G4_GLASS_PLATE");

	// Scintilator material 
	density = 1.032*g/cm3;
	pScint = new G4Material(name="pScintillator", density, ncomponents=2);
	pScint->AddElement(C, natoms=9);
	pScint->AddElement(H, natoms=10);
	
	TargetMaterial = man->FindOrBuildMaterial("G4_POLYCARBONATE");
	//TargetMaterial = man->FindOrBuildMaterial("G4_H");


	//G4cout << *(G4Material::GetMaterialTable()) << G4endl;


}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EddaDetectorConstruction::DefineMaterialProperties(){
// 	const G4int NUMENTRIES = 2;
// 	G4double pScint_PP[NUMENTRIES]    = { 171.2068*eV, 331*eV };
// 	G4double pScint_SCINT[NUMENTRIES] = { 1.0, 1.0 };
// 	G4double pScint_RIND[NUMENTRIES]  = { 1.57, 1.57 };
// 	G4double pScint_ABSL[NUMENTRIES]  = { 1500*cm, 2500*cm};
// 	G4MaterialPropertiesTable *pScint_mt = new G4MaterialPropertiesTable();
// 	pScint_mt->AddProperty("SCINTILLATION", pScint_PP, pScint_SCINT, NUMENTRIES);
// 	pScint_mt->AddProperty("RINDEX",        pScint_PP, pScint_RIND,  NUMENTRIES);
// 	pScint_mt->AddProperty("ABSLENGTH",     pScint_PP, pScint_ABSL,  NUMENTRIES);
// 	pScint->SetMaterialPropertiesTable(pScint_mt);

	const G4int VAC_NUMENTRIES = 3;

	G4double Vacuum_Energy[VAC_NUMENTRIES]={2.0*eV,7.0*eV,7.14*eV};
	G4double Vacuum_RIND[VAC_NUMENTRIES]={1.,1.,1.};
	G4MaterialPropertiesTable *Vacuum_mt = new G4MaterialPropertiesTable();
	Vacuum_mt->AddProperty("RINDEX", Vacuum_Energy, Vacuum_RIND,VAC_NUMENTRIES);
	Vacuum->SetMaterialPropertiesTable(Vacuum_mt);
	Air->SetMaterialPropertiesTable(Vacuum_mt);//Give air the same rindex

	const G4int FIBER_NUMENTRIES = 4;
	G4double FIBER_Energy[] = {2.00*eV, 2.87*eV, 2.90*eV, 3.47*eV};

	MPTPStyrene = new G4MaterialPropertiesTable();
	Pstyrene->SetMaterialPropertiesTable(MPTPStyrene);

	G4double RIndexPstyrene[FIBER_NUMENTRIES] = {1.5, 1.5, 1.5, 1.5};
	MPTPStyrene->AddProperty("RINDEX", FIBER_Energy, RIndexPstyrene, FIBER_NUMENTRIES);
	G4double Absorption1[FIBER_NUMENTRIES]={2.*cm, 2.*cm, 2.*cm, 2.*cm};
	MPTPStyrene->AddProperty("ABSLENGTH",FIBER_Energy,Absorption1,FIBER_NUMENTRIES);

 	G4double ScintilFast[FIBER_NUMENTRIES]={0.00, 0.00, 1.00, 1.00}; 
	MPTPStyrene->AddProperty("FASTCOMPONENT",FIBER_Energy, ScintilFast,FIBER_NUMENTRIES);
	MPTPStyrene->AddConstProperty("SCINTILLATIONYIELD",10./keV);
	MPTPStyrene->AddConstProperty("RESOLUTIONSCALE",1.0);
	MPTPStyrene->AddConstProperty("FASTTIMECONSTANT", 10.*ns);

	// Set the Birks Constant for the Polystyrene scintillator
	Pstyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
	
	G4double RefractiveIndexClad1[FIBER_NUMENTRIES]={ 1.49, 1.49, 1.49, 1.49};
	G4MaterialPropertiesTable* MPTClad1 = new G4MaterialPropertiesTable();
	MPTClad1->AddProperty("RINDEX", FIBER_Energy, RefractiveIndexClad1, FIBER_NUMENTRIES);
	MPTClad1->AddProperty("ABSLENGTH", FIBER_Energy, Absorption1, FIBER_NUMENTRIES);
	Pethylene->SetMaterialPropertiesTable(MPTClad1);
	
	G4double RefractiveIndexClad2[FIBER_NUMENTRIES]={ 1.42, 1.42, 1.42, 1.42};
	G4MaterialPropertiesTable* MPTClad2 = new G4MaterialPropertiesTable();
	MPTClad2->AddProperty("RINDEX", FIBER_Energy, RefractiveIndexClad2, FIBER_NUMENTRIES);
	MPTClad2->AddProperty("ABSLENGTH", FIBER_Energy, Absorption1, FIBER_NUMENTRIES);
	fPethylene->SetMaterialPropertiesTable(MPTClad2);

	const G4int MIRROR_NUMENTRIES = 2;
	G4double MirrorEnergy[MIRROR_NUMENTRIES] = {2*eV, 4*eV};
	G4double RefractiveIndexMirror[MIRROR_NUMENTRIES] = {1.4, 1.4};
	G4MaterialPropertiesTable* MPTMirror = new G4MaterialPropertiesTable();
	MPTMirror->AddProperty("RINDEX", MirrorEnergy, RefractiveIndexMirror, MIRROR_NUMENTRIES);
	MirrorMaterial->SetMaterialPropertiesTable(MPTMirror);
	
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4LogicalVolume * EddaDetectorConstruction::ConstructFiber(G4double Rmax, G4double Lz, G4double frac)
{
	// dimensions
	G4double fiber_rmin,fiber_rmax,fiber_z,fiber_sphi,fiber_ephi;
	G4double clad1_rmin,clad1_rmax,clad1_z,clad1_sphi,clad1_ephi;
	G4double clad2_rmin,clad2_rmax,clad2_z,clad2_sphi,clad2_ephi;

	fiber_rmin = 0.00*cm;
	fiber_rmax = Rmax*(1-2*frac);
	fiber_z    = Lz/2;
	fiber_sphi = 0.00*deg;
	fiber_ephi = 360.*deg;

	clad1_rmin = 0.;// fiber_rmax;
	clad1_rmax = fiber_rmax + frac*Rmax;

	clad1_z    = fiber_z;
	clad1_sphi = fiber_sphi;
	clad1_ephi = fiber_ephi; 

	clad2_rmin = 0.;//clad1_rmax;
	clad2_rmax = clad1_rmax + frac*Rmax;

	clad2_z    = fiber_z;
	clad2_sphi = fiber_sphi;
	clad2_ephi = fiber_ephi;

	// The Fiber
	//
	G4Tubs* Fiber_tube = new G4Tubs("Fiber", fiber_rmin, fiber_rmax, fiber_z, fiber_sphi, fiber_ephi);

	G4LogicalVolume* Fiber_log = new G4LogicalVolume(Fiber_tube, Pstyrene, "Fiber", 0, 0, 0);

	// Cladding (first layer)
	//
	G4Tubs* clad1_tube = new G4Tubs("Cladding1", clad1_rmin, clad1_rmax, clad1_z, clad1_sphi, clad1_ephi);

	G4LogicalVolume* clad1_log = new G4LogicalVolume(clad1_tube, Pethylene, "Cladding1", 0, 0, 0);

	// Cladding (second layer)
	//
	G4Tubs* clad2_tube = new G4Tubs("Cladding2", clad2_rmin, clad2_rmax, clad2_z, clad2_sphi, clad2_ephi);

	G4LogicalVolume *clad2_log = new G4LogicalVolume(clad2_tube, fPethylene, "Cladding2", 0, 0, 0);

	new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), Fiber_log, "Fiber", clad1_log, false, 0);
	new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), clad1_log, "Cladding1", clad2_log, false, 0);
	
	return clad2_log;
}
// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* EddaDetectorConstruction::ConstructSetUp()
{
	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	//     
	// World
	//

	G4double WorldSizeX = 1 *m;
	G4double WorldSizeY = 1 *m;
	G4double WorldSizeZ = 2 *m;

	G4Box* solidWorld = new G4Box("World",			//its name
					WorldSizeX/2,WorldSizeY/2,WorldSizeZ/2);	//its size

	logicWorld = new G4LogicalVolume(solidWorld,	//its solid
									Vacuum,			//defaultMaterial,
									"World");		//its name

	physiWorld = new G4PVPlacement(0,			//no rotation
							G4ThreeVector(),	//at (0,0,0)
							"World",			//its name
							logicWorld,			//its logical volume
							0,					//its mother  volume
							false,				//no boolean operation
							0);					//copy number


	//Target
	G4double TargetSizeX = 1*cm;
	G4double TargetSizeY = 15*mm;
	G4double TargetSizeZ = 0.5*mm;

	G4double TargetPositionX = 0*cm;
	G4double TargetPositionY = 0*cm;
	G4double TargetPositionZ = 0*cm;
	
	G4Box* solidTarget = new G4Box("SolidTarget", TargetSizeX/2, TargetSizeY/2, TargetSizeZ/2);
	LogicalTarget = new G4LogicalVolume(solidTarget, TargetMaterial, "LogicalTarget");
	PhysicalTarget = new G4PVPlacement(0, G4ThreeVector(TargetPositionX, TargetPositionY, TargetPositionZ),"PhysicalTarget", LogicalTarget, physiWorld, false, 0);

	//EDDA
/*
# accepted z-range (mm):   0.0 -   0.0
# ring geometry used for calculating prediction:
# ring #  z (mm)   dz(mm)  low r   up r   low z  up z
#    1	 55.790	  8.880	175.440	171.000	 55.790	 64.670
#    2	 64.670	  8.880	175.440	171.000	 64.670	 73.550
#    3	 73.550	  8.890	175.440	171.000	 73.550	 82.440
#    4	 82.440	  8.880	175.440	171.000	 82.440	 91.320
#    5	 91.320	  8.880	175.440	171.000	 91.320	100.200
#    6	100.200	  8.890	175.440	171.000	100.200	109.090
#    7	109.090	  8.880	175.440	171.000	109.090	117.970
#    8	117.970	  8.880	175.440	171.000	117.970	126.850
#    9	126.850	  9.464	175.440	171.000	126.850	136.314
#   10	136.314	 14.630	194.200	171.000	136.314	150.944
#   11	150.944	 16.221	194.200	171.000	150.944	167.164
#   12	167.164	 17.984	194.200	171.000	167.164	185.148
#   13	185.148	 19.939	194.200	171.000	185.148	205.087
#   14	205.087	 22.107	194.200	171.000	205.087	227.194
#   15	227.194	 24.510	194.200	171.000	227.194	251.704
#   16	251.704	 27.175	194.200	171.000	251.704	278.878
#   17	278.878	 30.129	194.200	171.000	278.878	309.007
#   18	309.007	 33.404	194.200	171.000	309.007	342.411
#   19	342.411	 37.036	194.200	171.000	342.411	379.447
#   20	379.447	 41.062	194.200	171.000	379.447	420.509
#   21	420.509	 45.526	194.200	171.000	420.509	466.035
#   22	466.035	 50.475	194.200	171.000	466.035	516.510
#   23	516.510	 55.963	194.200	171.000	516.510	572.472
#   24	572.472	 62.046	194.200	171.000	572.472	634.519
#   25	634.519	 68.792	194.200	171.000	634.519	703.311
#   26	703.311	 76.270	194.200	171.000	703.311	779.581
#   27	779.581	 84.562	194.200	171.000	779.581	864.143
#   28	864.143	 93.755	194.200	171.000	864.143	957.898
#   29	957.898	 20.000	194.200	171.000	957.898	977.898
*/
	double RBar = 160.;
	double Rmin = 171.;
	double Rmax[29] = {0};
	for(int i=0;i<9;i++){
		Rmax[i] = 175.440;
	}
	for(int i=9;i<29;i++){
		Rmax[i] = 194.200;
	}

	double dZ[29] = {8.880, 8.880, 8.890, 8.880, 8.880, 8.890, 8.880, 8.880, 9.464, 14.630, 16.221, 17.984, 19.939, 22.107, 24.510, 27.175, 30.129, 33.404, 37.036, 41.062, 45.526, 50.475, 55.963, 62.046, 68.792, 76.270, 84.562, 93.755, 20.000};

	double lowZ[29] = {55.790, 64.670, 73.550, 82.440, 91.320, 100.200, 109.090, 117.970, 126.850, 136.314, 150.944, 167.164, 185.148, 205.087, 227.194, 251.704, 278.878, 309.007, 342.411, 379.447, 420.509, 466.035, 516.510, 572.472, 634.519, 703.311, 779.581, 864.143, 957.898};

	double upZ[29] = 	{64.670, 73.550, 82.440, 91.320, 100.200, 109.090, 117.970, 126.850, 136.314, 150.944, 167.164, 185.148, 205.087, 227.194, 251.704, 278.878, 309.007, 342.411, 379.447, 420.509, 466.035, 516.510, 572.472, 634.519, 703.311, 779.581, 864.143, 957.898, 977.898};

	double posZ[29] = {55.790, 64.670,  73.550, 82.440, 91.320, 100.200, 109.090, 117.970, 126.850, 136.314, 150.944, 167.164, 185.148, 205.087, 227.194, 251.704, 278.878, 309.007, 342.411, 379.447, 420.509, 466.035, 516.510, 572.472, 634.519, 703.311, 779.581, 864.143, 957.898};


	G4double EddaStart = 55.790*mm;

	G4double EddaZ = (upZ[28] - lowZ[0])*mm;
	G4double EddaRMax = Rmax[9]*mm;
	G4double EddaRMin = RBar*mm;
	G4double EddaPhiMin = 0.*deg;
	G4double EddaPhiMax = 360.*deg; 

	G4Tubs* solidEdda = new G4Tubs("solidEdda", EddaRMin, EddaRMax, EddaZ/2., EddaPhiMin, EddaPhiMax );
	LogicalEdda = new G4LogicalVolume(solidEdda, Pstyrene, "LogicalEdda");
	PhysicalEdda = new G4PVPlacement(0, G4ThreeVector(0, 0, (EddaStart + EddaZ/2.)), "PhysicalEdda", LogicalEdda, physiWorld, false, 0);

		
	G4Color
		yellow(1.0,1.0,0.0),
		red(1.0,0.0,0.0),
		white(1.0,1.0,1.0);


	//Setting SensitiveDetectors
	G4SDManager* SDman = G4SDManager::GetSDMpointer();

	G4String SDname;
	G4VisAttributes *visLogicalRingL1 = new G4VisAttributes(G4Color(0.5, 0.75, 0));
	G4VisAttributes *visLogicalRingL2 = new G4VisAttributes(G4Color(0.5, 0.5, 0));
	G4VisAttributes *visLogicalRingR1 = new G4VisAttributes(G4Color(1., 0.3, 0.3));
	G4VisAttributes *visLogicalRingR2 = new G4VisAttributes(G4Color(1., 0., 0.));

	for(int i=0; i<29; i++){
		//left ring
	G4cout<<"Left: "<<"Ring " <<i<<G4endl;
		G4double startPhi = 90.*deg;
		G4double deltaPhi = 180.*deg;
		G4Tubs * solidRingL = new G4Tubs("solidRingL", Rmin*mm, Rmax[i]*mm, dZ[i]*mm/2., startPhi, deltaPhi);
		G4LogicalVolume *LogicalRingL = new G4LogicalVolume(solidRingL, Pstyrene, "LogicalRingL");

		
		//right ring
		startPhi = 270.*deg;
		G4Tubs * solidRingR = new G4Tubs("solidRingR", Rmin*mm, Rmax[i]*mm, dZ[i]*mm/2., startPhi, deltaPhi);
		G4LogicalVolume *LogicalRingR = new G4LogicalVolume(solidRingR, Pstyrene, "LogicalRingR");

		if(i%2==0){
			LogicalRingL->SetVisAttributes(visLogicalRingL1);
			LogicalRingR->SetVisAttributes(visLogicalRingR2);
		}else{
			LogicalRingL->SetVisAttributes(visLogicalRingL2);
			LogicalRingR->SetVisAttributes(visLogicalRingR1);
		}

		new G4PVPlacement(0, G4ThreeVector(0,0,(lowZ[i] + dZ[i]/2.)*mm - EddaStart - EddaZ/2. ), "PhysicalRingL", LogicalRingL, PhysicalEdda, false, i);
		new G4PVPlacement(0, G4ThreeVector(0,0,(lowZ[i] + dZ[i]/2.)*mm - EddaStart - EddaZ/2. ), "PhysicalRingR", LogicalRingR, PhysicalEdda, false, i);

	}
	
	double angleStep = 360./32.*deg;	// 32 Bars around 360deg
	G4double dx1 = 64.9*mm;
	G4double dx2 = 0.*mm;
	G4double dy1 = 930.*mm;
	G4double dy2 = 930.*mm;
	G4double dz = tan(angleStep)*dx1/2.;

	G4Trd *solidBar = new G4Trd("solidBar", dx1/2., dx2/2., dy1/2., dy2/2., dz/2.);
	G4VisAttributes *visLogicalBar1 = new G4VisAttributes(G4Color(0., 0., 0.78));
	G4VisAttributes *visLogicalBar2 = new G4VisAttributes(G4Color(0., 0.25, 0.5));
	for(int i=0;i<32;i++){
		G4LogicalVolume *LogicalBar = new G4LogicalVolume(solidBar, Pstyrene, "LogicalBar");
		if(i%2==0){
			LogicalBar->SetVisAttributes( visLogicalBar1 );
		}else{
			LogicalBar->SetVisAttributes( visLogicalBar2 );
		}
		G4RotationMatrix *rot1 = new G4RotationMatrix((angleStep)*i+90.*deg+angleStep/2., 90*deg,0 );
		G4ThreeVector vec = G4ThreeVector(0, 160.*mm+dz/2., 0);
		vec.setPhi(360./32.*deg*i);
		vec.setR(160.*mm+dz/2.);
		new G4PVPlacement(rot1, vec, "PhysicalBar", LogicalBar, PhysicalEdda, false, i+1);
	}



/* Barrels
	G4double Barrel_1Rmin = 10*cm;
	G4double Barrel_1Rmax = Barrel_1Rmin + 5*mm;
	G4double Barrel_1z = 1*m;
	G4double Barrel_PhiMin = 0*deg;
	G4double Barrel_PhiMax = 360*deg;

	G4Tubs* solidBarrel_1 = new G4Tubs("solidBarrel_1", Barrel_1Rmin, Barrel_1Rmax, Barrel_1z/2., Barrel_PhiMin, Barrel_PhiMax); 
	LogicalBarrel_1 = new G4LogicalVolume(solidBarrel_1, Pstyrene, "LogicalBarrel_1");
	PhysicalBarrel_1 = new G4PVPlacement(0, G4ThreeVector(0,0,0), "PhysicalBarrel_1", LogicalBarrel_1, physiWorld, false, 0);

	G4double Rmax = 0.5*mm;

	G4Torus* solidTorus1 = new G4Torus("solidTorus1", 0*mm, Rmax, Barrel_1Rmin, 0*deg, 360*deg);
	LogicalTorus1 = new G4LogicalVolume(solidTorus1, Pstyrene, "LogicalTorus1");
	for(G4int i=0; i<Barrel_1z/(2*Rmax); i++){
		G4double ZPosition = -Barrel_1z/2 + i*2*Rmax;
 		new G4PVPlacement(0, G4ThreeVector(0, 0, ZPosition), "PhysicalTorus1", LogicalTorus1, PhysicalBarrel_1, false, i);
	}

	G4Tubs* solidTubs1 = new G4Tubs("solidTubs1", 0*mm, Rmax, Barrel_1z/2., 0*deg, 360*deg);
	LogicalTubs1 = new G4LogicalVolume(solidTubs1, Pstyrene, "LogicalTubs1");
	for(G4int i=0; i< 2*TMath::Pi()*(Barrel_1Rmin + 3*Rmax) / (2*Rmax); i++){
		G4ThreeVector position = G4ThreeVector(Barrel_1Rmin + 3*Rmax,0,0);
		position.setPhi(i*2*Rmax/(Barrel_1Rmin + 3*Rmax));
		new G4PVPlacement(0, position, "PhysicalTubs1", LogicalTubs1, PhysicalBarrel_1, false, i); 
	}


	G4double Barrel_2Rmin = 30*cm;
	G4double Barrel_2Rmax = Barrel_2Rmin + 5*mm;
	G4double Barrel_2z = 1*m;

	

	

	G4Tubs* solidBarrel_2 = new G4Tubs("solidBarrel_2", Barrel_2Rmin, Barrel_2Rmax, Barrel_2z/2., Barrel_PhiMin, Barrel_PhiMax); 
	LogicalBarrel_2 = new G4LogicalVolume(solidBarrel_2, Pstyrene, "LogicalBarrel_2");
	PhysicalBarrel_2 = new G4PVPlacement(0, G4ThreeVector(0,0,0), "PhysicalBarrel_2", LogicalBarrel_2, physiWorld, false, 0);

	Rmax = 0.5*mm;

	G4Torus* solidTorus2 = new G4Torus("solidTorus2", 0*mm, Rmax, Barrel_1Rmin, 0*deg, 360*deg);
	LogicalTorus2 = new G4LogicalVolume(solidTorus2, Pstyrene, "LogicalTorus2");
// 	for(G4int i=0; i<Barrel_2z/(2*Rmax); i++){
// 		G4double ZPosition = -Barrel_2z/2 + i*2*Rmax;
//  		new G4PVPlacement(0, G4ThreeVector(0, 0, ZPosition), "PhysicalTorus2", LogicalTorus2, PhysicalBarrel_2, false, i);
// 	}

	G4Tubs* solidTubs2 = new G4Tubs("solidTubs2", 0*mm, Rmax, Barrel_1z/2., 0*deg, 360*deg);
	LogicalTubs2 = new G4LogicalVolume(solidTubs2, Pstyrene, "LogicalTubs2");
// 	for(G4int i=0; i< 2*TMath::Pi()*(Barrel_2Rmin + 3*Rmax) / (2*Rmax); i++){
// 		G4ThreeVector position = G4ThreeVector(Barrel_2Rmin + 3*Rmax,0,0);
// 		position.setPhi(i*2*Rmax/(Barrel_2Rmin + 3*Rmax));
// 		new G4PVPlacement(0, position, "PhysicalTubs2", LogicalTubs2, PhysicalBarrel_2, false, i); 
// 	}

	//Setting SensitiveDetectors
	G4SDManager* SDman = G4SDManager::GetSDMpointer();

	G4String SDname;
	DetectorPhysSDBarrel* aBarrelSD = new DetectorPhysSDBarrel( SDname = "/Barrel_1" );
	SDman->AddNewDetector( aBarrelSD );
	//LogicalBarrel_1->SetSensitiveDetector( aBarrelSD );
	LogicalTubs1->SetSensitiveDetector( aBarrelSD );

	aBarrelSD = new DetectorPhysSDBarrel( SDname = "/Barrel_2" );
	SDman->AddNewDetector( aBarrelSD );
	LogicalBarrel_2->SetSensitiveDetector( aBarrelSD );
*/
	 

/*
	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	//

	G4double  Rmax   = 0.5*mm;
	G4double  Lz     = 10.*cm;
	G4int NoTubes = 64;
	G4double space = 0.02*mm;	//space between Fibers

	G4double Rges = Rmax + space/2;

	G4double SiPMOffset = 1*mm;
	G4double SiPMX = 1.*mm;
	G4double SiPMY = 1.*mm;
	G4double SiPMZ = .1*mm;
	G4int NoSiPMs = 16;

	
	G4double LayerX = NoTubes*Rges + Rges;
	G4double LayerY = (2+sqrt(3))*Rges;
	G4double LayerZ = Lz + 2*SiPMOffset + 2*SiPMZ;

	G4ThreeVector LayerTrans(LayerX/2 + 4*cm,0,10*cm);
	G4RotationMatrix* LayerRot = new G4RotationMatrix;
	LayerRot->rotateX(M_PI/2 *rad);


	G4Box* SolidLayer = new G4Box("solidLayer", LayerX/2, LayerY/2, LayerZ/2);
	G4LogicalVolume* LogicalLayer = new G4LogicalVolume(SolidLayer, Air, "logicLayer");
	G4VPhysicalVolume* PhysicalLayer = new G4PVPlacement(G4Transform3D(*LayerRot, LayerTrans), "physicalLayer", LogicalLayer, physiWorld, false, 0);

	LogicalTube = ConstructFiber(Rmax, Lz, 0.015);
	
	G4OpticalSurface* TubeWrap = new G4OpticalSurface("TubeWrap");
	TubeWrap->SetType(dielectric_metal);
	TubeWrap->SetFinish(polished);
	TubeWrap->SetModel(glisur);
	const G4int NUM = 2;	

	G4double pp[NUM] = {2.0*eV, 3.5*eV};
	G4double reflectivity[NUM] = {1., 1.};
	G4double efficiency[NUM] = {.0, .0};

	G4MaterialPropertiesTable* TubeWrapProperty = new G4MaterialPropertiesTable();
	TubeWrapProperty->AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
	TubeWrapProperty->AddProperty("EFFICIENCY",pp,efficiency,NUM);
	TubeWrap->SetMaterialPropertiesTable(TubeWrapProperty);

	G4Box* SolidSiPm = new G4Box("SiPM", SiPMX/2, SiPMY/2, SiPMZ/2);
	LogicalSiPM = new G4LogicalVolume(SolidSiPm, SiPMMaterial,"SiPM");

	G4SDManager* SDman = G4SDManager::GetSDMpointer();

	G4String SiPMSDname = "DetectorPhys/SiPM";
	DetectorPhysSDSiPM* aSiPMSD = new DetectorPhysSDSiPM( SiPMSDname );
	SDman->AddNewDetector( aSiPMSD );
	LogicalLayer->SetSensitiveDetector( aSiPMSD );


	for(G4int i=0;i<NoTubes;i++){
		G4double XPosition = 0;
		G4double YPosition = 0;
		if(i  < NoTubes/2.){
			XPosition = i * (2 * Rges) - (NoTubes-1)/2.*(Rges) ;
			YPosition = -sqrt(3)*Rges/2;
		}
		else if(i  >= NoTubes/2.){
			if(NoTubes%2==0){
				XPosition = (i-NoTubes/2) * (2* Rges) + Rges - (NoTubes-1)/2. * (Rges);
			}else{
				XPosition = (i-NoTubes/2) * (2* Rges) - Rges - (NoTubes-1)/2. * (Rges);
			}
			YPosition = sqrt(3)*Rges/2;
		}
		PhysicalTube = new G4PVPlacement(0, G4ThreeVector(XPosition, YPosition, 0), "PhysicalTube", LogicalTube, PhysicalLayer, false, i);

		new G4LogicalBorderSurface("ScintWrap", PhysicalTube, PhysicalLayer, TubeWrap);
	}
	//Positions of SiPMs front
	for(G4int i=0;i<NoSiPMs; i++){
		G4double XPosition = i*LayerX/NoSiPMs - LayerX/2 + LayerX/(NoSiPMs*2);
		G4double YPosition = 0;
		G4double ZPosition = Lz/2+SiPMZ/2 + SiPMOffset;	
		PhysicalSiPM = new G4PVPlacement(0, G4ThreeVector(XPosition, YPosition, ZPosition), "SiPM", LogicalSiPM, PhysicalLayer, false, i);
	}
*/

	//Layout option1: back with mirror
	/*G4Box* SolidMirror = new G4Box("Mirror", LayerX/2, LayerY/2, SiPMZ/2);
	G4LogicalVolume* LogicalMirror = new G4LogicalVolume(SolidMirror, MirrorMaterial,"Mirror");
	G4VPhysicalVolume* PhysicalMirror = new G4PVPlacement(0, G4ThreeVector(0, 0, -Lz/2 - SiPMZ/2), "Mirror", LogicalMirror, PhysicalLayer, false, 0);	

	G4OpticalSurface* FiberMirror = new G4OpticalSurface("FiberMirror");
	FiberMirror->SetType(dielectric_metal);
	//FiberMirror->SetFinish(polished);
	//FiberMirror->SetModel(glisur);
	const G4int NUMFM = 2;	

	G4double ppFM[NUMFM] = {2.0*eV, 3.5*eV};
	G4double reflectivityFM[NUMFM] = {1., 1.};
	G4double efficiencyFM[NUMFM] = {.0, .0};

	G4MaterialPropertiesTable* FiberMirrorProperty = new G4MaterialPropertiesTable();
	FiberMirrorProperty->AddProperty("REFLECTIVITY",ppFM,reflectivityFM,NUMFM);
	FiberMirrorProperty->AddProperty("EFFICIENCY",ppFM,efficiencyFM,NUMFM);
	FiberMirror->SetMaterialPropertiesTable(FiberMirrorProperty);

	new G4LogicalBorderSurface("FiberMirror", PhysicalMirror, PhysicalTube, FiberMirror);*/
	

	

	// Visualization attributes


	G4Color
		//red(1.0,0.0,0.0),
		//yellow(1.0,1.0,0.0),
		green(0.0,1.0,0.0),
		blue(0.0,0.0,1.0),
		brown(0.4,0.4,0.1),
		metal(204/255., 204/255, 255/255.),
		lblue(153/255., 255/255., 153/255.),
		lgreen(153/255. ,255/255. ,153/255.);

	LogicalTarget->SetVisAttributes(new G4VisAttributes(white));
	logicWorld->SetVisAttributes (G4VisAttributes::Invisible);

	LogicalEdda->SetVisAttributes(G4VisAttributes::Invisible); 
	
// 	LogicalBarrel_1->SetVisAttributes( new G4VisAttributes(metal));
// 
// 	LogicalBarrel_2->SetVisAttributes( new G4VisAttributes(lgreen));
// 	LogicalTubs1->SetVisAttributes( new G4VisAttributes(false, white) );
// 	LogicalTorus1->SetVisAttributes( new G4VisAttributes(false, white) );
	//LogicalTubs2->SetVisAttributes( G4VisAttributes::Invisible);
	//LogicalTorus2->SetVisAttributes( G4VisAttributes::Invisible);

	//LogicalTube->SetVisAttributes(new G4VisAttributes(lgreen));
	//LogicalSiPM->SetVisAttributes(new G4VisAttributes(red));
	return physiWorld;
}
