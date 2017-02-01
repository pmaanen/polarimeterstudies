/*

	index=iCrystalX+fMaxCrystal+(iCrystalY+fMaxCrystal)*1000; * Testbeam2016B.cc
 *
 *  Created on: 18.07.2016
 *      Author: pmaanen
 */



#include <E22.hh>
#include "Colors.hh"

#include <G4UImanager.hh>
#include <G4UserLimits.hh>
#include "ExternalBeampipe.hh"
#include "E22Target.hh"
#include "G4SubtractionSolid.hh"
static auto man=G4NistManager::Instance();
static auto al=man->FindOrBuildMaterial("G4_Al");
static auto vacuum=man->FindOrBuildMaterial("G4_Galactic");
static auto plastic=man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

E22::E22():E21(),fDistance(1*CLHEP::m),fArmWidth(10*CLHEP::cm),fAngle(10*CLHEP::deg),fDetectorHeight(0),fMinDistance(25*CLHEP::cm),fSupport(false),fRightDetector(true),fLeftDetector(true),fTarget(true),fMonitor(false),fBeampipe(true),fTrigger(false), fVeto(false) {
	fWorldSizeXY=2*CLHEP::m;
	fWorldSizeZ=3*CLHEP::m;

	fVetoSizeXY=5*CLHEP::cm;
	fVetoSizeZ=5*CLHEP::mm;
	fHoleSizeXY=2*CLHEP::cm;

	fTargetMaterialName="G4_C";
	fNx=6;
	fNy=2;
	fTargetSizeX=fTargetSizeY=fTargetSizeZ=1*CLHEP::cm;
	fDetectorName="default";
	DefineCommands();
}

G4VPhysicalVolume* E22::Construct() {
	if(fGeometryHasBeenChanged)
		ComputeParameters();

	fWorldSizeZ=fWorldSizeXY=fDistance+fTriggerSizeZ+fHCalSizeZ+3*CLHEP::m;

	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial(fWorldMaterialName),"World");
	auto worldVisAttr=new G4VisAttributes();
	worldVisAttr->SetForceWireframe(true);
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);
	//fLogicWorld->SetUserLimits(new G4UserLimits(100.0 * CLHEP::um,1000*CLHEP::mm,100*CLHEP::ns,0,0));
	MakeSetup();

	if(fVeto){
		auto solidVetoBox=new G4Box("VetoBox",fVetoSizeXY/2,fVetoSizeXY/2,fVetoSizeZ/2);
		auto solidVetoHole=new G4Tubs("VetoHole",0,fHoleSizeXY,fVetoSizeZ,0,360*CLHEP::deg);
		auto solidVeto=new G4SubtractionSolid("Veto",solidVetoBox,solidVetoHole);
		auto logicVeto=new G4LogicalVolume(solidVeto,plastic,"Veto");
		logicVeto->SetVisAttributes(new G4VisAttributes(cyan));
		fSensitiveDetectors.Update("Veto",SDtype::kCalorimeter,logVolVector{logicVeto});
		new G4PVPlacement(0,G4ThreeVector(0,0,-fVetoSizeZ),logicVeto,"Veto",fLogicWorld,1,0,false);
	}
	if(fTarget)
		MakeTarget();
	if(fDetectorName=="effective")
		MakeEffectiveDetector();
	if(fDetectorName=="default")
		Make2016BDetector();
	if(fDetectorName=="sandwich")
		MakeSandwichDetector();
	return fPhysiWorld;
}

void E22::MakeSetup() {

	if(fSupport){
		auto solidSupport=new G4SubtractionSolid("Support",
				new G4Box("",fArmWidth/2,fArmWidth/2.,fDistance/2+fHCalSizeZ/2),
				new G4Box("",fArmWidth/4,fArmWidth/4.,fDistance)
		);

		auto logicHorizontalSupport=new G4LogicalVolume(solidSupport,al,"Support");


		auto rot1=new G4RotationMatrix;
		auto rot2=new G4RotationMatrix;

		rot1->rotateY(fAngle);
		rot2->rotateY(-fAngle);

		if(fRightDetector)
			new G4PVPlacement(rot1,G4ThreeVector(0,-1*CLHEP::m,fDistance/2+fHCalSizeZ/2).rotateY(-fAngle),logicHorizontalSupport,"Support",fLogicWorld,0,0,false);
		if(fLeftDetector)
			new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight-fNy*fHCalSizeZ/2,fDistance/2).rotateY(fAngle),logicHorizontalSupport,"Support",fLogicWorld,1,0,false);

		auto solidShortSupport=new G4SubtractionSolid("Support",
				new G4Box("",fArmWidth/2,fArmWidth/2.,fDistance/4),
				new G4Box("",fArmWidth/4,fArmWidth/4.,fDistance)
		);

		auto logicShortSupport=new G4LogicalVolume(solidShortSupport,man->FindOrBuildMaterial("G4_Al"),"Support");


		new G4PVPlacement(0,G4ThreeVector(0,fArmWidth/2-fMinDistance+fDetectorHeight-fNy*fHCalSizeZ/2,fDistance/4),logicShortSupport,"Support",fLogicWorld,1,0,false);


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

		if(fRightDetector)
			new G4PVPlacement(rot1,G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight+fArmWidth/2+(fArmWidth+fMinDistance)/2-fNy*fHCalSizeZ/2,fDistance-fArmWidth/2).rotateY(-fAngle),logicLongVerticalSupport,"Support",fLogicWorld,0,0,false);
		if(fLeftDetector)
			new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight+fArmWidth/2+fMinDistance/2-fNy*fHCalSizeZ/2,fDistance-fArmWidth/2).rotateY(fAngle),logicShortVerticalSupport,"Support",fLogicWorld,0,0,false);
	}
	if(fMonitor){
		auto logicMonitor=MakeDetector("Monitor",man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),4*CLHEP::cm,4*CLHEP::cm,5*CLHEP::mm);
		new G4PVPlacement(0,G4ThreeVector(0,fArmWidth-fMinDistance+fDetectorHeight-fNy*fHCalSizeZ/2+2*CLHEP::cm,fDistance/2),logicMonitor,"Monitor",fLogicWorld,0,0,0);
	}
	if(fBeampipe)
		new ExternalBeampipe(0,G4ThreeVector(0,0,-20*CLHEP::cm),fLogicWorld,0,0,this);
}

G4LogicalVolume* E22::MakeScintillatorMatrix(G4String name) {

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto motherSizeX=fNx*fHCalSizeXY;
	auto motherSizeY=fNy*fHCalSizeXY;
	auto motherSizeZ=fHCalSizeZ;

	auto solidMother=new G4Box("Detector",motherSizeX/2,motherSizeY/2,motherSizeZ/2);
	auto logicMother=new G4LogicalVolume(solidMother,man->FindOrBuildMaterial("G4_Galactic"),"Detector");
	auto logicCrystal=MakeDetector("Crystal",fHCalMaterial,fHCalSizeXY/2,fHCalSizeXY/2,fHCalSizeZ/2);
	fSensitiveDetectors.Update(name,SDtype::kCalorimeter,logVolVector{logicCrystal});
	logicCrystal->SetVisAttributes(new G4VisAttributes(green));

	G4int index=0;
	for(G4int iX=0;iX<fNx;iX++){
		for(G4int iY=0;iY<fNy;iY++){
			index=iX+1000*iY;
			new G4PVPlacement (rot, G4ThreeVector(-motherSizeX/2+(iX+0.5)*fHCalSizeXY,-motherSizeY/2+(iY+0.5)*fHCalSizeXY,0), logicCrystal, "", logicMother, false,index , false);
		}
	}

	logicMother->SetVisAttributes(G4VisAttributes::Invisible);
	return logicMother;


}

void E22::MakeEffectiveDetector() {

	auto rot1=new G4RotationMatrix;
	auto rot2=new G4RotationMatrix;

	rot1->rotateY(fAngle);
	rot2->rotateY(-fAngle);



	auto logicRight=MakeDetector("Right",vacuum,fNx*fHCalSizeZ,fNy*fHCalSizeZ,1*CLHEP::mm);
	fSensitiveDetectors.Update("Right",SDtype::kPerfect,logVolVector{logicRight});

	auto logicLeft=MakeDetector("Left",vacuum,fNx*fHCalSizeZ,fNy*fHCalSizeZ,1*CLHEP::mm);
	fSensitiveDetectors.Update("Left",SDtype::kPerfect,logVolVector{logicLeft});
	if(fRightDetector)
		new G4PVPlacement(rot1,G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight+fArmWidth/2+(fArmWidth+fMinDistance),fDistance-fArmWidth/2-fHCalSizeXY/2+.5*CLHEP::mm).rotateY(-fAngle),logicRight,"Right",fLogicWorld,0,0,false);
	if(fLeftDetector)
		new G4PVPlacement(rot2,G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight+fArmWidth/2+fMinDistance,fDistance-fArmWidth/2-fHCalSizeXY/2+.5*CLHEP::mm).rotateY(fAngle),logicLeft,"Left",fLogicWorld,0,1,false);

	//G4UImanager::GetUIpointer()->ApplyCommand("/PolarimeterStudies/Left/SetType perfect");
	//G4UImanager::GetUIpointer()->ApplyCommand("/PolarimeterStudies/Right/SetType perfect");
}

void E22::MakeSandwichDetector() {
}

void E22::MakeTarget() {

	new E22Target(0,G4ThreeVector(0,0,fTargetSizeZ/2),fLogicWorld,false,0,this);
}

void E22::Make2016BDetector() {

	auto rot1=new G4RotationMatrix;
	auto rot2=new G4RotationMatrix;

	rot1->rotateY(fAngle);
	rot2->rotateY(-fAngle);

	auto logicRight=MakeScintillatorMatrix("Right");
	auto logicLeft=MakeScintillatorMatrix("Left");
	auto trigVisAttr=new G4VisAttributes(cyan);

	if(fRightDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerR",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerR",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rot1,G4ThreeVector(0,0,fDistance-fTriggerSizeZ/2).rotateY(-fAngle),logicTrigger,"TriggerRight",fLogicWorld,0,0,false);
		}
		new G4PVPlacement(rot1,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(-fAngle),logicRight,"Right",fLogicWorld,0,0,false);
	}
	if(fLeftDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerL",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerL",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rot2,G4ThreeVector(0,0,fDistance-fTriggerSizeZ/2).rotateY(fAngle),logicTrigger,"TriggerLeft",fLogicWorld,0,0,false);

		}
		new G4PVPlacement(rot2,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(fAngle),logicLeft,"Left",fLogicWorld,0,0,false);
	}

}

void E22::DefineCommands() {


	fTargetMessenger =std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,
			"/PolarimeterStudies/target/",
			"target control"));

	fTargetMessenger->DeclareProperty("material",E22::fTargetMaterialName,"");

	fTargetMessenger->DeclarePropertyWithUnit("x","mm",E22::fTargetSizeX,"");
	fTargetMessenger->DeclarePropertyWithUnit("y","mm",E22::fTargetSizeY,"");
	fTargetMessenger->DeclarePropertyWithUnit("z","mm",E22::fTargetSizeZ,"");

	fMessenger->DeclarePropertyWithUnit("armlength","mm",E22::fDistance,"");

	fMessenger->DeclarePropertyWithUnit("height","mm",E22::fDetectorHeight,"");

	fMessenger->DeclarePropertyWithUnit("armwidth","mm",E22::fArmWidth,"");

	fMessenger->DeclarePropertyWithUnit("armangle","rad",E22::fAngle,"");

	fMessenger->DeclarePropertyWithUnit("vetoSizeXY","mm",E22::fVetoSizeXY,"");

	fMessenger->DeclarePropertyWithUnit("vetoSizeZ","mm",E22::fVetoSizeZ,"");

	fMessenger->DeclarePropertyWithUnit("holeSizeXY","mm",E22::fHoleSizeXY,"");

	fMessenger->DeclareProperty("support",E22::fSupport,"support beam on/off");

	fMessenger->DeclareProperty("nx",E22::fNx,"number of detectors in x");

	fMessenger->DeclareProperty("ny",E22::fNy,"number of detectors in y");

	fMessenger->DeclareProperty("left",E22::fLeftDetector,"left detector on/off");

	fMessenger->DeclareProperty("right",E22::fRightDetector,"right detector on/off");

	fMessenger->DeclareProperty("target",E22::fTarget,"target on/off");

	fMessenger->DeclareProperty("veto",E22::fVeto,"veto on/off");

	fMessenger->DeclareProperty("trigger",E22::fTrigger,"trigger on/off");
}
