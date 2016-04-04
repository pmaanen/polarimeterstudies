/*
 * JediPolarimeter.cc
 *
 *  Created on: 20.01.2015
 *      Author: pmaanen
 */
#include "JediMainVolume.hh"
#include "JediBeamPipe.hh"
#include "JediTargetChamber.hh"
#include "Colors.hh"
#include <G4UnionSolid.hh>
#include <fstream>
JediMainVolume::JediMainVolume(std::string _infile):fInfileName(_infile) {
	G4String el[]={"Lu","Y","Si","O","Ce"};
	std::vector<G4String> elements(el, el + sizeof(el) / sizeof(G4String) );
	G4double we[]={71.43*CLHEP::perCent,4.03*CLHEP::perCent,6.37*CLHEP::perCent,18.14*CLHEP::perCent,0.02*CLHEP::perCent};
	std::vector<G4double> weights(we, we + sizeof(we) / sizeof(G4double) );

	G4NistManager::Instance()->ConstructNewMaterial("LYSO",elements,weights,7.1*CLHEP::g/CLHEP::cm3);
	fWorldSizeXY=2*CLHEP::m;
	fWorldSizeZ=10*CLHEP::m;
	try{
		fScintillatorMaterialName=gConfig["detector.scintillatorMaterial"].as<std::string>();
		fThetaMin=gConfig["detector.thetamin"].as<double>()*CLHEP::mm*CLHEP::deg;
		fThetaMax=gConfig["detector.thetamax"].as<double>()*CLHEP::mm*CLHEP::deg;
		fBeampipeRadius=gConfig["detector.beampipeRadius"].as<double>()*CLHEP::mm;
		fBeampipeThickness=gConfig["detector.beampipeThickness"].as<double>()*CLHEP::mm;
		fCrystalLength=gConfig["detector.crystalLength"].as<double>()*CLHEP::mm;
		fCrystalWidth=gConfig["detector.crystalWidth"].as<double>()*CLHEP::mm;
	}
	catch(const std::exception& e){
		std::cout<<"exception in JediPolarimeter::JediPolarimeter: "<<e.what()<<std::endl;
		exit(1);
	}
	fScintillatorMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);
	fDeltaELength=1*CLHEP::cm;
	fDeltaEWidth=fCrystalWidth;

	fTargetChamberThickness=2*CLHEP::mm;

	fWrappingThickness=100*CLHEP::um;
	fSafetyDistance=.1*CLHEP::cm;
	fTargetThickness=1*CLHEP::cm;
	fTargetWidth=1*CLHEP::cm;
	fChangedParameters=true;
	DefineCommands();
	ComputeParameters();

}

JediMainVolume::~JediMainVolume() {
	delete fMessenger;
}

void JediMainVolume::ComputeParameters() {

	DetectorZ = (fBeampipeRadius+fSafetyDistance) / tan( fThetaMin );

	fInnerDetectorRadius=DetectorZ*tan( fThetaMin );
	fOuterDetectorRadius=(DetectorZ+fCrystalLength)*tan( fThetaMax );

	fMaxCrystal=ceil(fOuterDetectorRadius/fCrystalWidth);
	fMinCrystal=ceil(fInnerDetectorRadius/fCrystalWidth);

	fTargetChamberZ1=fBeampipeRadius/ tan(fThetaMax)-1*CLHEP::cm;
	fTargetChamberZ2=DetectorZ-fDeltaELength-1*CLHEP::cm;

	fDeltaEZ=DetectorZ-5*CLHEP::cm;

	fWorldSizeXY=2*fOuterDetectorRadius+0.5*CLHEP::m;
	fWorldSizeZ=2*(DetectorZ+fCrystalLength)+0.5*CLHEP::m;

	fChangedParameters=false;
}

void JediMainVolume::DefineCommands() {

	fMessenger = new G4GenericMessenger(this,
			"/PolarimeterStudies/detector/",
			"detector control");

	G4GenericMessenger::Command& thetaMinCmd
	= fMessenger->DeclareMethodWithUnit("thetamin","deg",
			&JediMainVolume::setThetaMin,
			"theta min (degrees)");

	thetaMinCmd.SetParameterName("thetamin", true);
	thetaMinCmd.SetRange("thetamin>=0.");
	thetaMinCmd.SetDefaultValue("5.");

	G4GenericMessenger::Command& thetaMaxCmd
	= fMessenger->DeclareMethodWithUnit("thetamax","deg",
			&JediMainVolume::setThetaMax,
			"theta max (degrees)");
	thetaMaxCmd.SetParameterName("thetamax", true);
	thetaMaxCmd.SetRange("thetamax>=0.");
	thetaMaxCmd.SetDefaultValue("20.");

	G4GenericMessenger::Command& beampipeRadiusCmd
	= fMessenger->DeclareMethodWithUnit("bpRadius","mm",
			&JediMainVolume::setBeampipeRadius,
			"beampipe radius (mm)");

	beampipeRadiusCmd.SetParameterName("bpRadius", true);
	beampipeRadiusCmd.SetRange("bpRadius>=0.");
	beampipeRadiusCmd.SetDefaultValue("100.");


	G4GenericMessenger::Command& crystalLengthCmd
	= fMessenger->DeclareMethodWithUnit("calolength","mm",
			&JediMainVolume::setCrystalLength,
			"crystal length (mm)");

	crystalLengthCmd.SetParameterName("length", true);
	crystalLengthCmd.SetRange("length>=0.");
	crystalLengthCmd.SetDefaultValue("100.");

	G4GenericMessenger::Command& crystalWidthCmd
	= fMessenger->DeclareMethodWithUnit("calowidth","mm",
			&JediMainVolume::setCrystalWidth,
			"crystal width (mm)");

	crystalWidthCmd.SetParameterName("width", true);
	crystalWidthCmd.SetRange("width>=0.");
	crystalWidthCmd.SetDefaultValue("30.");

	G4GenericMessenger::Command& updateCmd
	= fMessenger->DeclareMethod("update",&JediMainVolume::UpdateGeometry,"Update geometry");

	G4GenericMessenger::Command& checkCmd
	= fMessenger->DeclareMethod("check",&JediMainVolume::checkGeometry,"check geometry for overlaps");


	G4GenericMessenger::Command& matCmd
	= fMessenger->DeclareMethod("material",
			&JediMainVolume::setCaloMaterialName,
			"scintillator material");

	G4GenericMessenger::Command& dEWidthCmd
	= fMessenger->DeclareMethodWithUnit("dEwidth","mm",
			&JediMainVolume::setDeltaEwidth,
			"delta E width (mm)");

	G4GenericMessenger::Command& dELengthCmd
	= fMessenger->DeclareMethodWithUnit("dElength","mm",
			&JediMainVolume::setDeltaElength,
			"delta E width (mm)");

	G4GenericMessenger::Command& targetThicknessCmd
	= fMessenger->DeclareMethodWithUnit("tgtThickness","mm",
			&JediMainVolume::setTargetThickness,
			"");

	targetThicknessCmd.SetParameterName("Thickness", true);
	targetThicknessCmd.SetRange("Thickness>=0.");
	targetThicknessCmd.SetDefaultValue("10.");

	G4GenericMessenger::Command& targetWidthCmd
	= fMessenger->DeclareMethodWithUnit("tgtWidth","mm",
			&JediMainVolume::setTargetWidth,
			"");

	targetWidthCmd.SetParameterName("Width", true);
	targetWidthCmd.SetRange("Width>=0.");
	targetWidthCmd.SetDefaultValue("10.");

	matCmd.SetParameterName("material", true);
	matCmd.SetStates(G4State_Idle);

	G4GenericMessenger::Command& dumpCmd
	= fMessenger->DeclareMethod("dump",&JediMainVolume::WriteWorldToFile,"dump geometry to file");

	dumpCmd.SetParameterName("filename",true);

	return;

}

G4VPhysicalVolume* JediMainVolume::Construct() {
	if(fChangedParameters)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	if(fBuildBeampipe)
		new G4PVPlacement(0,G4ThreeVector(0,0,0),new JediBeamPipe(this),"Beampipe",fLogicWorld,false,0,false);
	if(fBuildTargetChamber)
		new G4PVPlacement(0,G4ThreeVector(0,0,fTargetChamberZ1+0.5*(fTargetChamberZ2-fTargetChamberZ1)),new JediTargetChamber(this),"TargetChamber",fLogicWorld,false,0,false);
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	auto carbon=G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
	/*
	if(fBuildTarget)
		new G4PVPlacement(0,G4ThreeVector(0,0,fTargetThickness/2),new JediTarget(this),"Target",logicWorld,0,false,0);
	 */
	return fPhysiWorld;
}

void JediMainVolume::WriteWorldToFile(G4String filename) {

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
		for(auto line:fGeomCache){
			out<<line<<std::endl;
		}
	}

}

void JediMainVolume::UpdateGeometry(){

	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
	G4RegionStore::GetInstance()->UpdateMaterialList(fPhysiWorld);
	G4RunManager::GetRunManager()->ReinitializeGeometry();

}

void JediMainVolume::ConstructSDandField() {


	for(auto iVol: fPerfectSDVolumes){
		if (fTrackerSD[iVol.first].Get()==0)
			fTrackerSD[iVol.first].Put(new PerfectDetector(iVol.first,iVol.first));
		SetSensitiveDetector(iVol.second,fTrackerSD[iVol.first].Get());
	}

	for(auto iVol: fCaloSDVolumes){
		if (fCaloSD[iVol.first].Get()==0)
			fCaloSD[iVol.first].Put(new CaloSensitiveDetector(iVol.first));
		SetSensitiveDetector(iVol.second,fCaloSD[iVol.first].Get());
	}

	for(auto iVol: fTrackerSDVolumes){
		if (fTrackerSD[iVol.first].Get()==0)
			fTrackerSD[iVol.first].Put(new TrackerSensitiveDetector(iVol.first,iVol.first));
		SetSensitiveDetector(iVol.second,fTrackerSD[iVol.first].Get());
	}
}
