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

#include "G4AutoLock.hh"

namespace { G4Mutex RunActionMutex = G4MUTEX_INITIALIZER; }

std::vector<G4String>* RunAction::filenames=0;
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
{
	G4AutoLock lock(&RunActionMutex);
	if(filenames){
		delete filenames;
		filenames=0;
	}
	delete Analysis::Instance();}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//*********************************************************************************
//******************  Begin Of Run Action                   ***********************
//*********************************************************************************

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
	fNEvents=aRun->GetNumberOfEventToBeProcessed();
	//Analysis::Instance()->PrepareNewRun(aRun);
	Analysis::Instance()->OpenFile();
	PushBackFileName(Analysis::Instance()->GetFileName());
	if (!IsMaster()) //it is a slave, do nothing else
	{
		G4cout << "ooo Run " << aRun->GetRunID() << " starts on slave." << G4endl;
	}
	else{
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
	if(IsMaster()){
			std::ostringstream hadd;
			std::ostringstream rm;
			std::ostringstream mv;
			G4String extension;
			auto name=Analysis::Instance()->GetFileName();
			if ( name.last(".") != std::string::npos ) {
				extension = name.substr(name.last("."));
				name = name.substr(0, name.last("."));
			}
			else {
				extension = ".";
				extension.append(Analysis::Instance()->GetFileType());
			}
			rm<<"rm ";
			mv<<"mv "<<name<<"_merged.root "<<name<<".root ";
			hadd<<"hadd "<<name<<"_merged.root "<<name<<".root ";
			for(size_t ii=0; ii<G4Threading::G4GetNumberOfCores(); ii++){
				hadd<<name<<"_t"<<ii<<".root ";
				rm<<iName<<" ";
			}

			G4cout<<hadd<<G4endl;
			system(hadd.str().c_str());
			G4cout<<rm<<G4endl;
			system(rm.str().c_str());
			G4cout<<mv<<G4endl;
			system(mv.str().c_str());
		}
		ClearFileNames();
	}
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
