/*

 * Testbeam2016B.cc
 *
 *  Created on: 18.07.2016
 *      Author: pmaanen
 */



#include "Colors.hh"

#include <G4UImanager.hh>
#include <G4UserLimits.hh>
#include <TestBeam2016B.hh>

static auto man=G4NistManager::Instance();
static auto al=man->FindOrBuildMaterial("G4_Al");
static auto vacuum=man->FindOrBuildMaterial("G4_Galactic");
TestBeam2016B::TestBeam2016B():fArmLength(1*CLHEP::m),fArmWidth(10*CLHEP::cm),fArmAngle(10*CLHEP::deg),fDetectorHeight(0),kSupport(true),fMinDistance(25*CLHEP::cm),TestBeam2016A() {
	fWorldSizeXY=3*CLHEP::m;
	fWorldSizeZ=5*CLHEP::m;
	fNx=5;
	fNy=2;
	fDetectorName="default";
	DefineCommands();
}

TestBeam2016B::~TestBeam2016B() {
	// TODO Auto-generated destructor stub
}

G4VPhysicalVolume* TestBeam2016B::Construct() {
	if(fChangedParameters)
		ComputeParameters();

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),"World");
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	fLogicWorld->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	MakeSetup();
	MakeTarget();

	if(fDetectorName=="effective")
		MakeEffectiveDetector();
	if(fDetectorName=="default")
		Make2016BDetector();
	if(fDetectorName=="sandwich")
		MakeSandwichDetector();
	return fPhysiWorld;
}

void TestBeam2016B::MakeSetup() {
	/*
	if(fTriggerThickness>0 and fTriggerHeight>0 and fTriggerWidth>0){
		G4Box* solidTrigger=new G4Box("Trigger",fTriggerWidth/2,fTriggerHeight/2,fTriggerThickness/2);
		auto logicStart=new G4LogicalVolume(solidTrigger,man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Start");
		fSensitiveDetectors.Update("Start",SDtype::kCalorimeter,logVolVector{logicStart});
		logicStart->SetVisAttributes(new G4VisAttributes(blue));
		new G4PVPlacement (0, G4ThreeVector(0,0,-8*CLHEP::cm), logicStart, "Start", fLogicWorld, false, 0, false);
		new G4PVPlacement (0, G4ThreeVector(0,0,-3*CLHEP::cm), logicStart, "Start", fLogicWorld, false, 1, false);

		auto logicStop=new G4LogicalVolume(solidTrigger,man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),"Stop");
		fSensitiveDetectors.Update("Stop",SDtype::kCalorimeter,logVolVector{logicStop});
		new G4PVPlacement (0, G4ThreeVector(0,0,60*CLHEP::cm), logicStop, "Stop", fLogicWorld, false, 0, false);

		auto solidKapton=new G4Box("Kapton",fTriggerWidth/2,fTriggerHeight/2,0.160/2*CLHEP::mm);
		auto logicKapton=new G4LogicalVolume(solidKapton,man->FindOrBuildMaterial("G4_KAPTON"),"Kapton");
		new G4PVPlacement(0,G4ThreeVector(0,0,-8.1*CLHEP::cm-0.160/2*CLHEP::mm),logicKapton,"Kapton",fLogicWorld,false,0,false);

		auto solidMylar=new G4Box("Mylar",fTriggerWidth/2,fTriggerHeight/2,0.5/2.*CLHEP::mm);
		auto logicMylar=new G4LogicalVolume(solidMylar,man->FindOrBuildMaterial("G4_MYLAR"),"Mylar");
		new G4PVPlacement(0,G4ThreeVector(0,0,-8.1*CLHEP::cm-0.160*CLHEP::mm-.5/2.*CLHEP::mm),logicMylar,"Mylar",fLogicWorld,false,0,false);
	}

	 */
	G4Material* supportMaterial=nullptr;
	if(kSupport){


		auto solidSupport=new G4SubtractionSolid("Support",
				new G4Box("",fArmWidth/2,fArmWidth/2.,fArmLength/2),
				new G4Box("",fArmWidth/4,fArmWidth/4.,fArmLength)
		);

		auto logicHorizontalSupport=new G4LogicalVolume(solidSupport,al,"Support");


		auto rot1=new G4RotationMatrix;
		auto rot2=new G4RotationMatrix;

		rot1->rotateY(fArmAngle);
		rot2->rotateY(-fArmAngle);


		new G4PVPlacement(rot1,G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight-fNy*fCrystalWidth/2,fArmLength/2).rotateY(-fArmAngle),logicHorizontalSupport,"Support",fLogicWorld,0,0,false);
		new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight-fNy*fCrystalWidth/2,fArmLength/2).rotateY(fArmAngle),logicHorizontalSupport,"Support",fLogicWorld,1,0,false);

		auto solidShortSupport=new G4SubtractionSolid("Support",
				new G4Box("",fArmWidth/2,fArmWidth/2.,fArmLength/4),
				new G4Box("",fArmWidth/4,fArmWidth/4.,fArmLength)
		);

		auto logicShortSupport=new G4LogicalVolume(solidShortSupport,man->FindOrBuildMaterial("G4_Al"),"Support");


		new G4PVPlacement(0,G4ThreeVector(0,fArmWidth/2-fMinDistance+fDetectorHeight-fNy*fCrystalWidth/2,fArmLength/4),logicShortSupport,"Support",fLogicWorld,1,0,false);


		auto solidLongVerticalSupport=new G4SubtractionSolid("VerticalSupport",
				new G4Box("",fArmWidth/2,(fArmWidth+fMinDistance)/2,fArmWidth/2),
				new G4Box("",fArmWidth/4,(fArmWidth+fMinDistance),fArmWidth/4)
		);

		auto solidShortVerticalSupport=new G4SubtractionSolid("VerticalSupport",
				new G4Box("",fArmWidth/2,fMinDistance/2,fArmWidth/2),
				new G4Box("",fArmWidth/4,fMinDistance,fArmWidth/4)
		);

		auto logicLongVerticalSupport=new G4LogicalVolume(solidLongVerticalSupport,man->FindOrBuildMaterial("G4_Al"),"VerticalSupport");
		auto logicShortVerticalSupport=new G4LogicalVolume(solidShortVerticalSupport,man->FindOrBuildMaterial("G4_Al"),"VerticalSupport");

		new G4PVPlacement(rot1,G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight+fArmWidth/2+(fArmWidth+fMinDistance)/2-fNy*fCrystalWidth/2,fArmLength-fArmWidth/2).rotateY(-fArmAngle),logicLongVerticalSupport,"Support",fLogicWorld,0,0,false);
		new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight+fArmWidth/2+fMinDistance/2-fNy*fCrystalWidth/2,fArmLength-fArmWidth/2).rotateY(fArmAngle),logicShortVerticalSupport,"Support",fLogicWorld,0,0,false);
	}
	auto logicMonitor=MakeDetector("Monitor",man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),4*CLHEP::cm,4*CLHEP::cm,5*CLHEP::mm);
	new G4PVPlacement(0,G4ThreeVector(0,fArmWidth-fMinDistance+fDetectorHeight-fNy*fCrystalWidth/2+2*CLHEP::cm,fArmLength/2),logicMonitor,"Monitor",fLogicWorld,0,0,0);

	auto solidVacuum=new G4Tubs("Vacuum",0,5*CLHEP::cm,10*CLHEP::cm,0,2*CLHEP::pi*CLHEP::rad);
	auto logicVacuum=new G4LogicalVolume(solidVacuum,man->FindOrBuildMaterial("G4_Galactic"),"Vacuum");
	logicVacuum->SetVisAttributes(new G4VisAttributes(tyellow));
	new G4PVPlacement(0,G4ThreeVector(0,0,-20*CLHEP::cm-100*CLHEP::um),logicVacuum,"Vacuum",fLogicWorld,false,0,false);

	auto solidExitWindow=new G4Tubs("exitWindow",0,5*CLHEP::cm,100*CLHEP::um,0,2*CLHEP::pi*CLHEP::rad);
	auto logicExitWindow=new G4LogicalVolume(solidExitWindow,man->FindOrBuildMaterial("G4_Fe"),"exitWindow");
	logicExitWindow->SetVisAttributes(new G4VisAttributes(gray));
	new G4PVPlacement(0,G4ThreeVector(0,0,10*CLHEP::cm-100*CLHEP::um),logicExitWindow,"exitWindow",logicVacuum,false,0,false);
}

G4LogicalVolume* TestBeam2016B::MakeScintillatorMatrix(G4String name) {

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto motherWidth=fNx*fCrystalWidth;
	auto motherHeight=fNy*fCrystalWidth;
	auto motherLength=fCrystalLength;

	auto solidMother=new G4Box("Detector",motherWidth/2,motherHeight/2,motherLength/2);
	auto logicMother=new G4LogicalVolume(solidMother,man->FindOrBuildMaterial("G4_Galactic"),"Detector");
	auto logicCrystal=MakeDetector("Crystal",fScintillatorMaterial,fCrystalWidth,fCrystalWidth,fCrystalLength);
	fSensitiveDetectors.Update(name,SDtype::kCalorimeter,logVolVector{logicCrystal});
	logicCrystal->SetVisAttributes(new G4VisAttributes(green));


	for(G4int iX=0;iX<fNx;iX++){
		for(G4int iY=0;iY<fNy;iY++){
			new G4PVPlacement (rot, G4ThreeVector(-motherWidth/2+(iX+0.5)*fCrystalWidth,-motherHeight/2+(iY+0.5)*fCrystalWidth,0), logicCrystal, "", logicMother, false,iY*fNx+iX , false);
		}
	}

	/*
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

	 */
	logicMother->SetVisAttributes(G4VisAttributes::Invisible);
	return logicMother;


}

void TestBeam2016B::MakeEffectiveDetector() {

	auto rot1=new G4RotationMatrix;
	auto rot2=new G4RotationMatrix;

	rot1->rotateY(fArmAngle);
	rot2->rotateY(-fArmAngle);



	auto logicRight=MakeDetector("Right",vacuum,fNx*fCrystalWidth,fNy*fCrystalWidth,1*CLHEP::mm);
	fSensitiveDetectors.Update("Right",SDtype::kPerfect,logVolVector{logicRight});

	auto logicLeft=MakeDetector("Left",vacuum,fNx*fCrystalWidth,fNy*fCrystalWidth,1*CLHEP::mm);
	fSensitiveDetectors.Update("Left",SDtype::kPerfect,logVolVector{logicLeft});

	new G4PVPlacement(rot1,G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight+fArmWidth/2+(fArmWidth+fMinDistance),fArmLength-fArmWidth/2-fCrystalLength/2+.5*CLHEP::mm).rotateY(-fArmAngle),logicRight,"Left",fLogicWorld,0,0,false);
	new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight+fArmWidth/2+fMinDistance,fArmLength-fArmWidth/2-fCrystalLength/2+.5*CLHEP::mm).rotateY(fArmAngle),logicLeft,"Right",fLogicWorld,0,1,false);

	G4UImanager::GetUIpointer()->ApplyCommand("/PolarimeterStudies/Left/SetType perfect");
	G4UImanager::GetUIpointer()->ApplyCommand("/PolarimeterStudies/Right/SetType perfect");
}

void TestBeam2016B::MakeSandwichDetector() {
}

void TestBeam2016B::MakeTarget() {

	G4Box* solidTarget=new G4Box("Target",fTargetWidth/2,fTargetWidth/2,fTargetThickness/2);
	G4LogicalVolume* logicTarget=new G4LogicalVolume(solidTarget,man->FindOrBuildMaterial("G4_C"),"CarbonTarget");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),logicTarget,"Target",fLogicWorld,0,false,0);



}

void TestBeam2016B::Make2016BDetector() {

	auto rot1=new G4RotationMatrix;
	auto rot2=new G4RotationMatrix;

	rot1->rotateY(fArmAngle);
	rot2->rotateY(-fArmAngle);

	auto logicRight=MakeScintillatorMatrix("Right");
	auto logicLeft=MakeScintillatorMatrix("Left");

	new G4PVPlacement(rot1,G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight+fArmWidth/2+(fArmWidth+fMinDistance),fArmLength-fArmWidth/2).rotateY(-fArmAngle),logicRight,"Left",fLogicWorld,0,0,false);
	new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight+fArmWidth/2+fMinDistance,fArmLength-fArmWidth/2).rotateY(fArmAngle),logicLeft,"Right",fLogicWorld,0,1,false);

}

void TestBeam2016B::DefineCommands() {

	fMessenger->DeclarePropertyWithUnit("armlength","mm",TestBeam2016B::fArmLength,"");

	fMessenger->DeclarePropertyWithUnit("height","mm",TestBeam2016B::fDetectorHeight,"");

	fMessenger->DeclarePropertyWithUnit("armwidth","mm",TestBeam2016B::fArmWidth,"");

	fMessenger->DeclarePropertyWithUnit("armangle","rad",TestBeam2016B::fArmAngle,"");

	fMessenger->DeclareProperty("support",TestBeam2016B::kSupport,"support beam on/off");

	fMessenger->DeclareProperty("nx",TestBeam2016B::fNx,"number of detectors in x");

	fMessenger->DeclareProperty("ny",TestBeam2016B::fNy,"number of detectors in y");

	fMessenger->DeclareProperty("ny",TestBeam2016B::fDetectorName,"detector");
}
