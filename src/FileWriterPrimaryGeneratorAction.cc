/*
 * FileWriterPrimaryGeneratorAction.cc
 *
 *  Created on: 18.05.2015
 *      Author: pmaanen
 */

#include <FileWriterPrimaryGeneratorAction.hh>
#include "DCElasticEventGenerator.hh"
#include "DCBreakupEventGenerator.hh"
#include "DCElasticTimeDependentGenerator.hh"
#include "CosmicMuonGenerator.hh"
#include "G4AutoLock.hh"

namespace { G4Mutex FileWriterMutex = G4MUTEX_INITIALIZER; }

FileWriterPrimaryGeneratorAction::FileWriter* FileWriterPrimaryGeneratorAction::fgFileWriter = 0;

FileWriterPrimaryGeneratorAction::FileWriterPrimaryGeneratorAction(G4int nEvents,G4String fileName=""):G4VUserPrimaryGeneratorAction(),fEvtGen(new DCElasticTimeDependentGenerator){
	G4AutoLock lock(&FileWriterMutex);
	if(!fgFileWriter){
			fgFileWriter=new FileWriter(fileName,nEvents);
	}
}

void FileWriterPrimaryGeneratorAction::GeneratePrimaries(G4Event*){
	/*
	 * I hijacked the fct to implement my event generation. This should only be called once per thread.
	 * TODO: Implement like following
	 * 1) Write events to internal cache
	 * 2) obtain lock
	 * 3) Send events to FileWriter
	 * 4) release lock
	 * 5) FileWriter returns false if no more events are needed, we can then return, otherwise start filling cache again;
	 *
	 * Example code from reader:
	 * G4AutoLock lock(&PrimaryGeneratorMutex);
	 *if( !fileReader ) fileReader = new PrimaryGeneratorAction::FileReader(_infile);
	 *
	 *PrimaryEvent evt;
	 *if(fileReader)
	 *{
	 *	G4AutoLock lock(&PrimaryGeneratorMutex);
	 *	evt = fileReader->GetEvent();
	 *}
	 */
	G4int cacheSize=10000;
	std::vector<PrimaryEvent> evtCache;
	G4bool doMore=true;
	while(doMore){
		for(G4int ii=0;ii<cacheSize;ii++){
			evtCache.push_back(fEvtGen->Generate());
		}
		if(fgFileWriter){
			G4AutoLock lock(&FileWriterMutex);
			doMore=fgFileWriter->WriteEventsToFile(evtCache);
			evtCache.clear();
		}
		else{
			std::stringstream o;
			o<<"pointer to file reader is invalid!"<<G4endl;
			G4Exception("FileWriterPrimaryGeneratorAction::GeneratePrimaries", "PointerError", FatalException,
					o.str().c_str());
		}
	}

	return;
}

FileWriterPrimaryGeneratorAction::~FileWriterPrimaryGeneratorAction() {
	delete fEvtGen;
	G4AutoLock lock(&FileWriterMutex);
		if(fgFileWriter){
				delete fgFileWriter;
				fgFileWriter=0;
		}
}
