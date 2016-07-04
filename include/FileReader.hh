/*
 * FileReader.hh
 *
 *  Created on: 24.08.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_FILEREADER_HH_
#define INCLUDE_FILEREADER_HH_
class FileReader_impl{
public:
	FileReader_impl():iEvent(0){}
	virtual genevent_t GetEvent()=0;
	virtual ~FileReader_impl(){};
protected:
	G4int iEvent;
	std::list<genevent_t> evCache;
};
class FileReader_ascii: public FileReader_impl{
public:
	FileReader_ascii(G4String fileName):FileReader_impl(){
		fInputFile.open(fileName.data());
	}
	virtual ~FileReader_ascii(){fInputFile.close();};

	genevent_t GetEvent(){
		if( evCache.size() == 0 )
		{
			genevent_t thisEvent;
			particle_t thisParticle(0,0,0,0,0);
			std::string line;
			for(auto iev=0;iev<100;++iev){
				while(std::getline(fInputFile,line)){
					G4double px,py,pz,vx,vy,vz,t,E;
					G4int ev,id;
					std::istringstream iss(line);
					iss>>ev>>id>>px>>py>>pz>>E>>vx>>vy>>vz>>t;
					px*=CLHEP::GeV,py*=CLHEP::GeV,pz*=CLHEP::GeV;
					thisParticle=particle_t(id,px,py,pz,E);
					thisEvent.time=t;
					thisEvent.x=vx;
					thisEvent.y=vy;
					thisEvent.z=vz;
					if(ev!=iEvent){
						evCache.push_back(thisEvent);
						thisEvent.particles.clear();
						thisEvent.particles.push_back(thisParticle);
						iEvent=ev;
						break;
					}
					else{
						thisEvent.particles.push_back(thisParticle);
					}
				}
			}
		}
		genevent_t ev = evCache.front();
		evCache.pop_front();
		return ev;
	}
private:
	std::ifstream fInputFile;
};

class FileReader_root:public FileReader_impl{
public:
	FileReader_root(G4String fileName):FileReader_impl(),fInputFile(nullptr),fInputTree(nullptr){
#ifdef MYDEBUG
		G4cout<<"FileReader_root::FileReader_root("<<fileName<<")"<<G4endl;
#endif
		fInputFile=new TFile(fileName,"READ");
		if(!fInputFile || fInputFile->IsZombie())
			G4Exception("[FileReader]", "FileReader", RunMustBeAborted,
					" ERROR: Input file not found.");

		fInputTree=dynamic_cast<TTree*>(fInputFile->Get("gen"));
		if(!fInputTree || fInputTree->IsZombie())
			G4Exception("[FileReader]", "FileReader", RunMustBeAborted,
					" ERROR: Input Tree not found.");
		fCurEntry=0;
	}
	genevent_t GetEvent(){

#ifdef MYDEBUG
		G4cout<<"FileReader_root::GetEvent()"<<G4endl;
#endif
		if( evCache.size() == 0 )
		{
			genevent_t* curEvent=nullptr;
			fInputTree->SetBranchAddress("gen",&curEvent);
			genevent_t thisEvent;
			particle_t thisParticle(0,0,0,0,0);
			std::string line;
			for(auto iev=0;iev<100;++iev){
					if(fCurEntry>fInputTree->GetEntries())
						G4Exception("FileReader_root","NoMoreEvents",RunMustBeAborted,"No more events in input file.");
					fInputTree->GetEvent(fCurEntry++);
					evCache.push_back(*curEvent);

			}
		}
		genevent_t ev = evCache.front();
		evCache.pop_front();
        /*
#ifdef MYDEBUG
		for(auto particle : ev){
			G4cout<<particle<<G4endl;
		}
#endif
         */
		return ev;
	}
	virtual ~FileReader_root(){
		delete fInputTree;
		fInputFile->Close();
		delete fInputFile;
	}
private:
	TFile* fInputFile;
	TTree* fInputTree;
	Int_t fCurEntry;
};

class FileReader{
public:
	FileReader(G4String fileName):fFileReader(nullptr){
		OpenFile(fileName);
	}
	genevent_t GetEvent(){
		return fFileReader->GetEvent();
	}
	virtual ~FileReader(){delete fFileReader;};
	void setFileName(G4String fileName){if(fileName==fFileName) return; else OpenFile(fileName);};

private:
	FileReader_impl* fFileReader;
	G4String fFileName;
	void OpenFile(G4String fileName){
		fFileName=fileName;
		if(fFileReader){
			delete fFileReader;
		}
		if(fileName.contains(".root")){
			fFileReader=new FileReader_root(fileName);
		}
		else
			fFileReader=new FileReader_ascii(fileName);
	}

};
#endif /* INCLUDE_FILEREADER_HH_ */
