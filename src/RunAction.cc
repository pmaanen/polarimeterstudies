#include "RunAction.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "Analysis.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include <ctime>
#include "PrimaryGeneratorAction.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::RunAction()
{
	Analysis::Instance();



	seed = -1;      // RANLUX seed
	luxury = 3;     // RANLUX luxury level (3 is default)
	saveRndm = 1;
	fNEvents=0;

	Analysis* analysisManager = Analysis::Instance();
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetFirstHistoId(1);

	// Creating histograms
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{	delete Analysis::Instance();}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//*********************************************************************************
//******************  Begin Of Run Action                   ***********************
//*********************************************************************************

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
	fNEvents=aRun->GetNumberOfEventToBeProcessed();
	Analysis::Instance()->PrepareNewRun(aRun);
	Analysis::Instance()->OpenFile(Analysis::Instance()->getFilename());
	Analysis::Instance()->CreateH1("dummy","dummy",1,0,1);
	if (!IsMaster()) //it is a slave, do nothing else
	{
		//Analysis::Instance()->PrepareNewRun(aRun);

		G4cout << "ooo Run " << aRun->GetRunID() << " starts on slave." << G4endl;
	}

	//Master or sequential
	G4cout << "ooo Run " << aRun->GetRunID() << " starts (global)." << G4endl;
	if (seed<0) //not initialized by anybody else
	{

		seed=time(0);
		G4Random::setTheSeed(seed,luxury);
	}

	// save Rndm status
	if (saveRndm > 0)
		G4Random::saveEngineStatus("beginOfRun.rndm");

	return;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//*********************************************************************************
//******************  End Of Run Action                   ***********************
//*********************************************************************************
void RunAction::EndOfRunAction(const G4Run* aRun)
{
	G4cout <<"Run Number:" <<aRun->GetRunID()<<" ended\n";

	Analysis::Instance()->Write();
	Analysis::Instance()->CloseFile();
	if (!IsMaster())
	{
		G4cout << "### Run " << aRun->GetRunID() << " (slave) ended." << G4endl;
		return;
	}
	// Complete clean-up
	G4cout << "### Run " << aRun->GetRunID() << " (global) ended." << G4endl;
	// save Rndm status
	if (saveRndm == 1)
		G4Random::saveEngineStatus("endOfRun.rndm");
	return;

}
