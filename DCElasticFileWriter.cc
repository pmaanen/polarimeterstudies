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

private:


};

DCElasticFileWriter::DCElasticFileWriter(){

}

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
int main(int argc, char** argv){

	namespace po = boost::program_options;
	po::options_description description("Usage");
	std::ifstream cfg;
	po::store(po::parse_command_line(argc, argv, description), vm);
	notify(vm);

	DCElasticFileWriter dc();
	int nevents=vm["nevents"].as<int>();
	ParticleMomentumVector ievent;
	for(int ii=0;ii<nevents;ii++){
		//ievent=dc.GenerateEvent();
		for(auto imom : ievent){
			G4cout<<imom.first->GetParticleName()<< " "<<imom.second<<G4endl;
		}
	}
	return 0;
}
