#include <JediCommon.hh>
#include "RunAction.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "Analysis.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include <TROOT.h>
#else
#include "G4RunManager.hh"
#endif
#include "Randomize.hh"
#include <ctime>
#include "PrimaryGeneratorAction.hh"
#include "JediRun.hh"
#include "JediPolarimeter.hh"
#include "JediPhysicsManager.hh"
#include "G4AutoLock.hh"
#include <G4EmProcessOptions.hh>
#include <DetectorConstructionFactory.hh>
#include "SingleCrystal.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::RunAction(JediPhysicsManager* physicsManager):fPhysicsManager(physicsManager)
{
	Analysis::Instance();
	ROOT::EnableThreadSafety();

	// RANLUX seed
	if(gConfig.count("random.seed"))
		fSeed=gConfig["random.seed"].as<int>();
	else
		fSeed = -1;

	fLuxury = 3;     // RANLUX luxury level (3 is default)
	fSaveRndm = gConfig["random.save_random"].as<bool>();
	fNEvents=0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4Run* RunAction::GenerateRun()
{ return new JediRun; }
//*********************************************************************************
//******************  Begin Of Run Action                   ***********************
//*********************************************************************************

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
	ROOT::EnableThreadSafety();
	fNEvents=aRun->GetNumberOfEventToBeProcessed();
	Analysis::Instance()->BeginOfRun();


	G4RunManager *      runManager( G4RunManager::GetRunManager() );
	const JediPolarimeter *  setup( static_cast< const JediPolarimeter * >(
			runManager->GetUserDetectorConstruction() ) );
	if(setup)
		fPhysicsManager->GeometryHasChanged(setup);
	else
		G4Exception("RunAction::BeginOfRunAction","",FatalException,"Detector Construction not found.");

	if (!IsMaster()) //it is a slave, do nothing else
	{
		if(gVerbose>2)
			G4cout << "ooo Run " << aRun->GetRunID() << " starts on slave." << G4endl;
		return;
	}

	//Master or sequential
	if(gVerbose>2)
		G4cout << "ooo Run " << aRun->GetRunID() << " starts (global)." << G4endl;
	if (fSeed<0) //not initialized by anybody else
	{

		fSeed=time(0);
		G4Random::setTheSeed(fSeed,fLuxury);
	}

	// save Rndm status
	if (fSaveRndm)
		G4Random::saveEngineStatus("beginOfRun.rndm");

	return;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//*********************************************************************************
//******************  End Of Run Action                   ***********************
//*********************************************************************************
void RunAction::EndOfRunAction(const G4Run* aRun)
{
	if (!IsMaster())
	{
		if(gVerbose>2)
			G4cout << "### Run " << aRun->GetRunID() << " (slave) ended." << G4endl;
		return;
	}
	auto an=Analysis::Instance();
	if (an->isEnabled()){
		an->EndOfRun(aRun);
	}
	// Complete clean-up
	if(gVerbose>2)
		G4cout << "### Run " << aRun->GetRunID() << " (global) ended." << G4endl;
	return;

}
