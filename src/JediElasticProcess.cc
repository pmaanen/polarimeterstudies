/*
 * JediElasticScatteringProcess.cc
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#include <JediElasticProcess.hh>
#include <JediElasticModel.hh>
#include <G4ParticleChange.hh>
#include <G4ParticleDefinition.hh>
#include <G4HadronicInteraction.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4Element.hh>
#include <G4StableIsotopes.hh>
#include <G4IonTable.hh>
#include <G4TrackStatus.hh>
#include <TGenPhaseSpace.h>
#include <TLorentzVector.h>
#include "global.hh"
#include "G4Threading.hh"


#include "G4HadronElasticProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4Nucleus.hh"
#include "G4ProcessManager.hh"
#include "G4CrossSectionDataStore.hh"
#include "G4ProductionCutsTable.hh"
#include "G4HadronicException.hh"




JediElasticProcess::JediElasticProcess():G4HadronicProcess( "dcelastic" ), fModel( nullptr ), theTotalResult( nullptr ) {
	theTotalResult = new G4ParticleChange();
}

JediElasticProcess::~JediElasticProcess() {
	delete theTotalResult;
}

G4VParticleChange* JediElasticProcess::PostStepDoIt(const G4Track& track,
		const G4Step&) {
	theTotalResult->Clear();
	theTotalResult->Initialize(track);
	G4double weight = track.GetWeight();
	theTotalResult->ProposeWeight(weight);

	// For elastic scattering, _any_ result is considered an interaction
	ClearNumberOfInteractionLengthLeft();

	G4double kineticEnergy = track.GetKineticEnergy();
	const G4DynamicParticle* dynParticle = track.GetDynamicParticle();
	const G4ParticleDefinition* part = dynParticle->GetDefinition();

	// NOTE: Low energies cause the model to vomit
	if (kineticEnergy <= 30*CLHEP::MeV)   return theTotalResult;

	// Initialize the hadronic projectile from the track
	//  G4cout << "track " << track.GetDynamicParticle()->Get4Momentum()<<G4endl;
	G4HadProjectile theProj(track);
	G4HadFinalState* result = 0;
	targetNucleus=G4Nucleus(12,6);
	if(verboseLevel>1) {
		G4cout << "JediElasticProcess::PostStepDoIt for "
				<< part->GetParticleName()
				<< G4endl;
	}
	try
	{
		result = fModel->ApplyYourself( theProj, targetNucleus);
	}
	catch(const G4HadronicException &aR)
	{
		G4ExceptionDescription d;
		d<<"Caught exception in model application for "
				<<part->GetParticleName();
		G4Exception("JediElasticProcess::PostStepDoIt","",EventMustBeAborted,d.str().c_str());
	}

	// directions
	G4ThreeVector indir = track.GetMomentumDirection();
	G4ThreeVector it(0., 0., 1.);
	G4ThreeVector outdir = result->GetMomentumChange();

	if(verboseLevel>1) {
		G4cout << "Efin= " << result->GetEnergyChange()
														   << " de= " << result->GetLocalEnergyDeposit()
														   << " nsec= " << result->GetNumberOfSecondaries()
														   << " dir= " << outdir
														   << G4endl;
	}

	// energies
	G4double edep = result->GetLocalEnergyDeposit();
	G4double efinal = result->GetEnergyChange();
	if(efinal < 0.0) { efinal = 0.0; }
	if(edep < 0.0)   { edep = 0.0; }

	// NOTE:  Very low energy scatters were causing numerical (FPE) errors
	//        in earlier releases; these limits have not been changed since.
	if(efinal <= 30*CLHEP::MeV) {
		edep += efinal;
		efinal = 0.0;
	}

	// primary change
	theTotalResult->ProposeEnergy(efinal);
	auto phi=fModel->SamplePhi(kineticEnergy,track.GetPolarization().getY(),outdir.getTheta())*CLHEP::rad;
	G4TrackStatus status = track.GetTrackStatus();
	if(efinal > 0.0) {
		outdir.rotate(phi,indir);
		//outdir.rotateUz(indir);
		theTotalResult->ProposeMomentumDirection(outdir);

	} else {
		if(part->GetProcessManager()->GetAtRestProcessVector()->size() > 0)
		{ status = fStopButAlive; }
		else { status = fStopAndKill; }
		theTotalResult->ProposeTrackStatus(status);
	}

	theTotalResult->SetNumberOfSecondaries(0);


	//G4cout<<"outdir:"<<outdir.getTheta()/CLHEP::deg<<" "<<outdir.getPhi()/CLHEP::deg<<G4endl;
	// recoil
	if(result->GetNumberOfSecondaries() > 0) {
		G4DynamicParticle* p = result->GetSecondary(0)->GetParticle();

		if(p->GetKineticEnergy() > 100*CLHEP::MeV) {
			theTotalResult->SetNumberOfSecondaries(1);
			G4ThreeVector pdir = p->GetMomentumDirection();
			// G4cout << "recoil " << pdir << G4endl;
			//!! is not needed for models inheriting G4HadronElastic
			pdir.rotate(phi, indir);
			//pdir.rotateUz(indir);
			// G4cout << "recoil rotated " << pdir << G4endl;
			p->SetMomentumDirection(pdir);

			// in elastic scattering time and weight are not changed
			G4Track* t = new G4Track(p, track.GetGlobalTime(),
					track.GetPosition());
			t->SetWeight(weight);
			t->SetTouchableHandle(track.GetTouchableHandle());
			theTotalResult->AddSecondary(t);

		} else {
			edep += p->GetKineticEnergy();
			delete p;
		}
	}
	theTotalResult->ProposeLocalEnergyDeposit(edep);
	theTotalResult->ProposeNonIonizingEnergyDeposit(edep);
	result->Clear();
	return theTotalResult;
}

G4bool JediElasticProcess::IsApplicable(const G4ParticleDefinition& particle) {
	G4bool res=false;
	if(particle.GetPDGEncoding()==G4Deuteron::DeuteronDefinition()->GetPDGEncoding())
		res=true;
	return res;
}

void JediElasticProcess::RegisterModel(JediElasticModel* model) {
	fModel = model;
	G4HadronicProcess::RegisterMe( dynamic_cast< G4HadronicInteraction * >( fModel ) );
}