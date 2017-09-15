/*
 * TestBeamSetup.cc
 *
 *  Created on: 21.04.2016
 *      Author: pmaanen
 */

#include "Analysis.hh"
#include "Colors.hh"
#include <G4UserLimits.hh>
#include <JediSensitiveDetector.hh>
#include <Testbeam2016a.hh>
#include "ExternalBeampipe.hh"
Testbeam2016a::Testbeam2016a():SingleCrystal(),fLogicTrigger(0),fTriggerOffsetX(0),fTriggerOffsetY(0),fTriggerOffsetZ(0),fCalorimeterPosition(0,0,0),fDetectorName("default") {
	fHCalSizeZ=8*CLHEP::cm;
	fHCalSizeXY=3*CLHEP::cm;
	fTriggerSizeY=2.5*CLHEP::cm;
	fTriggerSizeX=1.5*CLHEP::cm;
	fTriggerSizeZ=.2*CLHEP::cm;
	fTrigger=true;
	fScintillatorMaterialName="LYSO";
	fHCalMaterial=G4NistManager::Instance()->FindOrBuildMaterial(fScintillatorMaterialName);
	DefineCommands();
}

G4VPhysicalVolume* Testbeam2016a::Construct() {
	if(fGeometryHasBeenChanged)
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

void Testbeam2016a::MakeSetup() {


	if(fTriggerSizeY>0 and fTriggerSizeX>0 and fTriggerSizeZ>0){
		G4Box* solidTrigger=new G4Box("Trigger",fTriggerSizeX/2,fTriggerSizeY/2,fTriggerSizeZ/2);
		auto logicStart=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Start");
		fSensitiveDetectors.Update("Start",SDtype::kCalorimeter,logVolVector{logicStart});
		logicStart->SetVisAttributes(new G4VisAttributes(blue));
		new G4PVPlacement (0, G4ThreeVector(0,0,-8*CLHEP::cm), logicStart, "Start", fLogicWorld, false, 0, false);
		new G4PVPlacement (0, G4ThreeVector(0,0,-3*CLHEP::cm), logicStart, "Start", fLogicWorld, false, 1, false);

		auto logicStop=new G4LogicalVolume(solidTrigger,G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Stop");
		fSensitiveDetectors.Update("Stop",SDtype::kCalorimeter,logVolVector{logicStop});
		new G4PVPlacement (0, G4ThreeVector(0,0,60*CLHEP::cm), logicStop, "Stop", fLogicWorld, false, 0, false);

		auto solidKapton=new G4Box("Kapton",fTriggerSizeX/2,fTriggerSizeY/2,0.160/2*CLHEP::mm);
		auto logicKapton=new G4LogicalVolume(solidKapton,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),"Kapton");
		new G4PVPlacement(0,G4ThreeVector(0,0,-8.1*CLHEP::cm-0.160/2*CLHEP::mm),logicKapton,"Kapton",fLogicWorld,false,0,false);

		auto solidMylar=new G4Box("Mylar",fTriggerSizeX/2,fTriggerSizeY/2,0.5/2.*CLHEP::mm);
		auto logicMylar=new G4LogicalVolume(solidMylar,G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR"),"Mylar");
		new G4PVPlacement(0,G4ThreeVector(0,0,-8.1*CLHEP::cm-0.160*CLHEP::mm-.5/2.*CLHEP::mm),logicMylar,"Mylar",fLogicWorld,false,0,false);
	}

	//new ExternalBeampipe(0,G4ThreeVector(0,0,-fBeampipeLength/2),fLogicWorld,0,0,this);


	G4double fTargetSizeZ=10*CLHEP::mm;
	G4double fTargetSizeX=10*CLHEP::mm;
	auto rot=new G4RotationMatrix();
	auto logicTarget=BuildVolume<G4Tubs>("Target",G4NistManager::Instance()->FindOrBuildMaterial("G4_C"),0,fTargetSizeX/2,fTargetSizeZ/2,0,2*CLHEP::pi);
	auto pos=G4ThreeVector(0,0,fTargetSizeZ/2);
	new G4PVPlacement(rot,G4ThreeVector(0,0,fTargetSizeZ/2),logicTarget,"Target",fLogicWorld,false,0);

	fTargetTransform.SetNetTranslation( pos );
	fTargetTransform.SetNetRotation( *rot );
	fTarget=logicTarget;


}

void Testbeam2016a::DefineCommands() {

	SingleCrystal::DefineCommands();

	fMessenger->DeclareMethodWithUnit("triggersizeX","mm",
			&Testbeam2016a::setTriggerSizeX,
			"trigger size x (mm)");

	fMessenger->DeclareMethodWithUnit("triggersizeY","mm",
			&Testbeam2016a::setTriggerSizeY,
			"trigger size y (mm)");

	fMessenger->DeclareMethodWithUnit("triggersizeZ","mm",
			&Testbeam2016a::setTriggerSizeY,
			"trigger size z (mm)");

	fMessenger->DeclareMethodWithUnit("trgOffsetX","mm",
			&Testbeam2016a::setTriggerOffsetX,
			"trigger offset in x dir. (mm)");

	fMessenger->DeclareMethodWithUnit("trgOffsetY","mm",
			&Testbeam2016a::setTriggerOffsetY,
			"trigger offset in y dir. (mm)");

	fMessenger->DeclareMethodWithUnit("trgOffsetZ","mm",
			&Testbeam2016a::setTriggerOffsetZ,
			"trigger offset in z dir. (mm)");

	fMessenger->DeclarePropertyWithUnit("position","mm",
			Testbeam2016a::fCalorimeterPosition,
			"calorimeter position");


	auto detCmd=fMessenger->DeclareProperty("detector",Testbeam2016a::fDetectorName,"detector type.");
	detCmd.SetCandidates("default sandwich effective");


}

void Testbeam2016a::Make2016ADetector() {

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto bigCrystal=BuildVolume<G4Box>("lyso",fHCalMaterial,fHCalSizeXY/2,fHCalSizeXY/2,fHCalSizeZ/2);
	auto smallCrystal=BuildVolume<G4Box>("lyso",fHCalMaterial,fHCalSizeXY/4,fHCalSizeXY/2,fHCalSizeZ/2);
	fSensitiveDetectors.Update("Calorimeter",SDtype::kCalorimeter,logVolVector{bigCrystal,smallCrystal});
	bigCrystal->SetVisAttributes(new G4VisAttributes(green));
	smallCrystal->SetVisAttributes(new G4VisAttributes(green));
	fCalorimeterPosition=G4ThreeVector(0,0,40*CLHEP::mm);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(-fHCalSizeXY/2,fHCalSizeXY/2,fHCalSizeZ/2), bigCrystal, "Crystal1", fLogicWorld, false, 0, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(+fHCalSizeXY/2,fHCalSizeXY/2,fHCalSizeZ/2), bigCrystal, "Crystal2", fLogicWorld, false, 1, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(+fHCalSizeXY/2-fHCalSizeXY/4,-fHCalSizeXY/2,fHCalSizeZ/2), smallCrystal, "Crystal3a", fLogicWorld, false, 2, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(+fHCalSizeXY/2+fHCalSizeXY/4,-fHCalSizeXY/2,fHCalSizeZ/2), smallCrystal, "Crystal3b", fLogicWorld, false, 3, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(-fHCalSizeXY/2,-fHCalSizeXY/2,fHCalSizeZ/2), bigCrystal, "Crystal4", fLogicWorld, false, 4, false);


	auto logicPerfectDetector=	BuildVolume<G4Box>("Observer",G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),fHCalSizeXY,fHCalSizeXY,1*CLHEP::mm);
	fSensitiveDetectors.Update("Observer",SDtype::kPerfect,logVolVector{logicPerfectDetector});
	new G4PVPlacement(rot,fCalorimeterPosition+G4ThreeVector(0,0,20*CLHEP::cm-.5*CLHEP::mm),logicPerfectDetector,"Observer",fLogicWorld,false,0,false);
	logicPerfectDetector->SetVisAttributes(G4VisAttributes::Invisible);

	auto logicVeto=BuildVolume<G4Box>("Veto",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),66*CLHEP::mm,fHCalSizeXY,.6*CLHEP::cm);
	logicVeto->SetVisAttributes(new G4VisAttributes(red));
	fSensitiveDetectors.Update("Veto",SDtype::kTracker,logVolVector{logicVeto});
	auto vetoPosition=G4ThreeVector(3*CLHEP::mm,fHCalSizeZ+0.3*CLHEP::cm,fHCalSizeXY/2);
	for(int i=0;i<4;i++){
		auto vetoRot=new G4RotationMatrix(*rot);
		vetoRot->rotateZ(i*90*CLHEP::deg);
		auto iPos(vetoPosition);
		//new G4PVPlacement(vetoRot,fCalorimeterPosition+iPos.rotateZ(i*90*CLHEP::deg),logicVeto,"Veto",fLogicWorld,false,i,false);
	}


	return;
}

void Testbeam2016a::MakeEffectiveDetector() {
	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto logicHodoscope=MakeDetector("Hodoscope",G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),fHCalSizeZ,fHCalSizeZ,1*CLHEP::cm);
	fSensitiveDetectors.Update("Hodoscope",SDtype::kCalorimeter,logVolVector{logicHodoscope});
	auto logicObserver=MakeDetector("Detector",G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),fHCalSizeZ,fHCalSizeZ,1*CLHEP::mm);
	fSensitiveDetectors.Update("Detector",SDtype::kPerfect,logVolVector{logicObserver});
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(0,0,1*CLHEP::cm/2), logicHodoscope, "Hodoscope", fLogicWorld, false, 0, false);
	new G4PVPlacement (rot, fCalorimeterPosition+G4ThreeVector(0,0,1*CLHEP::cm+.5*CLHEP::mm), logicObserver, "Detector", fLogicWorld, false, 0, false);
	return;



}

void Testbeam2016a::MakeSandwichDetector() {
	G4RotationMatrix* rot = new G4RotationMatrix();
	rot->set(fPhi, fTheta, fPsi);
	auto logicHodoscope = MakeDetector("Hodoscope",
			G4NistManager::Instance()->FindOrBuildMaterial(
					"G4_PLASTIC_SC_VINYLTOLUENE"), fHCalSizeZ, fHCalSizeZ,
					1 * CLHEP::cm);
	fSensitiveDetectors.Update("Hodoscope", SDtype::kCalorimeter,
			logVolVector { logicHodoscope });
	auto logicAbsorber = MakeDetector("Detector",
			G4NistManager::Instance()->FindOrBuildMaterial("G4_W"),
			fHCalSizeZ, fHCalSizeZ, fHCalSizeXY);
	fSensitiveDetectors.Update("Detector", SDtype::kTracker, logVolVector {
		logicAbsorber });
	new G4PVPlacement(rot,
			fCalorimeterPosition + G4ThreeVector(0, 0, -.5 * CLHEP::cm),
			logicHodoscope, "Trigger", fLogicWorld, false, 0, false);
	new G4PVPlacement(rot,
			fCalorimeterPosition + G4ThreeVector(0, 0, fHCalSizeXY / 2),
			logicAbsorber, "Detector", fLogicWorld, false, 0, false);
	return;
}
