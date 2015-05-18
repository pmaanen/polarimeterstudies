/*
 * FileWriterPrimaryGeneratorAction.hh
 *
 *  Created on: 18.05.2015
 *      Author: pmaanen
 */

#ifndef FILEWRITERPRIMARYGENERATORACTION_HH_
#define FILEWRITERPRIMARYGENERATORACTION_HH_
#include "G4VUserPrimaryGeneratorAction.hh"
#include "EventGenerator.hh"

#include "G4String.hh"
#include "G4Types.hh"
#include <fstream>
class DCElasticEventGenerator;
class FileWriterPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
public:
	class FileWriter{
	public:
		FileWriter(G4String fileName,G4int nEvents):currentEventId(0),nEvents(nEvents){
			currentEventId=0;
			outFile.open(fileName.data());
		};
		~FileWriter(){outFile.close();};

		bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents){
			for(auto iEvent:someEvents){
				if(currentEventId==nEvents)
					return false;
				//Write one event to file, then check if goal is reached. If yes, return false. Otherwise continue until vector is empty
				for(auto iParticle:iEvent){
					outFile<<currentEventId<<" "<<iParticle<<std::endl;
				}
				currentEventId++;
			}
			return true;
		}

	private:
		G4int currentEventId,nEvents;
		std::ofstream outFile;
	};

	FileWriterPrimaryGeneratorAction(G4int nEvents,G4String fileName);
	virtual ~FileWriterPrimaryGeneratorAction();
	virtual void GeneratePrimaries(G4Event* E);
private:
	DCElasticEventGenerator* evtGen;
	static FileWriter* fileWriter;
};
#endif /* FILEWRITERPRIMARYGENERATORACTION_HH_ */
