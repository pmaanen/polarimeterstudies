/*
 * hit.hh
 *
 *  Created on: Jul 24, 2013
 *      Author: maanen
 */
#ifndef HIT_HH_
#define HIT_HH_
#include <TObject.h>
#include <vector>
#include <map>
#include <string>
class calorhit_t: public TObject {
public:
	calorhit_t():TObject(),edep(0),detid(0){;};
	Double_t edep;
	Int_t detid;
	ClassDef(calorhit_t,2)  //Event structure
};

class trackerhit_t : public TObject {
public:
	trackerhit_t():TObject(),x(0),y(0),z(0),px(0),py(0),pz(0),edep(0),pid(0),trid(0),tof(0),ekin(0),detid(0){;};
	Double_t x,y,z,px,py,pz,edep,pid,trid,tof,ekin,detid;
	ClassDef(trackerhit_t,5)
};

class simevent_t: public TObject {
public:
	simevent_t():TObject(),eventid(0),time(0){;};
	Int_t eventid;
	Int_t time;
	std::map<std::string,std::vector<calorhit_t> > calorimeter;
	std::map<std::string, std::vector<trackerhit_t> > tracker;
	ClassDef(simevent_t,2)
};

class particle_t : public TObject{
public:
	particle_t():TObject(),px(0),py(0),pz(0),E(0),id(0){;};
	particle_t(Int_t iid,Double_t ppx,Double_t ppy,Double_t ppz,Double_t ee):id(iid),px(ppx),py(ppy),pz(ppz),E(ee){;};
/*
 * id(iid),px(ppx),py(ppy),pz(ppz),E(ee)
	friend std::ostream& operator<< (std::ostream& stream, const particle_t& particle){
		stream<<particle.id<<" "<<particle.px/CLHEP::GeV<<" "<<particle.py/CLHEP::GeV<<" "<<particle.pz/CLHEP::GeV<<particle.E/CLHEP::GeV;
		return stream;
	};
*/
	Int_t id;
	Double_t px,py,pz,E;
	ClassDef(particle_t,5)
};

class genevent_t: public TObject {
public:
	genevent_t():TObject(),eventid(0),time(0),x(0),y(0),z(0){;};
	genevent_t(Int_t iid, Int_t tt, Double_t vvx, Double_t vvy, Double_t vvz):eventid(iid),time(tt),x(vvx),y(vvy),z(vvz){;};
	//std::vector<particle_t>::iterator begin(){return particles.begin();}
	//std::vector<particle_t>::iterator end(){return particles.end();}
	/*
	 * :TObject(),eventid(iid),time(tt),x(vvx),y(vvy),z(vvz)
	friend std::ostream& operator<< (std::ostream& stream, const genevent_t& event){
		for(auto ipart: event.particles){
			stream<<ipart<<event.x/CLHEP::mm<<" "<<event.y/CLHEP::mm<<" "<<event.z/CLHEP::mm<<" "<<event.time/CLHEP::s;
		}
		return stream;
	};
	*/
	Int_t eventid;
	Double_t time,x,y,z;
	std::vector<particle_t> particles;
	ClassDef(genevent_t,2);
};
#endif /* HIT_HH_ */
