/*
 * SingleCrystal.cc
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#include "SingleCrystal.hh"
#include "CathodeSD.hh"
#include <G4UserLimits.hh>
#include <G4SubtractionSolid.hh>
static G4Colour
white   (1.0, 1.0, 1.0),  // white
gray    (0.5, 0.5, 0.5), // gray
black   (0.0, 0.0, 0.0), // black
red     (1.0, 0.0, 0.0), // red
green   (0.0, 1.0, 0.0), // green
blue    (0.0, 0.0, 1.0), // blue
tblue    (0.0, 0.0, 1.0, 0.5), // transparent blue
cyan    (0.0, 1.0, 1.0), // cyan
magenta (1.0, 0.0, 1.0), // magenta
yellow  (1.0, 1.0, 0.0), // yellow
tcyan    (0.0, 1.0, 1.0, 0.5), // transparent cyan
tmagenta (1.0, 0.0, 1.0, 0.5); // transparent magenta
using namespace CLHEP;
SingleCrystal::SingleCrystal():JediPolarimeter(),physiScint(0),physiAirGap(0),physiCathode(0),fTheta(0),fPhi(0),fPsi(0)  {
	airThickness=0.2*CLHEP::mm;
	greaseThickness=.1*mm;
	windowThickness=1*mm;
	cathodeThickness=1*cm;
	fCrystalLength=10*CLHEP::cm;
	fCrystalWidth=3*CLHEP::cm;

	DefineCommands();
	defineSurfaces();




	G4NistManager* man = G4NistManager::Instance();
	fScintillatorMaterial=man->FindOrBuildMaterial("LYSO");
	G4Element *C = man->FindOrBuildElement("C");
	G4Element *H = man->FindOrBuildElement("H");
	G4Element *Si = man->FindOrBuildElement("Si");
	G4Element *O = man->FindOrBuildElement("O");
	G4Element *Sb = man->FindOrBuildElement("Sb");
	G4Element *Rb = man->FindOrBuildElement("Rb");
	G4Element *Cs = man->FindOrBuildElement("Cs");
	//------------------------------------------------
	// Polydimethylsiloxane (Grease)
	G4Material* Polydimethylsiloxane = new G4Material("Polydimethylsiloxane", 0.97*g/cm3, 4, kStateLiquid);
	Polydimethylsiloxane->AddElement(Si, 1);
	Polydimethylsiloxane->AddElement(O, 1);
	Polydimethylsiloxane->AddElement(C, 2);
	Polydimethylsiloxane->AddElement(H, 6);
	G4MaterialPropertiesTable* polydimethylsiloxaneprop = new G4MaterialPropertiesTable();
	const G4int numentriespolydimethylsiloxane = 3;
	G4double polydimethylsiloxaneenergy[numentriespolydimethylsiloxane] = {1.2*eV, 3.1*eV, 6.5*eV};
	G4double polydimethylsiloxaneabsorp[numentriespolydimethylsiloxane] = {10.*cm, 10.*cm, 10.*cm};
	G4double polydimethylsiloxanerindex[numentriespolydimethylsiloxane] = {1.4, 1.4, 1.4};
	polydimethylsiloxaneprop->AddProperty("ABSLENGTH", polydimethylsiloxaneenergy, polydimethylsiloxaneabsorp, numentriespolydimethylsiloxane);
	polydimethylsiloxaneprop->AddProperty("RINDEX", polydimethylsiloxaneenergy, polydimethylsiloxanerindex, numentriespolydimethylsiloxane);
	Polydimethylsiloxane->SetMaterialPropertiesTable(polydimethylsiloxaneprop);
	//------------------------------------------------

	//------------------------------
	// Fused silica
	G4Material* FusedSilica = new G4Material("FusedSilica", 2.201*g/cm3, 2, kStateSolid);
	FusedSilica->AddElement(Si, 1);
	FusedSilica->AddElement(O, 2);
	G4MaterialPropertiesTable* fusedsilicaprop = new G4MaterialPropertiesTable();
	const G4int numentriesfusedsilica = 3;
	G4double fusedsilicaenergy[numentriesfusedsilica] = {1.2*eV, 3.1*eV, 6.5*eV};
	G4double fusedsilicaabsorp[numentriesfusedsilica] = {2.*m, 2.*m, 2.*m};
	G4double fusedsilicarindex[numentriesfusedsilica] = {1.56, 1.47, 1.45};
	fusedsilicaprop->AddProperty("ABSLENGTH", fusedsilicaenergy, fusedsilicaabsorp, numentriesfusedsilica);
	fusedsilicaprop->AddProperty("RINDEX", fusedsilicaenergy, fusedsilicarindex, numentriesfusedsilica);
	FusedSilica->SetMaterialPropertiesTable(fusedsilicaprop);
	//------------------------------

	//------------------------------
	// Bialkali Cathode (dummy)
	G4Material* BialkaliCathode = new G4Material("BialkaliCathode", 3*g/cm3, 3, kStateSolid);
	BialkaliCathode->AddElement(Sb, 1);
	BialkaliCathode->AddElement(Rb, 1);
	BialkaliCathode->AddElement(Cs, 1);
	G4MaterialPropertiesTable* bialkalicathodeprop = new G4MaterialPropertiesTable();
	//  const G4int numentriesbialkalicath = 2;
	//  G4double bialkalicathodeenergy[numentriesbialkalicath] = {1.2*eV, 6.5*eV};
	//  G4double bialkalicathodeabsorp[numentriesbialkalicath] = {1.e-6*mm, 1.e-6*mm}; // absorb all
	//  bialkalicathodeprop->AddProperty("ABSLENGTH", bialkalicathodeenergy, bialkalicathodeabsorp, numentriesbialkalicath);
	bialkalicathodeprop->AddProperty("RINDEX", fusedsilicaenergy, fusedsilicarindex, numentriesfusedsilica); // use values from window to prevent refraction
	BialkaliCathode->SetMaterialPropertiesTable(bialkalicathodeprop);

}

SingleCrystal::~SingleCrystal() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* SingleCrystal::MakeCaloCrystal() {

	auto totalModuleLength=(wrappingThickness+fCrystalLength+greaseThickness+windowThickness+cathodeThickness);
	auto solidDetector= new G4Box("Detector",fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2);

	auto solidMother= new G4Box("Module",(fCrystalWidth+2*wrappingThickness)/2,(fCrystalWidth+2*wrappingThickness)/2,totalModuleLength/2);
	auto logicMother= new G4LogicalVolume(solidMother,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),"Module");
	auto solidWrappingFull= new G4Box("WrappingFull",(fCrystalWidth+2*wrappingThickness)/2,(fCrystalWidth+2*wrappingThickness)/2,(fCrystalLength+wrappingThickness)/2);
	auto solidWrapping=new G4SubtractionSolid("Wrapping",solidWrappingFull,solidDetector,0,G4ThreeVector(0,0,-wrappingThickness/2));

	assert(fScintillatorMaterial);
	auto logicDetector = new G4LogicalVolume(solidDetector,fScintillatorMaterial,"Detector");
	//new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	//new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"Wrapping");
	G4Box* solidCathode= new G4Box("Cathode",fCrystalWidth/2,fCrystalWidth/2,cathodeThickness/2);
	G4LogicalVolume* logicCathode = new G4LogicalVolume(solidCathode,G4Material::GetMaterial("BialkaliCathode"),"Cathode");
	logicCathode->SetVisAttributes(new G4VisAttributes(G4Colour(0.5,0.5,0.5)));

	G4LogicalVolume* logicGrease=0;
	if(greaseThickness>0){
		auto solidGrease= new G4Box("Grease",fCrystalWidth/2,fCrystalWidth/2,greaseThickness/2);
		logicGrease= new G4LogicalVolume(solidGrease,G4Material::GetMaterial("Polydimethylsiloxane"),"Grease");
		logicGrease->SetVisAttributes(new G4VisAttributes(yellow));
	}
	G4LogicalVolume* logicWindow=0;
	if(windowThickness>0){
		G4Box* solidWindow= new G4Box("Window",fCrystalWidth/2,fCrystalWidth/2,windowThickness/2);
		logicWindow = new G4LogicalVolume(solidWindow,G4Material::GetMaterial("FusedSilica"),"Window");
		logicWindow->SetVisAttributes(new G4VisAttributes(tblue));
	}
	if(logicGrease)
		physiGrease=new G4PVPlacement(0,G4ThreeVector(0,0,-totalModuleLength/2+cathodeThickness+windowThickness+greaseThickness/2),logicGrease,"Grease",logicMother, false, 0 , false);
	if(logicWindow)
		physiWindow=new G4PVPlacement(0,G4ThreeVector(0,0,-totalModuleLength/2+cathodeThickness+windowThickness/2),logicWindow,"Window",logicMother, false, 0 , false);

	auto physiWrapping=new G4PVPlacement(0,G4ThreeVector(0,0,(windowThickness+greaseThickness+cathodeThickness)/2),logicWrapping,"Wrapping",logicMother,false,0,false);
	physiCathode=new G4PVPlacement(0,G4ThreeVector(0,0,-totalModuleLength/2+cathodeThickness/2),logicCathode,"Cathode",logicMother, false, 0 , false);
	physiScint=new G4PVPlacement(0,G4ThreeVector(0,0,-totalModuleLength/2+fCrystalLength/2+windowThickness+greaseThickness+cathodeThickness),logicDetector,"CaloCrystal",logicMother, false, 0 , false);
	//physiAirGap=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(new G4VisAttributes(tcyan));
	//logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);

	new G4LogicalSkinSurface("WrappingSurface",logicWrapping,airGroundAluminum);
	new G4LogicalSkinSurface("PhotoCathodeSurface",logicCathode,silicaCathodeMaterial);
	new G4LogicalSkinSurface("GreaseSurface",logicGrease,polishedAir);
	new G4LogicalSkinSurface("Window",logicCathode,polishedAir);

	/*
	//Wrapping to Scintillator
		G4LogicalBorderSurface* wrap2ScintSurface = 0;
		wrap2ScintSurface = new G4LogicalBorderSurface("wrap2ScintSurface", physiWrapping, physiScint, airGroundAluminum);
		/*
		////AirGap to Scintillator
		//G4LogicalBorderSurface* air2ScintSurface = 0;
		//air2ScintSurface = new G4LogicalBorderSurface("air2ScintSurface", physiScint, physiAirGap, groundAir);


	//Scintillator to Grease

	G4LogicalBorderSurface* scint2GreaseSurface = 0;
	if(physiGrease)
		scint2GreaseSurface = new G4LogicalBorderSurface("scint2WindowSurface", physiScint, physiGrease, polishedAir);

	//Grease to Window
	G4LogicalBorderSurface* grease2WindowSurface = 0;
	if(physiGrease and physiWindow)
		grease2WindowSurface = new G4LogicalBorderSurface("scint2WindowSurface", physiGrease, physiWindow, polishedAir);
	//Window to Cathode
	G4LogicalBorderSurface* window2CathSurface = 0;
	if(physiWindow)
		window2CathSurface=new G4LogicalBorderSurface("scint2CathSurface", physiWindow, physiCathode, silicaCathodeMaterial);
	 */
	fCaloSDVolumes["Cathode"]=logicCathode;
	logicMother->SetVisAttributes(G4VisAttributes::Invisible);
	fCaloSDVolumes["Calorimeter"]=logicDetector;
	return logicMother;
}

G4VPhysicalVolume* SingleCrystal::Construct() {
	if(fChangedParameters)
		ComputeParameters();

	auto solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	auto detectorLength=(wrappingThickness+fCrystalLength+greaseThickness+windowThickness+cathodeThickness);

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);
	G4PVPlacement* physiDetector=new G4PVPlacement (rot, G4ThreeVector(0,0,detectorLength/2-cathodeThickness-windowThickness-greaseThickness), aCrystal, "Crystal", fLogicWorld, false, 0, false);
	//World to Wrapping Surface
	G4LogicalBorderSurface* world2WrapSurface = 0;
	world2WrapSurface=  new G4LogicalBorderSurface("world2WrapSurface", physiDetector, fPhysiWorld, airGroundAluminum);
	// Set user cuts to avoid deadlocks
	G4double maxStep = 10.0*CLHEP::m, maxLength = 10.0*CLHEP::m, maxTime = 100.0*CLHEP::ns, minEkin = 0.5*CLHEP::eV;
	fLogicWorld->SetUserLimits(new G4UserLimits(maxStep,maxLength,maxTime,minEkin));
	return fPhysiWorld;
}


void SingleCrystal::ConstructSDandField() {
	if (CrystalSD.Get()==0)
		CrystalSD.Put(new CaloSensitiveDetector("Calorimeter"));
	SetSensitiveDetector(fCaloSDVolumes["Calorimeter"],CrystalSD.Get());


	if(CathodeSD.Get()==0)
		CathodeSD.Put(new CathodeSensitiveDetector("Cathode"));
	SetSensitiveDetector(fCaloSDVolumes["Cathode"],CathodeSD.Get());

}

void SingleCrystal::defineSurfaces() {

	G4double ener[2] = {.1*CLHEP::eV, 10.*CLHEP::eV};

	LYSOPolishedAirTeflon = new G4OpticalSurface("LYSOPolishedAirTeflon", unified);
	LYSOPolishedAirTeflon->SetType(dielectric_LUT);
	LYSOPolishedAirTeflon->SetModel(LUT);
	LYSOPolishedAirTeflon->SetFinish(polishedteflonair);

	LYSOGroundAirTeflon = new G4OpticalSurface("LYSOGroundAirTeflon", unified);
	LYSOGroundAirTeflon->SetType(dielectric_LUT);
	LYSOGroundAirTeflon->SetModel(LUT);
	LYSOGroundAirTeflon->SetFinish(groundteflonair);

	polishedAir = new G4OpticalSurface("polishedAir", unified);
	polishedAir->SetType(dielectric_dielectric);
	polishedAir->SetModel(unified);
	polishedAir->SetFinish(ground); // necessary to enable UNIFIED code
	polishedAir->SetSigmaAlpha(1.3 * CLHEP::degree); // Janecek2010


	groundAir = new G4OpticalSurface("groundAir", unified);
	groundAir->SetType(dielectric_dielectric);
	groundAir->SetModel(unified);
	groundAir->SetFinish(ground);
	groundAir->SetSigmaAlpha(0.2 * CLHEP::radian); // Janecek2010

	// common paint attributes
	G4double paintspecularlobe[2] = {1.0, 1.0};
	G4double paintspecularspike[2] = {0.0, 0.0};
	G4double paintbackscatter[2] = {0.0, 0.0};
	G4double paintrindex[2] = {1.61, 1.61}; // Janecek2010

	// white painted
	G4double whitepaintrefl[2] = {0.955, 0.955}; // Janecek2010

	polishedWhitePainted = new G4OpticalSurface("polishedWhitePainted", unified);
	polishedWhitePainted->SetType(dielectric_dielectric);
	polishedWhitePainted->SetModel(unified);
	polishedWhitePainted->SetFinish(groundbackpainted);
	polishedWhitePainted->SetSigmaAlpha(1.3 * CLHEP::degree); // Janecek2010
	G4MaterialPropertiesTable* polishedWhitePaintedProperty = new G4MaterialPropertiesTable();
	polishedWhitePaintedProperty->AddProperty("RINDEX",ener,paintrindex,2);
	polishedWhitePaintedProperty->AddProperty("SPECULARLOBECONSTANT",ener,paintspecularlobe,2);
	polishedWhitePaintedProperty->AddProperty("SPECULARSPIKECONSTANT",ener,paintspecularspike,2);
	polishedWhitePaintedProperty->AddProperty("BACKSCATTERCONSTANT",ener,paintbackscatter,2);
	polishedWhitePaintedProperty->AddProperty("REFLECTIVITY",ener,whitepaintrefl,2);
	polishedWhitePainted->SetMaterialPropertiesTable(polishedWhitePaintedProperty);

	groundWhitePainted = new G4OpticalSurface("groundWhitePainted", unified);
	groundWhitePainted->SetType(dielectric_dielectric);
	groundWhitePainted->SetModel(unified);
	groundWhitePainted->SetFinish(groundbackpainted);
	groundWhitePainted->SetSigmaAlpha(12.0 * CLHEP::degree); // Janecek2010
	G4MaterialPropertiesTable* groundWhitePaintedProperty = new G4MaterialPropertiesTable();
	groundWhitePaintedProperty->AddProperty("RINDEX",ener,paintrindex,2);
	groundWhitePaintedProperty->AddProperty("SPECULARLOBECONSTANT",ener,paintspecularlobe,2);
	groundWhitePaintedProperty->AddProperty("SPECULARSPIKECONSTANT",ener,paintspecularspike,2);
	groundWhitePaintedProperty->AddProperty("BACKSCATTERCONSTANT",ener,paintbackscatter,2);
	groundWhitePaintedProperty->AddProperty("REFLECTIVITY",ener,whitepaintrefl,2);
	groundWhitePainted->SetMaterialPropertiesTable(groundWhitePaintedProperty);

	// black painted
	G4double blackpaintrefl[2] = {0.043, 0.043}; // Dury2006

	polishedBlackPainted = new G4OpticalSurface("polishedBlackPainted", unified);
	polishedBlackPainted->SetType(dielectric_dielectric);
	polishedBlackPainted->SetModel(unified);
	polishedBlackPainted->SetFinish(groundbackpainted);
	polishedBlackPainted->SetSigmaAlpha(1.3 * CLHEP::degree); // Janecek2010
	G4MaterialPropertiesTable* polishedBlackPaintedProperty = new G4MaterialPropertiesTable();
	polishedBlackPaintedProperty->AddProperty("RINDEX",ener,paintrindex,2);
	polishedBlackPaintedProperty->AddProperty("SPECULARLOBECONSTANT",ener,paintspecularlobe,2);
	polishedBlackPaintedProperty->AddProperty("SPECULARSPIKECONSTANT",ener,paintspecularspike,2);
	polishedBlackPaintedProperty->AddProperty("BACKSCATTERCONSTANT",ener,paintbackscatter,2);
	polishedBlackPaintedProperty->AddProperty("REFLECTIVITY",ener,blackpaintrefl,2);
	polishedBlackPainted->SetMaterialPropertiesTable(polishedBlackPaintedProperty);

	groundBlackPainted = new G4OpticalSurface("groundBlackPainted", unified);
	groundBlackPainted->SetType(dielectric_dielectric);
	groundBlackPainted->SetModel(unified);
	groundBlackPainted->SetFinish(groundbackpainted);
	groundBlackPainted->SetSigmaAlpha(12.0 * CLHEP::degree); // Janecek2010
	G4MaterialPropertiesTable* groundBlackPaintedProperty = new G4MaterialPropertiesTable();
	groundBlackPaintedProperty->AddProperty("RINDEX",ener,paintrindex,2);
	groundBlackPaintedProperty->AddProperty("SPECULARLOBECONSTANT",ener,paintspecularlobe,2);
	groundBlackPaintedProperty->AddProperty("SPECULARSPIKECONSTANT",ener,paintspecularspike,2);
	groundBlackPaintedProperty->AddProperty("BACKSCATTERCONSTANT",ener,paintbackscatter,2);
	groundBlackPaintedProperty->AddProperty("REFLECTIVITY",ener,blackpaintrefl,2);
	groundBlackPainted->SetMaterialPropertiesTable(groundBlackPaintedProperty);

	// surface definition of the aluminum shell around the scintillation crystal
	airGroundAluminum = new G4OpticalSurface("airGroundAluminum", unified);
	airGroundAluminum->SetType(dielectric_metal);
	airGroundAluminum->SetModel(unified);
	airGroundAluminum->SetFinish(ground);
	airGroundAluminum->SetSigmaAlpha(0.2);
	G4MaterialPropertiesTable *airGroundAluminumProperty = new G4MaterialPropertiesTable();
	G4double refl[2] = {0.7, 0.7};
	airGroundAluminumProperty->AddProperty("REFLECTIVITY",ener,refl,2);
	airGroundAluminum->SetMaterialPropertiesTable(airGroundAluminumProperty);

	silicaCathodeMaterial = new G4OpticalSurface("silicaCathodeMaterial", unified);
	silicaCathodeMaterial->SetType(dielectric_metal);
	silicaCathodeMaterial->SetModel(unified);
	silicaCathodeMaterial->SetFinish(ground);
	silicaCathodeMaterial->SetSigmaAlpha(0.);
	G4MaterialPropertiesTable *silicaCathodeMaterialProperty = new G4MaterialPropertiesTable();
	G4double cathoderefl[2] = {0., 0.};
	G4double cathodeeff[2] = {1., 1.};
	silicaCathodeMaterialProperty->AddProperty("REFLECTIVITY",ener,cathoderefl,2);
	silicaCathodeMaterialProperty->AddProperty("EFFICIENCY",ener,cathodeeff,2);
	silicaCathodeMaterial->SetMaterialPropertiesTable(silicaCathodeMaterialProperty);
}

void SingleCrystal::DefineCommands() {
	JediPolarimeter::DefineCommands();

	G4GenericMessenger::Command& thetaCmd
	= fMessenger->DeclareMethodWithUnit("theta","deg",
			&SingleCrystal::setTheta,
			"set theta");

	G4GenericMessenger::Command& phiCmd
	= fMessenger->DeclareMethodWithUnit("phi","deg",
			&SingleCrystal::setPhi,
			"set phi");

	G4GenericMessenger::Command& psiCmd
	= fMessenger->DeclareMethodWithUnit("psi","deg",
			&SingleCrystal::setPsi,
			"set psi");

	G4GenericMessenger::Command& greaseCmd
	= fMessenger->DeclareMethodWithUnit("grease","mm",
			&SingleCrystal::setGreaseThickness,
			"set grease thickness");

	G4GenericMessenger::Command& windowCmd
	= fMessenger->DeclareMethodWithUnit("window","mm",
			&SingleCrystal::setWindowThickness,
			"set window thickness");

	G4GenericMessenger::Command& cathodeCmd
	= fMessenger->DeclareMethodWithUnit("cathode","mm",
			&SingleCrystal::setCathodeThickness,
			"set cathode thickness");

}
