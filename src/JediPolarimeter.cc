/*
 * JediPolarimeter.cc
 *
 *  Created on: 20.01.2015
 *      Author: pmaanen
 */

#include <JediPolarimeter.hh>
#include <G4UnionSolid.hh>
#include <fstream>
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

JediPolarimeter::JediPolarimeter(std::string _infile):infile(_infile) {

	G4String el[]={"Lu","Y","Si","O","Ce"};
	std::vector<G4String> elements(el, el + sizeof(el) / sizeof(G4String) );
	G4double we[]={71.43*CLHEP::perCent,4.03*CLHEP::perCent,6.37*CLHEP::perCent,18.14*CLHEP::perCent,0.02*CLHEP::perCent};
	std::vector<G4double> weights(we, we + sizeof(we) / sizeof(G4double) );

	G4NistManager::Instance()->ConstructNewMaterial("G4_LYSO_SCINT",elements,weights,7.1*CLHEP::g/CLHEP::cm3);
	worldSizeXY=2*CLHEP::m;
	worldSizeZ=10*CLHEP::m;
	try{
		scintillatorMaterialName=gConfig["detector.scintillatorMaterial"].as<std::string>();
		thetaMin=gConfig["detector.thetamin"].as<double>()*CLHEP::mm*CLHEP::deg;
		thetaMax=gConfig["detector.thetamax"].as<double>()*CLHEP::mm*CLHEP::deg;
		beampipeRadius=gConfig["detector.beampipeRadius"].as<double>()*CLHEP::mm;
		beampipeThickness=gConfig["detector.beampipeThickness"].as<double>()*CLHEP::mm;
		crystalLength=gConfig["detector.crystalLength"].as<double>()*CLHEP::mm;
		crystalWidth=gConfig["detector.crystalWidth"].as<double>()*CLHEP::mm;
	}
	catch(const std::exception& e){
		std::cout<<"exception in JediPolarimeter::JediPolarimeter: "<<e.what()<<std::endl;
		exit(1);
	}
	scintillatorMaterial=G4NistManager::Instance()->FindOrBuildMaterial(scintillatorMaterialName);
	deltaELength=1*CLHEP::cm;
	deltaEWidth=crystalWidth;

	targetChamberThickness=2*CLHEP::mm;

	wrappingThickness=100*CLHEP::um;

	targetThickness=1*CLHEP::cm;
	targetWidth=1*CLHEP::cm;
	changedParameters=true;
	DefineCommands();
	ComputeParameters();

}

JediPolarimeter::~JediPolarimeter() {
	delete fMessenger;
}

void JediPolarimeter::ConstructSDandField() {}

void JediPolarimeter::ComputeParameters() {
	//crystalWidth+=2*CLHEP::mm;
	detectorZ = (beampipeRadius+5*CLHEP::mm) / tan( thetaMin );

	innerDetectorRadius=detectorZ*tan( thetaMin );
	outerDetectorRadius=detectorZ*tan( thetaMax );

	MaxCrystal=ceil(outerDetectorRadius/crystalWidth);
	MinCrystal=ceil(innerDetectorRadius/crystalWidth);

	targetChamberZ1=beampipeRadius/ tan(thetaMax)-1*CLHEP::cm;
	targetChamberZ2=detectorZ-deltaELength-1*CLHEP::cm;

	deltaEZ=detectorZ-5*CLHEP::cm;

	worldSizeXY=2*outerDetectorRadius+0.5*CLHEP::m;
	worldSizeZ=2*(detectorZ+crystalLength)+0.5*CLHEP::m;

	changedParameters=false;
}

G4LogicalVolume* JediPolarimeter::MakeBeampipe() {

	G4double windowThickness=0.1*CLHEP::mm;
	G4double foilThickness=0.1*CLHEP::mm;

	auto al=G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	auto mylar=G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR");
	auto uhv=G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

	G4Tubs* solidBeampipe=new G4Tubs("Beampipe",beampipeRadius-beampipeThickness,beampipeRadius,worldSizeZ/2-1*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicBeampipe = new G4LogicalVolume(solidBeampipe,al,"Beampipe");
	G4Tubs* solidMylarFoil=new G4Tubs("MylarFoil",beampipeRadius-beampipeThickness+windowThickness,beampipeRadius-beampipeThickness+foilThickness+windowThickness,(targetChamberZ2-targetChamberZ1)/2,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicMylarFoil=new G4LogicalVolume(solidMylarFoil,mylar,"MylarFoil");
	new G4PVPlacement(0,G4ThreeVector(0,0,targetChamberZ1+0.5*(targetChamberZ2-targetChamberZ1)),logicMylarFoil,"MylarFoil",logicBeampipe,0,false,0);
	G4Tubs* solidGap= new G4Tubs("Gap",beampipeRadius-beampipeThickness+foilThickness+windowThickness,beampipeRadius,(targetChamberZ2-targetChamberZ1)/2,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicGap=new G4LogicalVolume(solidGap,uhv,"Gap");
	new G4PVPlacement(0,G4ThreeVector(0,0,targetChamberZ1+0.5*(targetChamberZ2-targetChamberZ1)),logicGap,"Gap",logicBeampipe,0,false,0);

	logicGap->SetVisAttributes(G4VisAttributes::Invisible);
	G4VisAttributes* beampipeVisAttr = new G4VisAttributes(gray);
	logicBeampipe->SetVisAttributes(beampipeVisAttr);
	return logicBeampipe;
}

G4LogicalVolume* JediPolarimeter::MakeTargetChamber(){
	auto al=G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	//Do it with Cons+Tube
	auto windowRadius=targetChamberZ2*tan( thetaMax );
	auto exitWindowThickness=2*CLHEP::mm;
	G4Tubs* solidExitWindow=new G4Tubs("ExitWindow",beampipeRadius,windowRadius,exitWindowThickness/2,0,360*CLHEP::deg);
	G4double rInner1=beampipeRadius;
	G4double rOuter1=beampipeRadius+beampipeThickness;
	G4double rInner2=windowRadius;
	G4double rOuter2=windowRadius+beampipeThickness;
	G4Cons* solidConicalSection=new G4Cons("ConicalSection",rInner1,rOuter1,rInner2,rOuter2,(targetChamberZ2-targetChamberZ1)/2,0,360*CLHEP::deg);
	G4UnionSolid* solidTargetChamber= new G4UnionSolid("TargetChamber",solidConicalSection,solidExitWindow,0,G4ThreeVector(0,0,(targetChamberZ2-targetChamberZ1)/2));
	G4LogicalVolume* logicTargetChamber=new G4LogicalVolume(solidTargetChamber,al,"TargetChamber");
	caloSDVolumes["logicExitWindow"]=logicTargetChamber;
	return logicTargetChamber;
}

void JediPolarimeter::DefineCommands() {
	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/detector/",
			"detector control");

	G4GenericMessenger::Command& thetaMinCmd
	= fMessenger->DeclareMethodWithUnit("thetamin","deg",
			&JediPolarimeter::setThetaMin,
			"theta min (degrees)");

	thetaMinCmd.SetParameterName("thetamin", true);
	thetaMinCmd.SetRange("thetamin>=0.");
	thetaMinCmd.SetDefaultValue("5.");

	G4GenericMessenger::Command& thetaMaxCmd
	= fMessenger->DeclareMethodWithUnit("thetamax","deg",
			&JediPolarimeter::setThetaMax,
			"theta max (degrees)");
	thetaMaxCmd.SetParameterName("thetamax", true);
	thetaMaxCmd.SetRange("thetamax>=0.");
	thetaMaxCmd.SetDefaultValue("20.");

	G4GenericMessenger::Command& beampipeRadiusCmd
	= fMessenger->DeclareMethodWithUnit("bpRadius","mm",
			&JediPolarimeter::setBeampipeRadius,
			"beampipe radius (mm)");

	beampipeRadiusCmd.SetParameterName("bpRadius", true);
	beampipeRadiusCmd.SetRange("bpRadius>=0.");
	beampipeRadiusCmd.SetDefaultValue("100.");


	G4GenericMessenger::Command& crystalLengthCmd
	= fMessenger->DeclareMethodWithUnit("calolength","mm",
			&JediPolarimeter::setCrystalLength,
			"crystal length (mm)");

	crystalLengthCmd.SetParameterName("length", true);
	crystalLengthCmd.SetRange("length>=0.");
	crystalLengthCmd.SetDefaultValue("100.");

	G4GenericMessenger::Command& crystalWidthCmd
	= fMessenger->DeclareMethodWithUnit("calowidth","mm",
			&JediPolarimeter::setCrystalWidth,
			"crystal width (mm)");

	crystalWidthCmd.SetParameterName("width", true);
	crystalWidthCmd.SetRange("width>=0.");
	crystalWidthCmd.SetDefaultValue("30.");

	G4GenericMessenger::Command& updateCmd
	= fMessenger->DeclareMethod("update",&JediPolarimeter::UpdateGeometry,"Update geometry");

	G4GenericMessenger::Command& checkCmd
	= fMessenger->DeclareMethod("check",&JediPolarimeter::checkGeometry,"check geometry for overlaps");


	G4GenericMessenger::Command& matCmd
	= fMessenger->DeclareMethod("material",
			&JediPolarimeter::setCaloMaterialName,
			"scintillator material");

	G4GenericMessenger::Command& dEWidthCmd
	= fMessenger->DeclareMethodWithUnit("dEwidth","mm",
			&JediPolarimeter::setDeltaEwidth,
			"delta E width (mm)");

	G4GenericMessenger::Command& dELengthCmd
	= fMessenger->DeclareMethodWithUnit("dElength","mm",
			&JediPolarimeter::setDeltaElength,
			"delta E width (mm)");

	G4GenericMessenger::Command& targetThicknessCmd
	= fMessenger->DeclareMethodWithUnit("tgtThickness","mm",
			&JediPolarimeter::setTargetThickness,
			"");

	targetThicknessCmd.SetParameterName("Thickness", true);
	targetThicknessCmd.SetRange("Thickness>=0.");
	targetThicknessCmd.SetDefaultValue("10.");

	G4GenericMessenger::Command& targetWidthCmd
	= fMessenger->DeclareMethodWithUnit("tgtWidth","mm",
			&JediPolarimeter::setTargetWidth,
			"");

	targetWidthCmd.SetParameterName("Width", true);
	targetWidthCmd.SetRange("Width>=0.");
	targetWidthCmd.SetDefaultValue("10.");

	matCmd.SetParameterName("material", true);
	matCmd.SetStates(G4State_Idle);

	G4GenericMessenger::Command& dumpCmd
	= fMessenger->DeclareMethod("dump",&JediPolarimeter::WriteWorldToFile,"dump geometry to file");

	dumpCmd.SetParameterName("filename",true);

	return;

}

G4VPhysicalVolume* JediPolarimeter::Construct() {
	if(changedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",worldSizeXY/2,worldSizeXY/2,worldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	physiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),logicWorld,"World",0,0,0,0);
	new G4PVPlacement(0,G4ThreeVector(0,0,0),MakeBeampipe(),"Beampipe",logicWorld,false,0,false);
	new G4PVPlacement(0,G4ThreeVector(0,0,targetChamberZ1+0.5*(targetChamberZ2-targetChamberZ1)),MakeTargetChamber(),"TargetChamber",logicWorld,false,0,false);
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

	auto carbon=G4NistManager::Instance()->FindOrBuildMaterial("G4_C");

	G4Box* solidTarget=new G4Box("Target",targetWidth/2,targetWidth/2,targetThickness/2);
	G4LogicalVolume* logicTarget=new G4LogicalVolume(solidTarget,carbon,"CarbonTarget");
	//new G4PVPlacement(0,G4ThreeVector(0,0,targetThickness/2),logicTarget,"Target",logicWorld,0,false,0);

	return physiWorld;
}

void JediPolarimeter::WriteWorldToFile(G4String filename) {

	if(filename.contains(".gdml")){
		//TODO: gdml yaddayadda
	}
	else{
		std::streambuf * buf;
		std::ofstream outFile;
		if(filename!=""){
			outFile.open(filename.data());
			buf = outFile.rdbuf();
		}
		else{
			buf=std::cout.rdbuf();
		}
		std::ostream out(buf);
		for(auto line:geomCache){
			out<<line<<std::endl;
		}
	}

}

void JediPolarimeter::UpdateGeometry(){

	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
	G4RegionStore::GetInstance()->UpdateMaterialList(physiWorld);
	G4RunManager::GetRunManager()->ReinitializeGeometry();

}
