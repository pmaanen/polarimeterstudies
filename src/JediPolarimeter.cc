/*
 * JediPolarimeter.cc
 *
 *  Created on: 20.01.2015
 *      Author: pmaanen
 */

#include <JediPolarimeter.hh>
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
JediPolarimeter::JediPolarimeter() {


/*
	G4Material *Scint_mat = new G4Material("Scint", density=7.4*CLHEP::g/CLHEP::cm3, 4);
	Scint_mat->AddElement(Lu, 71*perCent);
	Scint_mat->AddElement(Si, 7*perCent);
	Scint_mat->AddElement(O, 18*perCent);
	Scint_mat->AddElement(Y, 4*perCent);
*/
	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;

	beampipeRadius=10*CLHEP::cm;
	beampipeThickness=2*CLHEP::mm;

	crystalLength=10*CLHEP::cm;
	crystalWidth=8.5*CLHEP::cm;

	changedParameters=true;
	DefineCommands();
	ComputeParameters();

}

JediPolarimeter::~JediPolarimeter() {
	delete fMessenger;
}

void JediPolarimeter::ComputeParameters() {
	DetectorZ = (beampipeRadius+5*CLHEP::mm) / tan( thetaMin );

	innerDetectorRadius=DetectorZ*tan( thetaMin );
	outerDetectorRadius=DetectorZ*tan( thetaMax );

	MaxCrystal=ceil(outerDetectorRadius/crystalWidth);
	MinCrystal=ceil(innerDetectorRadius/crystalWidth);

	G4cout<<"----------------"<<G4endl;
	G4cout<<"detector z: "<<G4BestUnit(DetectorZ, "Length")<<"/inner radius: "
			<<G4BestUnit(innerDetectorRadius, "Length")<<"/outer radius: "
			<<G4BestUnit(outerDetectorRadius, "Length")<<G4endl;
	G4cout<<"----------------"<<G4endl;
	changedParameters=false;
}

G4LogicalVolume* JediPolarimeter::MakeBeampipe() {
	G4Tubs* solidBeampipe=new G4Tubs("Beampipe",beampipeRadius-beampipeThickness,beampipeRadius,3*CLHEP::m-1*CLHEP::cm,0*CLHEP::deg,360*CLHEP::deg);
	G4LogicalVolume* logicBeampipe = new G4LogicalVolume(solidBeampipe,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),"Beampipe");
	G4VisAttributes* beampipeVisAttr = new G4VisAttributes(gray);
	logicBeampipe->SetVisAttributes(beampipeVisAttr);
	return logicBeampipe;
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
	= fMessenger->DeclareMethodWithUnit("length","mm",
			&JediPolarimeter::setCrystalLength,
			"crystal length (mm)");
	crystalLengthCmd.SetParameterName("length", true);
	crystalLengthCmd.SetRange("length>=0.");
	crystalLengthCmd.SetDefaultValue("100.");



	G4GenericMessenger::Command& crystalWidthCmd
	= fMessenger->DeclareMethodWithUnit("width","mm",
			&JediPolarimeter::setCrystalWidth,
			"crystal width (mm)");

	crystalWidthCmd.SetParameterName("width", true);
	crystalWidthCmd.SetRange("width>=0.");
	crystalWidthCmd.SetDefaultValue("30.");

}
