/*
 * SingleCrystal.cc
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#include <SingleCrystal.hh>
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
SingleCrystal::SingleCrystal():JediPolarimeter() {
	crystalLength=500*CLHEP::cm;
	crystalWidth=10*CLHEP::cm;

	DefineCommands();
}

SingleCrystal::~SingleCrystal() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* SingleCrystal::MakeCaloCrystal() {
	/*
	G4Box* solidWrapping= new G4Box("Wrapping",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume*  logicWrapping= new G4LogicalVolume(solidWrapping,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Wrapping");
	G4Box* solidReflector= new G4Box("Wrapping",(crystalWidth-1*wrappingThickness)/2,(crystalWidth-1*wrappingThickness)/2,(crystalLength-1*wrappingThickness)/2);
	G4LogicalVolume*  logicReflector= new G4LogicalVolume(solidReflector,G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),"Reflector");
	 */

	G4Box* solidDetector= new G4Box("Detector",crystalWidth/2,crystalWidth/2,crystalLength/2);
	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector,scintillatorMaterial,"Detector");
	/*
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDetector,"CaloCrystal",logicReflector, false, 0 , false);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicReflector,"Reflector",logicWrapping,false,0,false);

	logicWrapping->SetVisAttributes(G4VisAttributes::Invisible);
	logicReflector->SetVisAttributes(G4VisAttributes::Invisible);
	 */
	G4VisAttributes* detectorVisAttr=new G4VisAttributes(green);
	logicDetector->SetVisAttributes(detectorVisAttr);
	caloSDVolumes["Calorimeter"]=logicDetector;
	return logicDetector;
}

G4VPhysicalVolume* SingleCrystal::Construct() {
	if(changedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",worldSizeXY/2,worldSizeXY/2,worldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);
	G4LogicalVolume* aCrystal=MakeCaloCrystal();
	new G4PVPlacement (0, G4ThreeVector(0,0,crystalLength/2), aCrystal, "Crystal", logicWorld, false, 0, false);

	return physiWorld;
}

void SingleCrystal::defineSurfaces() {

	G4double ener[2] = {.1*CLHEP::eV, 10.*CLHEP::eV};

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
