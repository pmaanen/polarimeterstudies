#define FILEWRITER
#include "DCElasticEventGenerator.hh"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "G4ParticleDefinition.hh"
#include <G4Deuteron.hh>
#include "G4IonTable.hh"
#include "G4ios.hh"
#include "TCanvas.h"
#include "TH1.h"
#include "Randomize.hh"
#define MAIN
#include "global.hh"


class DCElasticFileWriter : public DCElasticEventGenerator{
public:
	DCElasticFileWriter();
	virtual ~DCElasticFileWriter();
	virtual void Initialize();
	virtual ParticleMomentumVector GenerateEvent();

private:


};

DCElasticFileWriter::~DCElasticFileWriter(){}

DCElasticFileWriter::DCElasticFileWriter():DCElasticEventGenerator(){}

void DCElasticFileWriter::Initialize(){
	Double_t m_target = 11187.9 * CLHEP::MeV;
	Double_t m_beam = 1875 * CLHEP::MeV;
	target.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[2] = {m_beam, m_target} ;
	beam.SetPxPyPzE(0, 0, sqrt(beamEnergy/CLHEP::GeV*(beamEnergy/CLHEP::GeV+2*m_beam)), beamEnergy/CLHEP::GeV+m_beam);
	cms = beam + target;
	TLorentzVector temp=beam;
	temp.Boost(-cms.BoostVector());
	momentum_cms=temp.Vect().Mag();
	ps.SetDecay(cms, 2, masses);
	if(!SigmaFunc)
		BuildFunction();
	SigmaFunc->SetParameter(0,beamEnergy/CLHEP::MeV);
	SigmaFunc->SetParameter(1,momentum_cms);
	SigmaFunc->SetParameter(2,beamPolarization);
	MaxY=SigmaFunc->GetMaximum();
	Initialized=true;
}

ParticleMomentumVector DCElasticFileWriter::GenerateEvent(){

	if(!Initialized)
		Initialize();
	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		ps.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector pscattered_4 = *ps.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector precoil_4 = *ps.GetDecay(1) ;

		//spatial parts of generated L-vectors
		G4ThreeVector precoil_3(precoil_4.Vect().X()*CLHEP::GeV,precoil_4.Vect().Y()*CLHEP::GeV,precoil_4.Vect().Z()*CLHEP::GeV);
		G4ThreeVector pscattered_3(pscattered_4.Vect().X()*CLHEP::GeV,pscattered_4.Vect().Y()*CLHEP::GeV,pscattered_4.Vect().Z()*CLHEP::GeV);

		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = pscattered_3.getTheta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = pscattered_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered>3*CLHEP::deg and th_scattered<30*CLHEP::deg){

			//Boost momentum of deuteron from lab-sytem to cm-system.
			TVector3 CMv = cms.BoostVector();     // in case beam simulation

			pscattered_4.Boost(-CMv);          // in case beam simulation
			//retrieve polar scattering angle for deuteron in cm-frame
			G4double CM_theta_scattered = pscattered_4.Theta()*CLHEP::rad;
			G4double acc=MaxY*G4UniformRand();
			if(SigmaFunc->Eval(CM_theta_scattered/CLHEP::deg,phi_scattered/CLHEP::deg)<acc) continue;
			else {
				//G4cout<<G4endl;
				//G4cout<<"Lab:"<<th_scattered/CLHEP::deg<<G4endl;
				//G4cout<<"CMS:"<<CM_theta_scattered/CLHEP::deg<<G4endl;
				ParticleMomentumVector res;
				res.push_back(std::make_pair(1000010020,pscattered_3));
				res.push_back(std::make_pair(1000060120,precoil_3));
				return res;
			}
		}
	}
}
int main(int argc, char** argv){

	namespace po = boost::program_options;
	po::options_description description("Usage");
	description.add_options()
															("help,h", "Display this help message")
															("nevents,n", po::value<int>()->default_value(100), "number of events")
															("beam_energy,e", po::value<double>()->default_value(100),"energy of beam in MeV")
															("beam_polarization,p", po::value<double>()->default_value(1),"beam polarization");
	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), vm);
	notify(vm);

	DCElasticFileWriter *dc=new DCElasticFileWriter();
	dc->setBeamEnergy(vm["beam_energy"].as<double>()*CLHEP::MeV);
	dc->setBeamPolarization(vm["beam_polarization"].as<double>());
	int nevents=vm["nevents"].as<int>();
	ParticleMomentumVector ievent;
	for(int ii=0;ii<nevents;ii++){
		auto ievent=dc->GenerateEvent();
		for(auto imom : ievent){
			G4cout<<ii<<" "<<imom.first<<" "<<imom.second.getX()<<" "<<imom.second.getX()<<" "<<imom.second.getY()<<" "<<imom.second.getZ()<<G4endl;
		}
	}
	delete dc;
	return 0;
}
