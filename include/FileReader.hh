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
	virtual PrimaryEvent GetEvent()=0;
	virtual ~FileReader_impl(){};
protected:
	G4int iEvent;
	std::list<PrimaryEvent> evCache;
};
class FileReader_ascii: public FileReader_impl{
public:
	FileReader_ascii(G4String fileName):FileReader_impl(){
		inputFile.open(fileName.data());
	}
	virtual ~FileReader_ascii(){inputFile.close();};

	PrimaryEvent GetEvent(){
		if( evCache.size() == 0 )
		{
			PrimaryEvent thisEvent;
			PrimaryParticle thisParticle(0,0,0,0);
			std::string line;
			for(auto iev=0;iev<100;++iev){
				while(std::getline(inputFile,line)){
					G4double px,py,pz,vx,vy,vz,t;
					G4int ev,id;
					std::istringstream iss(line);
					iss>>ev>>id>>px>>py>>pz>>vx>>vy>>vz>>t;
					px*=CLHEP::GeV,py*=CLHEP::GeV,pz*=CLHEP::GeV;
					thisParticle=PrimaryParticle(id,px,py,pz);
					thisEvent.t=t;
					thisEvent.vx=vx;
					thisEvent.vy=vy;
					thisEvent.vz=vz;
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
		PrimaryEvent ev = evCache.front();
		evCache.pop_front();
		return ev;
	}
private:
	std::ifstream inputFile;
};

class FileReader_root:public FileReader_impl{
public:
	FileReader_root(G4String fileName):FileReader_impl(){
		inputFile=new TFile(fileName,"READ");
		inputTree=dynamic_cast<TTree*>(inputFile->Get("gen"));
		curEntry=0;
	}
	PrimaryEvent GetEvent(){
		if( evCache.size() == 0 )
		{
			PrimaryEvent thisEvent;
			PrimaryParticle thisParticle(0,0,0,0);
			std::string line;
			Float_t px,py,pz,vx,vy,vz,t;
			Int_t ev,id;
			inputTree->SetBranchAddress("evt",&ev);
			inputTree->SetBranchAddress("pid",&id);
			inputTree->SetBranchAddress("px",&px);
			inputTree->SetBranchAddress("py",&py);
			inputTree->SetBranchAddress("pz",&pz);
			inputTree->SetBranchAddress("vx",&vx);
			inputTree->SetBranchAddress("vy",&vy);
			inputTree->SetBranchAddress("vz",&vz);
			inputTree->SetBranchAddress("t",&t);
			for(auto iev=0;iev<100;++iev){
				if(curEntry>inputTree->GetEntries())
					G4Exception("FileReader_root","NoMoreEvents",RunMustBeAborted,"No more events in input file.");
				inputTree->GetEvent(curEntry++);
				thisParticle=PrimaryParticle(id,G4double(px*CLHEP::GeV),G4double(py*CLHEP::GeV),G4double(pz*CLHEP::GeV));
				thisEvent.t=t;
				thisEvent.vx=vx;
				thisEvent.vy=vy;
				thisEvent.vz=vz;
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
		PrimaryEvent ev = evCache.front();
		evCache.pop_front();
		return ev;
	}
	virtual ~FileReader_root(){
		delete inputTree;
		inputFile->Close();
		delete inputFile;
	}
private:
	TFile* inputFile;
	TTree* inputTree;
	Int_t curEntry;
};

class FileReader{
public:
	FileReader(G4String fileName){
		if(fileName.contains(".root")){
			myFileReader=new FileReader_root(fileName);
		}
		else
			myFileReader=new FileReader_ascii(fileName);
	}
	PrimaryEvent GetEvent(){
		return myFileReader->GetEvent();
	}
	virtual ~FileReader(){delete myFileReader;};
private:
	FileReader_impl* myFileReader;

};
#endif /* INCLUDE_FILEREADER_HH_ */
