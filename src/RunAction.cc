#include "RunAction.hh"
#include "global.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "Analysis.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "Randomize.hh"
#include <ctime>
#include "PrimaryGeneratorAction.hh"

#include "G4AutoLock.hh"

namespace { G4Mutex RunActionMutex = G4MUTEX_INITIALIZER; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::RunAction()
{
	Analysis::Instance();



	fSeed = -1;      // RANLUX seed
	fLuxury = 3;     // RANLUX luxury level (3 is default)
	fSaveRndm = 0;
	fNEvents=0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
	G4AutoLock lock(&RunActionMutex);
	delete Analysis::Instance();}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//*********************************************************************************
//******************  Begin Of Run Action                   ***********************
//*********************************************************************************

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
	fNEvents=aRun->GetNumberOfEventToBeProcessed();
	//Analysis::Instance()->OpenFile();
	//PushBackFileName(Analysis::Instance()->GetFileName());
	auto an=Analysis::Instance();
	if(true){
		//Analysis::Instance()->OpenFile(Analysis::Instance()->GetFileName());
	}
	if (!IsMaster()) //it is a slave, do nothing else
	{
		G4cout << "ooo Run " << aRun->GetRunID() << " starts on slave." << G4endl;
		an->BeginOfRun();
	}
	else{}

	//Master or sequential
	G4cout << "ooo Run " << aRun->GetRunID() << " starts (global)." << G4endl;
	if (fSeed<0) //not initialized by anybody else
	{

		fSeed=time(0);
		G4Random::setTheSeed(fSeed,fLuxury);
	}

	// save Rndm status
	if (fSaveRndm > 0)
		G4Random::saveEngineStatus("beginOfRun.rndm");

	return;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//*********************************************************************************
//******************  End Of Run Action                   ***********************
//*********************************************************************************
void RunAction::EndOfRunAction(const G4Run* aRun)
{
	auto an=Analysis::Instance();
	if(!IsMaster()){
		an->EndOfRun();
	}
	if(true){
		if(IsMaster()){
				std::ostringstream hadd;
				std::ostringstream rm;
				std::ostringstream mv;
				std::ostringstream command;
				G4String extension;
				auto name=Analysis::Instance()->getFileName();
				if ( name.find(".") != std::string::npos ) {
					extension = name.substr(name.find("."));
					name = name.substr(0, name.find("."));
				}
				else {
					extension = ".";
					extension.append("root");
				}
				hadd<<"hadd -v 0 -f "<<Analysis::Instance()->getFileName()<<" ";
				for(int ii=0;ii<G4MTRunManager::GetMasterRunManager()->GetNumberOfThreads(); ii++){
					hadd<<name<<"_t"<<ii<<extension<<" ";
					rm<<"rm "<<name<<"_t"<<ii<<extension<<";";
				}
				command<<hadd.str()<<"; "<<rm.str();
				//G4cout<<command.str()<<G4endl;
				system(command.str().c_str());
		}
	}
	if (!IsMaster())
	{
		G4cout << "### Run " << aRun->GetRunID() << " (slave) ended." << G4endl;
		return;
	}
	// Complete clean-up
	G4cout << "### Run " << aRun->GetRunID() << " (global) ended." << G4endl;
	return;

}
