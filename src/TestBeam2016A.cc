/*
 * TestBeamSetup.cc
 *
 *  Created on: 21.04.2016
 *      Author: pmaanen
 */

#include "TestBeam2016A.hh"
#include "Analysis.hh"
#include "Colors.hh"
#include <G4UserLimits.hh>
#include <JediSensitiveDetector.hh>
#include "ExternalBeampipe.hh"
TestBeam2016A::TestBeam2016A():SingleCrystal(),fLogicTrigger(0),fTriggerOffsetX(0),fTriggerOffsetY(0),fTriggerOffsetZ(0),fCalorimeterPosition(0,0,0),fDetectorName("default") {
	fCrystalLength=10*CLHEP::cm;
	fCrystalWidth=3*CLHEP::cm;
	fTriggerHeight=2.5*CLHEP::cm;
	fTriggerWidth=1.5*CLHEP::cm;
	fTriggerThickness=.2*CLHEP::cm;
	fTrigger=true;
	fScintillatorMaterialName="LYSO";
	fScintillatorMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);
	DefineCommands();
}

TestBeam2016A::~TestBeam2016A(){
}

G4LogicalVolume* TestBeam2016A::MakeDetector(G4String name, G4Material* mat,G4double width, G4double height, G4double thickness) {
	auto solidDetector= new G4Box(name,width/2,height/2,thickness/2);
	auto logicDetector = new G4LogicalVolume(solidDetector,mat,name);
	return logicDetector;
}

G4VPhysicalVolume* TestBeam2016A::Construct() {
	if(fChangedParameters)
		ComputeParameters();

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	fLogicWorld->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));

	MakeSetup();


	if(fDetectorName=="effective")
		MakeEffectiveDetector();
	else if(fDetectorName=="default")
		Make2016ADetector();
	else if(fDetectorName=="sandwich")
		MakeSandwichDetector();
	else
		Make2016ADetector();

	return fPhysiWorld;
}

void TestBeam2016A::MakeSetup() {


	if(fTriggerThickness>0 and fTriggerHeight>0 and fTriggerWidth>0){
		G4Box* solidTrigger=new G4Box("Trigger",fTriggerWidth/2,fTriggerHeight/2,fTriggerThickness/2);
		auto logicStart=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Start");
		fSensitiveDetectors.Update("Start",SDtype::kCalorimeter,logVolVector{logicStart});
		logicStart->SetVisAttributes(new G4VisAttributes(blue));
		new G4PVPlacement (0, G4ThreeVector(0,0,-8*CLHEP::cm), logicStart, "Start", fLogicWorld, false, 0, false);
		new G4PVPlacement (0, G4ThreeVector(0,0,-3*CLHEP::cm), logicStart, "Start", fLogicWorld, false, 1, false);

		auto logicStop=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Stop");
		fSensitiveDetectors.Update("Stop",SDtype::kCalorimeter,logVolVector{logicStop});
		new G4PVPlacement (0, G4ThreeVector(0,0,60*CLHEP::cm), logicStop, "Stop", fLogicWorld, false, 0, false);

		auto solidKapton=new G4Box("Kapton",fTriggerWidth/2,fTriggerHeight/2,0.160/2*CLHEP::mm);
		auto logicKapton=new G4LogicalVolume(solidKapton,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),"Kapton");
		new G4PVPlacement(0,G4ThreeVector(0,0,-8.1*CLHEP::cm-0.160/2*CLHEP::mm),logicKapton,"Kapton",fLogicWorld,false,0,false);

		auto solidMylar=new G4Box("Mylar",fTriggerWidth/2,fTriggerHeight/2,0.5/2.*CLHEP::mm);
		auto logicMylar=new G4LogicalVolume(solidMylar,G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR"),"Mylar");
		new G4PVPlacement(0,G4ThreeVector(0,0,-8.1*CLHEP::cm-0.160*CLHEP::mm-.5/2.*CLHEP::mm),logicMylar,"Mylar",fLogicWorld,false,0,false);
	}

	new ExternalBeampipe(0,G4ThreeVector(0,0,-20*CLHEP::cm),fLogicWorld,0,0,this);


}

void TestBeam2016A::MakeSandwichDetector() {

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto logicHodoscope=MakeDetector("Hodoscope",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fCrystalWidth,fCrystalWidth,1*CLHEP::cm);
	fSensitiveDetectors.Update("Hodoscope",SDtype::kCalorimeter,logVolVector{logicHodoscope});
	auto logicAbsorber=MakeDetector("Detector",G4NistManager::Instance()->FindOrBuildMaterial("G4_W"),fCrystalWidth,fCrystalWidth,fCrystalLength);
	fSensitiveDetectors.Update("Detector",SDtype::kTracker,logVolVector{logicAbsorber});
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(0,0,-.5*CLHEP::cm), logicHodoscope, "Hodoscope", fLogicWorld, false, 0, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(0,0,fCrystalLength/2), logicAbsorber, "Detector", fLogicWorld, false, 0, false);
	return;



}

void TestBeam2016A::DefineCommands() {

	SingleCrystal::DefineCommands();
	G4GenericMessenger::Command& triggerLengthCmd
	= fMessenger->DeclareMethodWithUnit("triggerheight","mm",
			&TestBeam2016A::setTriggerLength,
			"trigger height (mm)");


	G4GenericMessenger::Command& triggerThicknessCmd
	= fMessenger->DeclareMethodWithUnit("triggerthickness","mm",
			&TestBeam2016A::setTriggerThickness,
			"trigger thickness (mm)");

	G4GenericMessenger::Command& triggerWidthCmd
	= fMessenger->DeclareMethodWithUnit("triggerwidth","mm",
			&TestBeam2016A::setTriggerWidth,
			"trigger width (mm)");

	triggerWidthCmd.SetParameterName("width", true);
	triggerWidthCmd.SetRange("width>=0.");
	triggerWidthCmd.SetDefaultValue("30.");

	G4GenericMessenger::Command& trgOffsetXCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetX","mm",
			&TestBeam2016A::setTriggerOffsetX,
			"trigger offset in x dir. (mm)");
	G4GenericMessenger::Command& trgOffsetYCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetY","mm",
			&TestBeam2016A::setTriggerOffsetY,
			"trigger offset in y dir. (mm)");
	G4GenericMessenger::Command& trgOffsetZCmd
	= fMessenger->DeclareMethodWithUnit("trgOffsetZ","mm",
			&TestBeam2016A::setTriggerOffsetZ,
			"trigger offset in z dir. (mm)");

	fMessenger->DeclarePropertyWithUnit("position","mm",
			TestBeam2016A::fCalorimeterPosition,
			"calorimeter position");


	auto detCmd=fMessenger->DeclareProperty("detector",TestBeam2016A::fDetectorName,"detector type.");
	detCmd.SetCandidates("default sandwich effective");


}

void TestBeam2016A::Make2016ADetector() {

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto bigCrystal=MakeDetector("lyso",fScintillatorMaterial,fCrystalWidth,fCrystalWidth,fCrystalLength);
	auto smallCrystal=MakeDetector("lyso",fScintillatorMaterial,fCrystalWidth/2,fCrystalWidth,fCrystalLength);
	fSensitiveDetectors.Update("Calorimeter",SDtype::kCalorimeter,logVolVector{bigCrystal,smallCrystal});
	bigCrystal->SetVisAttributes(new G4VisAttributes(green));
	smallCrystal->SetVisAttributes(new G4VisAttributes(green));

	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(-fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2), bigCrystal, "Crystal1", fLogicWorld, false, 0, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(+fCrystalWidth/2,fCrystalWidth/2,fCrystalLength/2), bigCrystal, "Crystal2", fLogicWorld, false, 1, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(+fCrystalWidth/2-fCrystalWidth/4,-fCrystalWidth/2,fCrystalLength/2), smallCrystal, "Crystal3a", fLogicWorld, false, 2, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(+fCrystalWidth/2+fCrystalWidth/4,-fCrystalWidth/2,fCrystalLength/2), smallCrystal, "Crystal3b", fLogicWorld, false, 3, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(-fCrystalWidth/2,-fCrystalWidth/2,fCrystalLength/2), bigCrystal, "Crystal4", fLogicWorld, false, 4, false);


	auto logicPerfectDetector=MakeDetector("Observer",G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),2*fCrystalWidth,2*fCrystalWidth,1*CLHEP::mm);
	fSensitiveDetectors.Update("Observer",SDtype::kPerfect,logVolVector{logicPerfectDetector});
	new G4PVPlacement(rot,fCalorimeterPosition+G4ThreeVector(0,0,-.5*CLHEP::mm),logicPerfectDetector,"Observer",fLogicWorld,false,0,false);
	logicPerfectDetector->SetVisAttributes(G4VisAttributes::Invisible);

	auto logicVeto=MakeDetector("Veto",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),66*CLHEP::mm,.6*CLHEP::cm,fCrystalLength);
	logicVeto->SetVisAttributes(new G4VisAttributes(red));
	fSensitiveDetectors.Update("Veto",SDtype::kTracker,logVolVector{logicVeto});
	auto vetoPosition=G4ThreeVector(3*CLHEP::mm,fCrystalWidth+0.3*CLHEP::cm,fCrystalLength/2);
	for(int i=0;i<4;i++){
		auto vetoRot=new G4RotationMatrix(*rot);
		vetoRot->rotateZ(i*90*CLHEP::deg);
		auto iPos(vetoPosition);
		new G4PVPlacement(vetoRot,fCalorimeterPosition+iPos.rotateZ(i*90*CLHEP::deg),logicVeto,"Veto",fLogicWorld,false,i,false);
	}


	return;
}

void TestBeam2016A::MakeEffectiveDetector() {
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto logicHodoscope=MakeDetector("Hodoscope",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fCrystalWidth,fCrystalWidth,1*CLHEP::cm);
	fSensitiveDetectors.Update("Hodoscope",SDtype::kCalorimeter,logVolVector{logicHodoscope});
	auto logicObserver=MakeDetector("Detector",G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),fCrystalWidth,fCrystalWidth,1*CLHEP::mm);
	fSensitiveDetectors.Update("Detector",SDtype::kPerfect,logVolVector{logicObserver});
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(0,0,1*CLHEP::cm/2), logicHodoscope, "Hodoscope", fLogicWorld, false, 0, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(0,0,1*CLHEP::cm+.5*CLHEP::mm), logicObserver, "Detector", fLogicWorld, false, 0, false);
	return;



}


