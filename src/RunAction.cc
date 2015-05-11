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
	PushBackFileName(Analysis::Instance()->GetFileName());
	if (!IsMaster()) //it is a slave, do nothing else
	{
		Analysis::Instance()->CreateH1("slave","slave",1,0,1);
		G4cout << "ooo Run " << aRun->GetRunID() << " starts on slave." << G4endl;
		Analysis::Instance()->CreateH2("thetap","Theta vs p",100,0,10000,45,0,90);
	}
	else{
		Analysis::Instance()->CreateH1("master","master",1,0,1);
		Analysis::Instance()->CreateH2("thetap","Theta vs p",100,0,10000,45,0,90);
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
		if(filenames){
			std::ostringstream hadd;
			std::ostringstream rm;
			std::ostringstream mv;
			rm<<"rm ";
			mv<<"mv "<<Analysis::Instance()->GetFileName()<<"_merged.root "<<Analysis::Instance()->GetFileName()<<".root ";;
			hadd<<"hadd "<<Analysis::Instance()->GetFileName()<<"_merged.root ";
			G4cout<<"I have "<<filenames->size()<<" filenames. ";
			G4cout<<"These are: ";
			for(auto iName : *filenames){
				G4cout<<iName<<" ";
				hadd<<iName<<" ";
				rm<<iName<<" ";
			}
			G4cout<<G4endl;
			system(hadd.str().c_str());
			system(rm.str().c_str());
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

#include "G4AutoLock.hh"

namespace { G4Mutex RunActionMutex = G4MUTEX_INITIALIZER; }


void RunAction::ClearFileNames() {
	G4AutoLock lock(&RunActionMutex);
	if(filenames)
		delete filenames;
	filenames=0;
}

void RunAction::PushBackFileName(G4String filename) {
	G4AutoLock lock(&RunActionMutex);
	if(!filenames)
		filenames=new std::vector<G4String>;

	std::ostringstream fullName;
	if(G4Threading::IsWorkerThread())
		fullName<<filename<<"_t"<<G4Threading::G4GetThreadId()<<".root";
	else
		fullName<<filename<<".root";

	filenames->push_back(fullName.str());
}
