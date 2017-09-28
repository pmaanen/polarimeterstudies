/*
 * JediPolarimeter.cc
 *
 *  Created on: 20.01.2015
 *      Author: pmaanen
 */

#include <InternalBeampipe.hh>
#include "JediPolarimeter.hh"
#include "Colors.hh"
#include <G4UnionSolid.hh>
#include <G4AutoLock.hh>
#include <G4WorkerRunManager.hh>
#include <JediConfigurationManager.hh>
#include <fstream>
#include "Colors.hh"

JediPolarimeter::JediPolarimeter(std::string _infile):fInfileName(_infile) {
	if(JediConfigurationManager::Instance()->GetVerbose()>3)
		G4cout<<"JediPolarimeter::JediPolarimeter()"<<G4endl;
	G4String el[]={"Lu","Y","Si","O","Ce"};
	std::vector<G4String> elements(el, el + sizeof(el) / sizeof(G4String) );
	G4double we[]={71.43*CLHEP::perCent,4.03*CLHEP::perCent,6.37*CLHEP::perCent,18.14*CLHEP::perCent,0.02*CLHEP::perCent};
	std::vector<G4double> weights(we, we + sizeof(we) / sizeof(G4double) );

	fWorldMaterialName="G4_AIR";
	fDeltaELength=1*CLHEP::cm;
	fDeltaEWidth=fHCalSizeXY;
	fTargetChamberThickness=2*CLHEP::mm;
	fWrappingThickness=100*CLHEP::um;
	fSafetyDistance=1*CLHEP::mm;
	fTargetThickness=1*CLHEP::cm;
	fTargetWidth=1*CLHEP::cm;


	CopyPropertiesFromConfig();
	DefineMaterials();
	DefineCommands();
	ComputeParameters();
	fTarget=nullptr;
	fHCalMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);
}

JediPolarimeter::~JediPolarimeter() {
	if(JediConfigurationManager::Instance()->GetVerbose()>3)
		G4cout<<"JediPolarimeter::~JediPolarimeter()"<<G4endl;
	delete fMessenger;
}

void JediPolarimeter::CopyPropertiesFromConfig() {

	try{
		fScintillatorMaterialName=JediConfigurationManager::Instance()->GetMap()["detector.scintillator_material"].as<std::string>();
		fThetaMin=JediConfigurationManager::Instance()->GetMap()["detector.theta_min"].as<double>()*CLHEP::mm*CLHEP::deg;
		fThetaMax=JediConfigurationManager::Instance()->GetMap()["detector.theta_max"].as<double>()*CLHEP::mm*CLHEP::deg;
		fBeampipeRadius=JediConfigurationManager::Instance()->GetMap()["detector.beampipe_radius"].as<double>()*CLHEP::mm;
		fBeampipeThickness=JediConfigurationManager::Instance()->GetMap()["detector.beampipe_thickness"].as<double>()*CLHEP::mm;
		fHCalSizeXY=JediConfigurationManager::Instance()->GetMap()["detector.hcal_size_xy"].as<double>()*CLHEP::mm;
		fHCalSizeZ=JediConfigurationManager::Instance()->GetMap()["detector.hcal_size_z"].as<double>()*CLHEP::mm;
	}
	catch(const std::exception& e){
		std::cout<<"exception in JediPolarimeter::JediPolarimeter: "<<e.what()<<std::endl;
		throw e;
	}
	return;
}

void JediPolarimeter::DefineMaterials() {
	G4String el[]={"Lu","Y","Si","O","Ce"};
	std::vector<G4String> elements(el, el + sizeof(el) / sizeof(G4String) );
	G4double we[]={71.43*CLHEP::perCent,4.03*CLHEP::perCent,6.37*CLHEP::perCent,18.14*CLHEP::perCent,0.02*CLHEP::perCent};
	std::vector<G4double> weights(we, we + sizeof(we) / sizeof(G4double) );
	G4NistManager::Instance()->ConstructNewMaterial("LYSO",elements,weights,7.1*CLHEP::g/CLHEP::cm3);
	return;
}

void JediPolarimeter::ComputeParameters() {

	fDetectorZ = (fBeampipeRadius+fSafetyDistance) / tan( fThetaMin );

	fInnerDetectorRadius=fBeampipeRadius+fSafetyDistance;
	fOuterDetectorRadius=(fDetectorZ+fHCalSizeZ)*tan( fThetaMax );

	fMaxCrystal=ceil(fOuterDetectorRadius/fHCalSizeXY)+1;

	fTargetChamberZ1=fBeampipeRadius/ tan(fThetaMax)-1*CLHEP::cm;
	fTargetChamberZ2=fDetectorZ-fDeltaELength-1*CLHEP::cm;

	fDeltaEZ=fDetectorZ-5*CLHEP::cm;

	fWorldSizeXY=2*fOuterDetectorRadius+0.5*CLHEP::m;
	fWorldSizeZ=2*(fDetectorZ+fHCalSizeZ+1*CLHEP::cm);

	fBeampipeLength=fWorldSizeZ;

	fGeometryHasBeenChanged=false;
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

	fMessenger->DeclareMethod("update",&JediPolarimeter::GeometryHasChanged,"Update geometry");

	fMessenger->DeclareMethod("check",&JediPolarimeter::checkGeometry,"check geometry for overlaps");


	G4GenericMessenger::Command& matCmd
	= fMessenger->DeclareMethod("material",
				    &JediPolarimeter::setCaloMaterial,"scintillator material");

	fMessenger->DeclareMethodWithUnit("dEwidth","mm",
			&JediPolarimeter::setDeltaEwidth,
			"delta E width (mm)");

	fMessenger->DeclareMethodWithUnit("dElength","mm",
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

	fMessenger->DeclareProperty("worldmaterial",JediPolarimeter::fWorldMaterialName,"world material name");

	G4GenericMessenger::Command& dumpCmd
	= fMessenger->DeclareMethod("dump",&JediPolarimeter::WriteWorldToFile,"dump geometry to file");

	dumpCmd.SetParameterName("filename",true);

	return;

}

G4VPhysicalVolume* JediPolarimeter::Construct() {
	if(fGeometryHasBeenChanged)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	new InternalBeampipe(0,G4ThreeVector(0,0,fBeampipeLength/2),fLogicWorld,false,0,this);
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);

	/*
	auto carbon=G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
	G4Box* solidTarget=new G4Box("Target",fTargetWidth/2,fTargetWidth/2,fTargetThickness/2);
	G4LogicalVolume* logicTarget=new G4LogicalVolume(solidTarget,carbon,"CarbonTarget");
	//new G4PVPlacement(0,G4ThreeVector(0,0,targetThickness/2),logicTarget,"Target",logicWorld,0,false,0);
	 */
	return fPhysiWorld;
}

void JediPolarimeter::WriteWorldToFile(G4String filename) {

	if(filename.contains(".gdml")){
		G4Exception("JediPolarimeter::WriteWorldToFile","",JustWarning,"writing to gdml file is not implemented!");
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

    void JediPolarimeter::GeometryHasChanged(){
	this->ComputeParameters();
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
	G4RunManager::GetRunManager()->PhysicsHasBeenModified();
	G4RegionStore::GetInstance()->UpdateMaterialList(fPhysiWorld);
	G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void JediPolarimeter::ConstructSDandField() {
	if(JediConfigurationManager::Instance()->GetVerbose()>2)
		G4cout<<"JediPolarimeter::ConstructSDandField()"<<G4endl;

	for (const auto & iSD : fSensitiveDetectors.getMap()){
	  if (!fSD[iSD.first].Get()){
			fSD[iSD.first].Put(new JediSensitiveDetector(iSD.first,iSD.second.fType));
			G4SDManager::GetSDMpointer()->AddNewDetector(fSD[iSD.first].Get());
	  }
	  for(auto & iVol: iSD.second.fLogVol){
	    iVol->SetSensitiveDetector(fSD[iSD.first].Get());
	  }
	}
}

void JediPolarimeter::setCaloMaterial(G4String scintillatorMaterialName) {
	auto oldName=fHCalMaterial->GetName();
	auto newMat=G4NistManager::Instance()->FindOrBuildMaterial(scintillatorMaterialName);
	if(!newMat){
		G4Exception("JediPolarimeter::setScintillatorMaterialName","MatNotFound",G4ExceptionSeverity::JustWarning,"Material not found! Material not changed.");
		return;
	}
	fHCalMaterial=newMat;
	fScintillatorMaterialName=scintillatorMaterialName;
	G4cout<<"Changing Material from "<<oldName<<" to "<<fHCalMaterial->GetName()<<G4endl;
	return;
}
