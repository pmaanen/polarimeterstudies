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
#include "G4UIsession.hh"
#include <fstream>
#include "TFile.h"
#include "TTree.h"
class DCElasticEventGenerator;
class FileWriterPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
public:
	class FileWriter_impl{
	public:
		FileWriter_impl(G4int nEvents):fCurrentEventId(0),fNEvents(nEvents){};
		virtual bool WriteEventsToFile(const std::vector<genevent_t> &someEvents)=0;
		virtual ~FileWriter_impl(){};

	protected:
		G4int fCurrentEventId,fNEvents;
	};
	class FileWriter_ascii: public FileWriter_impl{
	public:
		FileWriter_ascii(G4String fileName,G4int nEvents):FileWriter_impl(nEvents),fBuffer(0){
			if(fileName!=""){
				fOutFile.open(fileName.data());
				fBuffer = fOutFile.rdbuf();
			}
			else{
				fBuffer=std::cout.rdbuf();
			}
			fOut=new std::ostream(fBuffer);
		}

		virtual ~FileWriter_ascii(){if(fOutFile.is_open())fOutFile.close();delete fOut;};

		virtual bool WriteEventsToFile(const std::vector<genevent_t> &someEvents){
			for(auto iEvent:someEvents){
				if(fCurrentEventId==fNEvents)
					return false;
				//Write one event to file, then check if goal is reached. If yes, return false. Otherwise continue until vector is empty
				for(auto iParticle:iEvent.particles){
					//*fOut<<fCurrentEventId<<" "<<iParticle<<" "<<iEvent.time<<" "<<iEvent.x<<" "<<iEvent.y<<" "<<iEvent.z<<std::endl;
				}
				fCurrentEventId++;
			}
			return true;
		}

	private:
		std::ostream *fOut;
		std::streambuf * fBuffer;
		std::ofstream fOutFile;
	};

	class FileWriter_root:public FileWriter_impl{
	public:
		FileWriter_root(G4String fileName, G4int nEvents):FileWriter_impl(nEvents),fOutFile(0),fOutTree(0),fGenEvent(nullptr){
			if(fileName!=""){
				fOutFile=new TFile(fileName.data(),"RECREATE");
			}
			else{
				fOutFile=new TFile("generator.root","RECREATE");
			}
			fOutTree = new TTree("gen","Generated Events");
			fOutTree->Branch("events","genevent_t",&fGenEvent);

		};

		bool WriteEventsToFile(const std::vector<genevent_t> &someEvents){
			for(auto iEvent:someEvents){
				fGenEvent=new genevent_t(iEvent);
				fOutTree->Fill();
				if(fGenEvent->eventid==fNEvents)
					return false;
			}
			return true;
		};
		virtual ~FileWriter_root(){fOutTree->Write();fOutFile->Write(); delete fOutTree; fOutFile->Close(); delete fOutFile;};
	private:
		TFile* fOutFile;
		TTree* fOutTree;
		genevent_t* fGenEvent;
	};
	class FileWriter{
	public:
		FileWriter(G4String fileName, G4int nEvents){
			if(fileName.contains(".root")){
				myFileWriter=new FileWriter_root(fileName,nEvents);
			}
			else
				myFileWriter=new FileWriter_ascii(fileName,nEvents);
		}
		bool WriteEventsToFile(const std::vector<genevent_t> &someEvents){
			return myFileWriter->WriteEventsToFile(someEvents);
		}
		virtual ~FileWriter(){delete myFileWriter;};

	private:
		FileWriter_impl* myFileWriter;
	};


	FileWriterPrimaryGeneratorAction(G4int nEvents,G4String generator,G4String fileName);
	virtual ~FileWriterPrimaryGeneratorAction();
	virtual void GeneratePrimaries(G4Event* E);
	private:
	EventGenerator* fEvtGen;
	static FileWriter* fgFileWriter;
};
#endif /* FILEWRITERPRIMARYGENERATORACTION_HH_ */
