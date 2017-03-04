/*

	index=iCrystalX+fMaxCrystal+(iCrystalY+fMaxCrystal)*1000; * Testbeam2016B.cc
 *
 *  Created on: 18.07.2016
 *      Author: pmaanen
 */



#include "Colors.hh"

#include <G4UImanager.hh>
#include <G4UserLimits.hh>
#include <Testbeam2016b.hh>
#include "ExternalBeampipe.hh"
#include "G4SubtractionSolid.hh"


#include <math.h>
static auto man=G4NistManager::Instance();
static auto al=man->FindOrBuildMaterial("G4_Al");
static auto plastic=man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
static auto air=man->FindOrBuildMaterial("G4_Air");
Testbeam2016b::Testbeam2016b():Testbeam2016a() {
	Reset();
	//Tedlar= Brand name for DuPont® polyvinyl fluoride
	auto tedlarDens=1.76*CLHEP::g/CLHEP::cm3;
	/*
	fTedlar=new G4Material("Tedlar",tedlarDens,3);
	fTedlar->AddElement(man->FindOrBuildElement("H"),int(3));
	fTedlar->AddElement(man->FindOrBuildElement("C"),int(2));
	fTedlar->AddElement(man->FindOrBuildElement("F"),int(1));
	 */
	man->ConstructNewMaterial("Tedlar",std::vector<G4String>{"H","C","F"},std::vector<G4int>{3,2,1},tedlarDens);
	DefineCommands();
	ComputeParameters();
}

void Testbeam2016b::ConstructSDandField() {

	if(gVerbose>2)
		G4cout<<"Testbeam2016b::ConstructSDandField()"<<G4endl;

	for (const auto & iSD : fSensitiveDetectors.getMap()){
		if (!fSD[iSD.first].Get()){
			if(iSD.second.fType==SDtype::kCalorimeter){
				fSD[iSD.first].Put(new JediSensitiveDetector(iSD.first));
				fSD[iSD.first].Get()->AddSD(new CaloSensitiveDetector(iSD.first,3));
				for(auto & iVol: iSD.second.fLogVol){
					SetSensitiveDetector(iVol,fSD[iSD.first].Get());
				}
			}
		}
	}
}

void Testbeam2016b::Reset() {
	fDistance=1*CLHEP::m;
	fArmWidth=10*CLHEP::cm;

	fAngle=10*CLHEP::deg;
	fDetectorHeight=0;
	fMinDistance=25*CLHEP::cm;
	fBuildSupport=false;

	fRightDetector=true;
	fLeftDetector=true;
	fBuildTarget=true;
	fMonitor=false;
	fBeampipe=true;
	fTrigger=false;
	fVeto=false;
	fStart=false;

	fWorldSizeXY=2*CLHEP::m;
	fWorldSizeZ=5*CLHEP::m;

	fVetoSizeXY=5*CLHEP::cm;
	fVetoSizeZ=5*CLHEP::mm;
	fHoleSizeXY=2*CLHEP::cm;

	fTargetMaterialName="G4_GRAPHITE";
	fNx=6;
	fNy=2;
	fAngleLeft=fAngle;
	fAngleRight=fAngle;

	fApertureSize=40*CLHEP::mm;
	fCollimatorPosition=G4ThreeVector(0,0,-25*CLHEP::mm);
	fTargetDistance=610*CLHEP::mm;
	fTargetSizeX=fTargetSizeY=50*CLHEP::mm;
	fTargetSizeZ=1*CLHEP::cm;
	fDetectorName="default";
	fBeampipeLength=50*CLHEP::cm;
	fTargetPos=G4ThreeVector(0,0,0);

	fTriggerSizeLeftZ=10*CLHEP::mm;
	fTriggerSizeRightZ=5*CLHEP::mm;
}



G4VPhysicalVolume* Testbeam2016b::Construct() {
	if(fGeometryHasBeenChanged)
		ComputeParameters();
	G4Box* solidWorld=new G4Box("World",fWorldSizeXY/2,fWorldSizeXY/2,fWorldSizeZ/2);
	fLogicWorld = new G4LogicalVolume(solidWorld,G4NistManager::Instance()->FindOrBuildMaterial(fWorldMaterialName),"World");
	//auto worldVisAttr=new G4VisAttributes();
	fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
	if(fDetectorName=="calibration")
		fSensitiveDetectors.Update("Air",SDtype::kCalorimeter,logVolVector{fLogicWorld});
	fPhysiWorld=new G4PVPlacement(0,G4ThreeVector(0,0,0),fLogicWorld,"World",0,0,0,0);

	if(fDetectorName=="effective")
		BuildEffectiveDetector();
	else if(fDetectorName=="default")
		Build2016BDetector();
	else if(fDetectorName=="sandwich")
		BuildSandwichDetector();
	else if(fDetectorName=="calibration")
		BuildCalibrationSetup();
	else if(fDetectorName=="isolation")
		BuildIsolationSetup();
	else{
		G4ExceptionDescription ed;
		ed<<"Scenario with name "<<fDetectorName<<" is now known!";
		G4Exception("Testbeam2016b::Construct","",FatalException,ed);
	}
	return fPhysiWorld;
}

void Testbeam2016b::BuildSetup() {

	if(fBuildTarget)
		BuildTarget();

	if(fBuildSupport)
		BuildSupportElements();

	if(fMonitor){
		auto logicMonitor=MakeDetector("Monitor",man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),4*CLHEP::cm,4*CLHEP::cm,5*CLHEP::mm);
		new G4PVPlacement(0,G4ThreeVector(0,fArmWidth-fMinDistance+fDetectorHeight-fNy*fHCalSizeZ/2+2*CLHEP::cm,fDistance/2),logicMonitor,"Monitor",fLogicWorld,0,0,0);
	}
	if(fBeampipe){
		auto bp=new ExternalBeampipe(0,G4ThreeVector(0,0,-fBeampipeLength/2-fTargetDistance),fLogicWorld,0,0,this);
		if(fDetectorName=="calibration")
			fSensitiveDetectors.Update("ExitWindow",SDtype::kCalorimeter,logVolVector{bp->getExitWindow()});
	}
	if(fCollimator){
		auto logicColl=BuildCollimator();
		logicColl->SetVisAttributes(new G4VisAttributes(gray));
		auto rotLeft=new G4RotationMatrix();
		rotLeft->rotateY(180*CLHEP::deg);
		auto rotRight=new G4RotationMatrix();

		auto posX=fApertureSize/2+27.5*CLHEP::mm;
		G4double posZl,posZr;
		for(int i=0;i<4;i++){
			posZl=2*i*26*CLHEP::mm+12.5*CLHEP::mm-207*CLHEP::mm;
			posZr=(2*i+1)*26*CLHEP::mm+12.5*CLHEP::mm-207*CLHEP::mm;
			new G4PVPlacement(rotLeft,G4ThreeVector(-posX,0,posZl)+fCollimatorPosition,logicColl,"CollimatorLeft",fLogicWorld,0,2*i,0);
			new G4PVPlacement(rotRight,G4ThreeVector(posX,0,posZr)+fCollimatorPosition,logicColl,"CollimatorRight",fLogicWorld,0,2*i+1,0);
		}

		if(fVeto){
			auto solidVetoBox=new G4Box("VetoBox",fVetoSizeXY/2,fVetoSizeXY/2,fVetoSizeZ/2);
			auto solidVetoHole=new G4Tubs("VetoHole",0,fHoleSizeXY,fVetoSizeZ,0,360*CLHEP::deg);
			auto solidVeto=new G4SubtractionSolid("Veto",solidVetoBox,solidVetoHole);
			auto logicVeto=new G4LogicalVolume(solidVeto,plastic,"Veto");
			logicVeto->SetVisAttributes(new G4VisAttributes(cyan));
			fSensitiveDetectors.Update("Veto",SDtype::kCalorimeter,logVolVector{logicVeto});
			new G4PVPlacement(0,G4ThreeVector(0,0,-fVetoSizeZ),logicVeto,"Veto",fLogicWorld,1,0,false);
		}

		if(fStart){
			auto solidStart=new G4Box("VetoBox",fVetoSizeXY/2,fVetoSizeXY/2,fVetoSizeZ/2);
			auto logicStart=new G4LogicalVolume(solidStart,plastic,"Start");
			logicStart->SetVisAttributes(new G4VisAttributes(cyan));
			fSensitiveDetectors.Update("Start",SDtype::kCalorimeter,logVolVector{logicStart});
			new G4PVPlacement(0,G4ThreeVector(0,0,-fVetoSizeZ/2),logicStart,"Veto",fLogicWorld,1,0,false);
		}
	}

}

G4LogicalVolume* Testbeam2016b::BuildScintillatorMatrix(G4String name) {

	G4RotationMatrix* rot=new G4RotationMatrix();
	rot->set(fPhi,fTheta,fPsi);

	auto logicScintillator=BuildCaloCrystal(name);
	auto solidCrystal=dynamic_cast<G4Box*>(logicScintillator->GetSolid());

	if(!solidCrystal)
		G4Exception("Testbeam2016b::MakeScintillatorMatrix","",FatalException,"solidCrystal==nullptr. Dynamic cast failed!");
	auto motherSizeX=2*fNx*solidCrystal->GetXHalfLength();
	auto motherSizeY=2*fNy*solidCrystal->GetYHalfLength();
	auto motherSizeZ=2*solidCrystal->GetZHalfLength();

	auto logicMother=BuildVolume<G4Box>("Detector",man->FindOrBuildMaterial("G4_Galactic"),motherSizeX/2,motherSizeY/2,motherSizeZ/2);


	G4int index=0;
	for(G4int iX=0;iX<fNx;iX++){
		for(G4int iY=0;iY<fNy;iY++){
			index=iX+10*iY;
			new G4PVPlacement (rot,
					G4ThreeVector(-motherSizeX/2+(iX+0.5)*2*solidCrystal->GetXHalfLength(),
							-motherSizeY/2+(iY+0.5)*2*solidCrystal->GetYHalfLength(),0),
							logicScintillator, "hcalelement", logicMother, false,index , false);
		}
	}

	logicScintillator->SetVisAttributes(new G4VisAttributes(green));
	logicMother->SetVisAttributes(G4VisAttributes::Invisible);
	return logicMother;
}

void Testbeam2016b::BuildEffectiveDetector() {

	G4double startSizeX=15*CLHEP::mm;
	G4double startSizeY=20*CLHEP::mm;
	G4double startSizeZ=2*CLHEP::mm;
	auto logicStart=BuildVolume<G4Box>("Start",plastic,startSizeX/2,startSizeY/2,startSizeZ);
	fSensitiveDetectors.Update("Start",SDtype::kCalorimeter,{logicStart});
	logicStart->SetVisAttributes(new G4VisAttributes(blue));
	auto startRot=new G4RotationMatrix();
	startRot->rotateZ(45*CLHEP::deg);

	auto startPos=G4ThreeVector(0,0,-37.5*CLHEP::mm);

	auto rotRight=new G4RotationMatrix;
	auto rotLeft=new G4RotationMatrix;

	startPos.rotateZ(45*CLHEP::deg);

	new G4PVPlacement(startRot,startPos,logicStart,"Start",fLogicWorld,0,0,0);

	rotRight->rotateY(fAngleRight);
	rotLeft->rotateY(-fAngleLeft);

	auto logicRight=BuildScintillatorMatrix("Right");
	auto logicLeft=BuildScintillatorMatrix("Left");
	auto trigVisAttr=new G4VisAttributes(cyan);

	if(fRightDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerR",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerR",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance-fTriggerSizeLeftZ/2).rotateY(-fAngleRight),logicTrigger,"TriggerRight",fLogicWorld,0,0,false);
		}
		auto logicObserver=BuildVolume<G4Box>("ObserverRight",air,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
		fSensitiveDetectors.Update("ObserverR",SDtype::kPerfect,logVolVector{logicObserver});
		new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance-3*fTriggerSizeLeftZ/2).rotateY(-fAngleRight),logicObserver,"ObserverRight",fLogicWorld,0,0,false);
		new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(-fAngleRight),logicRight,"Right",fLogicWorld,0,0,false);
	}
	if(fLeftDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerL",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeRightZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerL",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance-fTriggerSizeRightZ/2).rotateY(fAngleLeft),logicTrigger,"TriggerLeft",fLogicWorld,0,0,false);

		}
		auto logicObserver=BuildVolume<G4Box>("ObserverLight",air,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
		fSensitiveDetectors.Update("ObserverL",SDtype::kPerfect,logVolVector{logicObserver});
		new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance-3*fTriggerSizeRightZ/2).rotateY(fAngleLeft),logicObserver,"ObserverRight",fLogicWorld,0,0,false);
		new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(fAngleLeft),logicLeft,"Left",fLogicWorld,0,0,false);
	}

}

void Testbeam2016b::BuildSandwichDetector() {
	G4Exception(" Testbeam2016b::MakeSandwichDetector","",FatalException," Testbeam2016b::MakeSandwichDetector is not yet implemented!");
}

void Testbeam2016b::BuildTarget() {

	auto rot=new G4RotationMatrix();
	auto targetMat=G4NistManager::Instance()->FindOrBuildMaterial(fTargetMaterialName);
	if(!targetMat){
		std::stringstream message;
		message<<"Could not find target material with name "<<fTargetMaterialName;
		G4Exception("Testbeam2016b::MakeTarget()","",FatalException,message.str().c_str());
	}
	auto logicTarget=BuildVolume<G4Tubs>("Target",targetMat,0,fTargetSizeX/2,fTargetSizeZ/2,0,2*CLHEP::pi);

	logicTarget->SetVisAttributes(new G4VisAttributes(G4Color::Brown()));
	if(fDetectorName=="calibration")
		fSensitiveDetectors.Update("Target",SDtype::kPerfect,logVolVector{logicTarget});
	auto pos=G4ThreeVector(0,0,0);
	if(fTargetPos==G4ThreeVector(0,0,0))
		pos=G4ThreeVector(0,0,fTargetSizeZ/2);
	else
		pos=fTargetPos;
	new G4PVPlacement(rot,pos,logicTarget,"Target",fLogicWorld,false,0);


	if(false){
		auto logicObs=BuildVolume<G4Tubs>("Observer",G4NistManager::Instance()->FindOrBuildMaterial(fWorldMaterialName),0,2*fTargetSizeX/2,1*CLHEP::mm,0,2*CLHEP::pi);
		auto visAttr=new G4VisAttributes(cyan);
		logicObs->SetVisAttributes(visAttr);
		auto posObs=G4ThreeVector(0,0,fTargetSizeZ/2+10*CLHEP::cm);
		fSensitiveDetectors.Update("Observer",SDtype::kPerfect,logVolVector{logicObs});
		new G4PVPlacement(rot,posObs,logicObs,"Target",fLogicWorld,false,0);
	}

	fTargetTransform.SetNetTranslation( pos );
	fTargetTransform.SetNetRotation( *rot );
	fTarget=logicTarget;
}

void Testbeam2016b::Build2016BDetector() {

	BuildSetup();

	auto rotRight=new G4RotationMatrix;
	auto rotLeft=new G4RotationMatrix;

	rotRight->rotateY(fAngleRight);
	rotLeft->rotateY(-fAngleLeft);

	auto logicRight=BuildScintillatorMatrix("Right");
	auto logicLeft=BuildScintillatorMatrix("Left");
	auto trigVisAttr=new G4VisAttributes(cyan);

	if(fRightDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerR",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerR",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance-fTriggerSizeZ/2).rotateY(-fAngleRight),logicTrigger,"TriggerRight",fLogicWorld,0,0,false);
		}
		new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(-fAngleRight),logicRight,"Right",fLogicWorld,0,0,false);
	}
	if(fLeftDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerL",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerL",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance-fTriggerSizeZ/2).rotateY(fAngleLeft),logicTrigger,"TriggerLeft",fLogicWorld,0,0,false);

		}
		new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(fAngleLeft),logicLeft,"Left",fLogicWorld,0,0,false);
	}

}

void Testbeam2016b::DefineCommands() {

	fMessenger->DeclareMethod("reset",&Testbeam2016b::Reset,"");

	fMessenger->DeclareProperty("scenario",Testbeam2016b::fDetectorName,"");

	fMessenger->DeclarePropertyWithUnit("armlength","mm",Testbeam2016b::fDistance,"");

	fMessenger->DeclarePropertyWithUnit("height","mm",Testbeam2016b::fDetectorHeight,"");

	fMessenger->DeclarePropertyWithUnit("armwidth","mm",Testbeam2016b::fArmWidth,"");

	fMessenger->DeclareProperty("target_position",Testbeam2016b::fTargetPos,"");

	fMessenger->DeclarePropertyWithUnit("angle","rad",Testbeam2016b::fAngle,"");

	fMessenger->DeclarePropertyWithUnit("angle_left","rad",Testbeam2016b::fAngleLeft,"");

	fMessenger->DeclarePropertyWithUnit("angle_right","rad",Testbeam2016b::fAngleRight,"");

	fMessenger->DeclarePropertyWithUnit("trigger_thick_left","mm",Testbeam2016b::fTriggerSizeLeftZ,"");

	fMessenger->DeclarePropertyWithUnit("trigger_thick_right","mm",Testbeam2016b::fTriggerSizeRightZ,"");

	fMessenger->DeclarePropertyWithUnit("vetoSizeXY","mm",Testbeam2016b::fVetoSizeXY,"");

	fMessenger->DeclarePropertyWithUnit("vetoSizeZ","mm",Testbeam2016b::fVetoSizeZ,"");

	fMessenger->DeclarePropertyWithUnit("holeSizeXY","mm",Testbeam2016b::fHoleSizeXY,"");

	fMessenger->DeclarePropertyWithUnit("aperture","mm",Testbeam2016b::fApertureSize,"");

	fMessenger->DeclareProperty("support",Testbeam2016b::fBuildSupport,"support beam on/off");

	fMessenger->DeclareProperty("nx",Testbeam2016b::fNx,"number of detectors in x");

	fMessenger->DeclareProperty("ny",Testbeam2016b::fNy,"number of detectors in y");

	fMessenger->DeclareProperty("left",Testbeam2016b::fLeftDetector,"left detector on/off");

	fMessenger->DeclareProperty("right",Testbeam2016b::fRightDetector,"right detector on/off");

	fMessenger->DeclareProperty("target",Testbeam2016b::fBuildTarget,"target on/off");

	fMessenger->DeclareProperty("start",Testbeam2016b::fStart,"start on/off");

	fMessenger->DeclarePropertyWithUnit("targetSizeZ","mm",Testbeam2016b::fTargetSizeZ,"size of target in z.");

	fMessenger->DeclarePropertyWithUnit("targetSizeXY","mm",Testbeam2016b::fTargetSizeX,"diameter of target.");

	fMessenger->DeclareProperty("veto",Testbeam2016b::fVeto,"veto on/off");

	fMessenger->DeclareProperty("trigger",Testbeam2016b::fTrigger,"trigger on/off");

	fMessenger->DeclareProperty("collimator",Testbeam2016b::fCollimator,"collimator on/off");

	fMessenger->DeclareProperty("target_material",Testbeam2016b::fTargetMaterialName,"target material");
}

G4LogicalVolume* Testbeam2016b::BuildCaloCrystal(G4String detName) {
	G4double teflonThick=150*CLHEP::um;
	G4double tedlarThick=teflonThick;
	G4double kaptonThick=50*CLHEP::um;

	auto motherSizeZ=2*teflonThick+2*tedlarThick+2*kaptonThick+fHCalSizeZ;
	auto motherSizeXY=2*teflonThick+2*tedlarThick+2*kaptonThick+fHCalSizeXY;

	auto teflon=man->FindOrBuildMaterial("G4_TEFLON");
	auto kapton=man->FindOrBuildMaterial("G4_KAPTON");
	auto tedlar=man->FindOrBuildMaterial("Tedlar");
	auto logicKapton=BuildVolume<G4Box>(detName,kapton,motherSizeXY/2,motherSizeXY/2,motherSizeZ/2);

	auto logicTedlar=BuildVolume<G4Box>("Wrap1",tedlar,motherSizeXY/2-kaptonThick,
			motherSizeXY/2-kaptonThick,
			motherSizeZ/2-kaptonThick);
	auto logicTeflon=BuildVolume<G4Box>("Wrap2",teflon,motherSizeXY/2-kaptonThick-tedlarThick,
			motherSizeXY/2-kaptonThick-tedlarThick,
			motherSizeZ/2-kaptonThick-tedlarThick);
	auto logicDetector=BuildVolume<G4Box>(detName+"_SD",fHCalMaterial,motherSizeXY/2-kaptonThick-tedlarThick-teflonThick,
			motherSizeXY/2-kaptonThick-tedlarThick-teflonThick,
			motherSizeZ/2-kaptonThick-tedlarThick-teflonThick);

	auto pos=G4ThreeVector(0,0,0);
	new G4PVPlacement(0,pos,logicTedlar,"Wrap1",logicKapton,0,0,0);
	new G4PVPlacement(0,pos,logicTeflon,"Wrap2",logicTedlar,0,0,0);
	new G4PVPlacement(0,pos,logicDetector,"Crystal",logicTeflon,0,0,0);
	if(fDetectorName=="calibration"){
		fSensitiveDetectors.Update("Kapton",SDtype::kCalorimeter,logVolVector{logicKapton});
		fSensitiveDetectors.Update("Teflon",SDtype::kCalorimeter,logVolVector{logicTeflon});
		fSensitiveDetectors.Update("Tedlar",SDtype::kCalorimeter,logVolVector{logicTedlar});
	}
	fSensitiveDetectors.Update(detName,SDtype::kCalorimeter,logVolVector{logicDetector});

	logicTedlar->SetVisAttributes(G4VisAttributes::Invisible);
	logicTeflon->SetVisAttributes(G4VisAttributes::Invisible);
	logicDetector->SetVisAttributes(G4VisAttributes::Invisible);
	return logicKapton;
}

G4LogicalVolume* Testbeam2016b::BuildCollimator(){


	auto coll_mat=G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");

	auto collSizeX=135*CLHEP::mm;
	auto collSizeY=154*CLHEP::mm;
	auto collSizeZ=25*CLHEP::mm;
	auto box1=new G4Box("box1",collSizeX/2,collSizeY/2,collSizeZ/2);


	auto rot=new G4RotationMatrix();
	rot->rotateZ(45*CLHEP::deg);


	G4double holeSize=sqrt(2)*40*CLHEP::mm;
	auto box2=new G4Box("box2",holeSize/2,holeSize/2,57/2*CLHEP::mm);

	G4double holeSize1=sqrt(2)*60*CLHEP::mm;
	auto box3=new G4Box("box2",holeSize1/2,holeSize1/2,30/2*CLHEP::mm);

	auto solidColl1=new G4SubtractionSolid("collimator",box1,box2,rot,G4ThreeVector(-collSizeX/2,0,0));
	auto solidColl2=new G4SubtractionSolid("collimator",solidColl1,box3,rot,G4ThreeVector(collSizeX/2,-collSizeY/2,0));

	auto logicColl=new G4LogicalVolume(solidColl2,coll_mat,"collimator");
	return logicColl;
}

void Testbeam2016b::ComputeParameters() {

	JediPolarimeter::ComputeParameters();
	fBeampipeLength=25*CLHEP::cm;
	G4double Zmax=fDistance+fTriggerSizeZ+fHCalSizeZ+10*CLHEP::cm;
	G4double Zmin=fTargetDistance+fBeampipeLength+10*CLHEP::cm;
	fWorldSizeZ=2*std::max(Zmin,Zmax);
	fWorldSizeXY=fDistance+fTriggerSizeZ+fHCalSizeZ+50*CLHEP::cm;
}

void Testbeam2016b::BuildSupportElements() {
	auto solidSupport=new G4SubtractionSolid("Support",
			new G4Box("Support1",fArmWidth/2,fArmWidth/2.,fDistance/2+fHCalSizeZ/2),
			new G4Box("Support2",fArmWidth/4,fArmWidth/4.,fDistance)
	);

	auto logicHorizontalSupport=new G4LogicalVolume(solidSupport,al,"Support");


	auto rotRight=new G4RotationMatrix;
	auto rotLeft=new G4RotationMatrix;

	rotRight->rotateY(fAngleRight);
	rotLeft->rotateY(-fAngleLeft);

	if(fRightDetector)
		new G4PVPlacement(rotRight,
				G4ThreeVector(0,-1*CLHEP::m,fDistance/2+fHCalSizeZ/2).rotateY(-fAngleRight),
				logicHorizontalSupport,"Support",fLogicWorld,0,0,false);
	if(fLeftDetector)
		new G4PVPlacement(rotLeft,
				G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight-fNy*fHCalSizeZ/2,fDistance/2).rotateY(fAngleLeft),
				logicHorizontalSupport,"Support",fLogicWorld,1,0,false);

	auto solidShortSupport=new G4SubtractionSolid("Support",
			new G4Box("",fArmWidth/2,fArmWidth/2.,fDistance/4),
			new G4Box("",fArmWidth/4,fArmWidth/4.,fDistance)
	);

	auto logicShortSupport=new G4LogicalVolume(solidShortSupport,man->FindOrBuildMaterial("G4_Al"),"Support");


	new G4PVPlacement(0,G4ThreeVector(0,fArmWidth/2-fMinDistance+fDetectorHeight-fNy*fHCalSizeZ/2,fDistance/4),
			logicShortSupport,"Support",fLogicWorld,1,0,false);


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
		new G4PVPlacement(rotRight,
				G4ThreeVector(0,-3./2.*fArmWidth-fMinDistance+fDetectorHeight+fArmWidth/2+(fArmWidth+fMinDistance)/2-fNy*fHCalSizeZ/2,
						fDistance-fArmWidth/2).rotateY(-fAngleRight),logicLongVerticalSupport,"Support",fLogicWorld,0,0,false);
	if(fLeftDetector)
		new G4PVPlacement(rotLeft,
				G4ThreeVector(0,-fArmWidth/2.-fMinDistance+fDetectorHeight+fArmWidth/2+fMinDistance/2-fNy*fHCalSizeZ/2,
						fDistance-fArmWidth/2).rotateY(fAngleLeft),logicShortVerticalSupport,"Support",fLogicWorld,0,0,false);
}

void Testbeam2016b::BuildCalibrationSetup() {

	fRightDetector=false;
	fCollimator=false;
	fNx=1;
	fNy=1;

	BuildSetup();

	auto rot1=new G4RotationMatrix();
	rot1->rotateY(fAngle);

	auto logicDet=BuildScintillatorMatrix("Detector");
	auto trigVisAttr=new G4VisAttributes(cyan);

	if(fTrigger){
		auto logicTrigger=MakeDetector("Trigger",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
		logicTrigger->SetVisAttributes(trigVisAttr);
		fSensitiveDetectors.Update("TriggerR",SDtype::kCalorimeter,logVolVector{logicTrigger});
		new G4PVPlacement(rot1,G4ThreeVector(0,0,fDistance-fTriggerSizeZ/2).rotateY(-fAngle),
				logicTrigger,"Trigger",fLogicWorld,0,0,false);
	}
	new G4PVPlacement(rot1,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(-fAngle),
			logicDet,"Detector",fLogicWorld,0,0,false);
}

void Testbeam2016b::BuildIsolationSetup(){


	fStart=false;
	BuildSetup();

	G4double startSizeX=15*CLHEP::mm;
	G4double startSizeY=20*CLHEP::mm;
	G4double startSizeZ=2*CLHEP::mm;
	auto logicStart=BuildVolume<G4Box>("Start",plastic,startSizeX/2,startSizeY/2,startSizeZ);
	fSensitiveDetectors.Update("Start",SDtype::kCalorimeter,{logicStart});
	logicStart->SetVisAttributes(new G4VisAttributes(blue));
	auto startRot=new G4RotationMatrix();
	startRot->rotateZ(45*CLHEP::deg);

	auto startPos=G4ThreeVector(0,0,-37.5*CLHEP::mm);

	auto rotRight=new G4RotationMatrix;
	auto rotLeft=new G4RotationMatrix;

	startPos.rotateZ(45*CLHEP::deg);

	new G4PVPlacement(startRot,startPos,logicStart,"Start",fLogicWorld,0,0,0);

	rotRight->rotateY(fAngleRight);
	rotLeft->rotateY(-fAngleLeft);

	auto logicRight=BuildScintillatorMatrix("Right");
	auto logicLeft=BuildScintillatorMatrix("Left");
	auto trigVisAttr=new G4VisAttributes(cyan);

	if(fRightDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerR",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerR",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance-fTriggerSizeLeftZ/2).rotateY(-fAngleRight),logicTrigger,"TriggerRight",fLogicWorld,0,0,false);
		}
		new G4PVPlacement(rotRight,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(-fAngleRight),logicRight,"Right",fLogicWorld,0,0,false);
	}
	if(fLeftDetector){
		if(fTrigger){
			auto logicTrigger=MakeDetector("TriggerL",plastic,fNx*fHCalSizeXY/2,fNy*fHCalSizeXY/2,fTriggerSizeRightZ/2);
			logicTrigger->SetVisAttributes(trigVisAttr);
			fSensitiveDetectors.Update("TriggerL",SDtype::kCalorimeter,logVolVector{logicTrigger});
			new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance-fTriggerSizeRightZ/2).rotateY(fAngleLeft),logicTrigger,"TriggerLeft",fLogicWorld,0,0,false);

		}
		new G4PVPlacement(rotLeft,G4ThreeVector(0,0,fDistance+fHCalSizeZ/2).rotateY(fAngleLeft),logicLeft,"Left",fLogicWorld,0,0,false);
	}


}
