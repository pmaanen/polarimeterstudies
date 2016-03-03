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
		virtual bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents)=0;
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

		virtual bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents){
			for(auto iEvent:someEvents){
				if(fCurrentEventId==fNEvents)
					return false;
				//Write one event to file, then check if goal is reached. If yes, return false. Otherwise continue until vector is empty
				for(auto iParticle:iEvent){
					*fOut<<fCurrentEventId<<" "<<iParticle<<" "<<iEvent.t<<" "<<iEvent.vx<<" "<<iEvent.vy<<" "<<iEvent.vz<<std::endl;
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
		FileWriter_root(G4String fileName, G4int nEvents):FileWriter_impl(nEvents),fOutFile(0),fOutTree(0){
			if(fileName!=""){
				fOutFile=new TFile(fileName.data(),"RECREATE");
			}
			else{
				fOutFile=new TFile("generator.root","RECREATE");
			}
			fOutTree = new TTree("gen","Generated Events");
			fOutTree->Branch("evt",&fEvt,"evt/I");
			fOutTree->Branch("pid",&fPid,"pid/I");
			fOutTree->Branch("px",&fPx,"px/F");
			fOutTree->Branch("py",&fPy,"py/F");
			fOutTree->Branch("pz",&fPz,"pz/F");
			fOutTree->Branch("vx",&fVx,"vx/F");
			fOutTree->Branch("vy",&fVy,"vy/F");
			fOutTree->Branch("vz",&fVz,"vz/F");
			fOutTree->Branch("t",&fT,"t/F");

		}

		bool WriteEventsToFile(const std::vector<PrimaryEvent> &someEvents){
			for(auto iEvent:someEvents){
				if(fCurrentEventId==fNEvents)
					return false;
				//Write one event to file, then check if goal is reached. If yes, return false. Otherwise continue until vector is empty
				for(auto iParticle:iEvent){
					fEvt=fCurrentEventId;
					fPid=Int_t(iParticle.id);
					fPx=Float_t(iParticle.px/CLHEP::GeV);
					fPy=Float_t(iParticle.py/CLHEP::GeV);
					fPz=Float_t(iParticle.pz/CLHEP::GeV);
					fVx=Float_t(iEvent.vx/CLHEP::mm);
					fVy=Float_t(iEvent.vy/CLHEP::mm);
					fVz=Float_t(iEvent.vz/CLHEP::mm);

					fT=Float_t(iEvent.t);
					fOutTree->Fill();
				}
				fCurrentEventId++;
			}
			return true;

		};
		virtual ~FileWriter_root(){fOutTree->Write();fOutFile->Write(); delete fOutTree; fOutFile->Close(); delete fOutFile;};
	private:
		TFile* fOutFile;
		TTree* fOutTree;
		Int_t fEvt,fPid;
		Float_t fPx,fPy,fPz,fVx,fVy,fVz,fT;
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


	FileWriterPrimaryGeneratorAction(G4int nEvents,G4String generator,G4String fileName);
	virtual ~FileWriterPrimaryGeneratorAction();
	virtual void GeneratePrimaries(G4Event* E);
	private:
	EventGenerator* fEvtGen;
	static FileWriter* fgFileWriter;
};
#endif /* FILEWRITERPRIMARYGENERATORACTION_HH_ */
