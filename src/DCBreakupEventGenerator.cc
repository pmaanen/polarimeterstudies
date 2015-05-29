/*
 * DCBreakupEventGenerator.cc
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#include <DCBreakupEventGenerator.hh>
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4ParticleGun.hh"
DCBreakupEventGenerator::DCBreakupEventGenerator(G4ParticleGun* gun):PhaseSpaceGenerator(gun) {
	beamEnergy=235.*CLHEP::MeV;
	Initialized=false;

	thetaMin=5*CLHEP::deg;
	thetaMax=20*CLHEP::deg;
	DefineCommands();
}

DCBreakupEventGenerator::~DCBreakupEventGenerator() {
	// TODO Auto-generated destructor stub
}

TF2* DCBreakupEventGenerator::BuildFunction() {
	SigmaFunc=new TF2("tf1","sin(x)*exp(-x)",0,100,0,100);
	return SigmaFunc;
}

void DCBreakupEventGenerator::Initialize() {
	particles.push_back(G4Proton::ProtonDefinition());
	particles.push_back(G4Neutron::NeutronDefinition());
	particles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));
	for(auto ipart=particles.begin();ipart!=particles.end();++ipart){
		if(!(*ipart))
			throw;//G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","DC001",0,"beam particle not found.");
	}
	Double_t m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;
	Double_t m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	target.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	Double_t masses[3] = {particles[0]->GetPDGMass()/CLHEP::GeV,particles[1]->GetPDGMass()/CLHEP::GeV,particles[1]->GetPDGMass()/CLHEP::GeV} ;
	beam.SetPxPyPzE(0, 0, sqrt(beamEnergy/CLHEP::GeV*(beamEnergy/CLHEP::GeV+2*m_beam)), beamEnergy/CLHEP::GeV+m_beam);
	cms = beam + target;
	ps.SetDecay(cms, 3, masses);
	if(!SigmaFunc)
		BuildFunction();
	//MaxY=SigmaFunc->GetMaximum();
	Initialized=true;
}

void DCBreakupEventGenerator::Generate(G4Event* E) {
	auto event=PrimaryEvent(Generate());
	for(auto iPart=event.particles.begin();iPart!=event.particles.end();++iPart){
		//TODO Write Truth
		pGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(iPart->id));
		pGun->SetParticleMomentum(G4ThreeVector(iPart->px,iPart->py,iPart->pz));
		pGun->GeneratePrimaryVertex(E);
	}
	return;
}

PrimaryEvent DCBreakupEventGenerator::Generate() {

	if(!Initialized)
		Initialize();
	while (1) {


		//Sample an event assuming constant cross-section in cm-system
		ps.Generate();

		//L-vector of scattered particle in lab-frame
		TLorentzVector proton_4 = *ps.GetDecay(0);

		//L-vector of recoil particle in lab-frame
		TLorentzVector neutron_4 = *ps.GetDecay(1) ;

		//L-vector of recoil particle in lab-frame
		TLorentzVector carbon_4 = *ps.GetDecay(2) ;

		//spatial parts of generated L-vectors
		G4ThreeVector proton_3(proton_4.Vect().X()*CLHEP::GeV,proton_4.Vect().Y()*CLHEP::GeV,proton_4.Vect().Z()*CLHEP::GeV);
		G4ThreeVector neutron_3(neutron_4.Vect().X()*CLHEP::GeV,neutron_4.Vect().Y()*CLHEP::GeV,neutron_4.Vect().Z()*CLHEP::GeV);
		G4ThreeVector carbon_3(carbon_4.Vect().X()*CLHEP::GeV,carbon_4.Vect().Y()*CLHEP::GeV,carbon_4.Vect().Z()*CLHEP::GeV);
		//Magnitude of spatial vectors
		//G4double momentum_recoil  = precoil_3.mag();
		//G4double momentum_scattered  = pscattered_3.mag();


		//Polar angle for deuteron in lab-frame (degrees)
		G4double th_scattered  = proton_3.getTheta()*CLHEP::rad;
		//Polar angle for proton in lab-frame (degrees)
		//G4double th_recoil  = precoil_3.getTheta();

		G4double phi_scattered = proton_4.Vect().Phi();//pscattered_3.getPhi();
		if(phi_scattered<0)
			phi_scattered+=360*CLHEP::deg;

		//G4double phi_recoil = precoil_3.getPhi();

		//Set angular cut in lab-frame
		if(th_scattered>thetaMin and th_scattered<thetaMax){
				PrimaryEvent res;


				PrimaryParticle a(2212,1.,1.,1.);
				res.particles.push_back(PrimaryParticle(2212,proton_3.getX(),proton_3.getY(),proton_3.getZ()));
				res.particles.push_back(PrimaryParticle(2112,neutron_3.getX(),neutron_3.getY(),neutron_3.getZ()));
				res.particles.push_back(PrimaryParticle(1000060120,carbon_3.getX(),carbon_3.getY(),carbon_3.getZ()));
				G4cout<<"BREAKUP!"<<G4endl;
				//Analysis::Instance()->FillH1(1, th_scattered/CLHEP::deg);
				//Analysis::Instance()->FillH1(2, phi_scattered/CLHEP::deg);
				//Analysis::Instance()->FillH1(3, th_scattered/CLHEP::deg,1/sin(th_scattered));
				return res;
			}
		}
}
