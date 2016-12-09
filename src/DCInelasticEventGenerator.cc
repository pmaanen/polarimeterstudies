/*
 * DCInelasticEventGenerator.cc
 *
 *  Created on: 18.03.2016
 *      Author: pmaanen
 */

#include <DCInelasticEventGenerator.hh>
#include "G4Deuteron.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "TMath.h"
#include "G4ParticleGun.hh"
#include "global.hh"
#include "VertexGeneratorO.hh"
#include "VertexGeneratorU.hh"

#include "TF1.h"
DCInelasticEventGenerator::DCInelasticEventGenerator():PhaseSpaceGenerator("dcinelastic"),fBeamPolarization(0) {
	fBeamEnergy=270.*CLHEP::MeV;
	fInitialized=false;

	fThetaMin=2.0*CLHEP::deg;
	fThetaMax=180.*CLHEP::deg;
	fScatteringModel=std::unique_ptr<DeuteronCarbonInelasticScatteringModel>(new DeuteronCarbonInelasticScatteringModel);
	DefineCommands();

}

genevent_t DCInelasticEventGenerator::Generate() {


	if(!fInitialized)
		Initialize();
	genevent_t thisEvent;
	auto pos=VertexGeneratorO::GetInstance()->generateVertex();
	pos.setZ(VertexGeneratorU::GetInstance()->generateVertex().getZ());
	thisEvent.x=pos.x();
	thisEvent.y=pos.y();
	thisEvent.z=pos.z();


	return thisEvent;
}

void DCInelasticEventGenerator::Initialize() {
	fParticles.push_back(G4Deuteron::DeuteronDefinition());
	fParticles.push_back(G4IonTable::GetIonTable()->GetIon(6,12));

	/*
	for(const auto& ipart : fParticles){
		if(*ipart==nullptr)
			G4Exception("DCElasticEventGenerator::DCElasticEventGenerator()","",FatalException,"beam particle not found.");
	}
	 */
	Double_t m_target = G4IonTable::GetIonTable()->GetIon(6,12)->GetPDGMass()/CLHEP::GeV;
	Double_t m_beam = G4Deuteron::DeuteronDefinition()->GetPDGMass()/CLHEP::GeV;
	fTarget.SetPxPyPzE(0.0, 0.0, 0.0, m_target);
	fBeam.SetPxPyPzE(0, 0, sqrt(fBeamEnergy/CLHEP::GeV*(fBeamEnergy/CLHEP::GeV+2*m_beam)), fBeamEnergy/CLHEP::GeV+m_beam);
	fCms = fBeam + fTarget;
	if(!fScatteringModel)
		fScatteringModel=std::unique_ptr<DeuteronCarbonInelasticScatteringModel>(new DeuteronCarbonInelasticScatteringModel());

	VertexGeneratorO::GetInstance()->setBeamposition(fBeamposition.getX(),fBeamposition.getY(),fBeamposition.getZ());
	VertexGeneratorO::GetInstance()->setBeamsize(fBeamsize.getX(),fBeamsize.getY(),fBeamsize.getZ());
	VertexGeneratorU::GetInstance()->setBeamsize(0,0,fBeamsize.getZ());
	//fQ=std::unique_ptr<TF1>(new TF1("q",fScatteringModel.get(),&DeuteronCarbonElasticScatteringModel::q,0,TMath::Pi(),1,"DeuteronCarbonElasticScatteringModel","q"));
	fQEx=std::unique_ptr<TF2>(new TF2("qEx",fScatteringModel.get(),&DeuteronCarbonInelasticScatteringModel::QEx,0,5,0,100,1));
	fPhi=std::unique_ptr<TF1>(new TF1("Phi",fScatteringModel.get(),&DeuteronCarbonInelasticScatteringModel::Phi,0,2*TMath::Pi(),3));

	fInitialized=true;
}
