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
		FileWriter_impl(G4int nEvents):currentEventId(0),nEvents(nEvents){};
		virtual bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents)=0;
		virtual ~FileWriter_impl(){};

	protected:
		G4int currentEventId,nEvents;
	};
	class FileWriter_ascii: public FileWriter_impl{
	public:
		FileWriter_ascii(G4String fileName,G4int nEvents):FileWriter_impl(nEvents),buf(0){
			if(fileName!=""){
				outFile.open(fileName.data());
				buf = outFile.rdbuf();
			}
			else{
				buf=std::cout.rdbuf();
			}
			out=new std::ostream(buf);
		}

		virtual ~FileWriter_ascii(){if(outFile.is_open())outFile.close();delete out;};

		virtual bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents){
			for(auto iEvent:someEvents){
				if(currentEventId==nEvents)
					return false;
				//Write one event to file, then check if goal is reached. If yes, return false. Otherwise continue until vector is empty
				for(auto iParticle:iEvent){
					*out<<currentEventId<<" "<<iParticle<<" "<<iEvent.t<<" "<<iEvent.vx<<" "<<iEvent.vy<<" "<<iEvent.vz<<std::endl;
				}
				currentEventId++;
			}
			return true;
		}

	private:
		std::ostream *out;
		std::streambuf * buf;
		std::ofstream outFile;
	};

	class FileWriter_root:public FileWriter_impl{
	public:
		FileWriter_root(G4String fileName, G4int nEvents):FileWriter_impl(nEvents),outFile(0),outTree(0){
			if(fileName!=""){
				outFile=new TFile(fileName.data(),"RECREATE");
			}
			else{
				outFile=new TFile("generator.root","RECREATE");
			}
			outTree = new TTree("gen","Generated Events");
			outTree->Branch("evt",&evt,"evt/I");
			outTree->Branch("pid",&pid,"pid/I");
			outTree->Branch("px",&px,"px/F");
			outTree->Branch("py",&py,"py/F");
			outTree->Branch("pz",&pz,"pz/F");
			outTree->Branch("vx",&vx,"vx/F");
			outTree->Branch("vy",&vy,"vy/F");
			outTree->Branch("vz",&vz,"vz/F");
			outTree->Branch("t",&t,"t/F");

		}

		bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents){
			for(auto iEvent:someEvents){
				if(currentEventId==nEvents)
					return false;
				//Write one event to file, then check if goal is reached. If yes, return false. Otherwise continue until vector is empty
				for(auto iParticle:iEvent){
					evt=currentEventId;
					pid=Int_t(iParticle.id);
					px=Float_t(iParticle.px/CLHEP::GeV);
					py=Float_t(iParticle.py/CLHEP::GeV);
					pz=Float_t(iParticle.pz/CLHEP::GeV);
					t=Float_t(iEvent.t);
					outTree->Fill();
				}
				currentEventId++;
			}
			return true;

		};
		virtual ~FileWriter_root(){outTree->Write();outFile->Write(); delete outTree; outFile->Close(); delete outFile;};
	private:
		TFile* outFile;
		TTree* outTree;
		Int_t evt,pid;
		Float_t px,py,pz,vx,vy,vz,t;
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
		bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents){
			return myFileWriter->WriteEventsToFile(someEvents);
		}
		virtual ~FileWriter(){delete myFileWriter;};

	private:
		FileWriter_impl* myFileWriter;
	};


	FileWriterPrimaryGeneratorAction(G4int nEvents,G4String fileName);
	virtual ~FileWriterPrimaryGeneratorAction();
	virtual void GeneratePrimaries(G4Event* E);
	private:
	EventGenerator* evtGen;
	static FileWriter* fileWriter;
};
#endif /* FILEWRITERPRIMARYGENERATORACTION_HH_ */
