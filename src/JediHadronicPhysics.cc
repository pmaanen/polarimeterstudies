/*
 * JediHadronicPhysics.cc
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#include <JediHadronicPhysics.hh>
#include "JediElasticProcess.hh"
#include "JediBreakupProcess.hh"
#include <G4ProcessManager.hh>
#include <JediFakeCrossSectionData.hh>
#include <JediElasticCrossSectionData.hh>
#include <JediBreakupCrossSectionData.hh>
#include <G4IonTable.hh>
#include "JediStudiedProcess.hh"
#include "global.hh"
JediHadronicPhysics::JediHadronicPhysics(G4int ver):G4VPhysicsConstructor("Jedi"),wasActivated(false),fUseFastSim(true) {

	fElastic=std::unique_ptr<JediElasticModel>(new JediElasticModel);
	fBreakup=std::unique_ptr<JediBreakupModel>(new JediBreakupModel);
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,"/PolarimeterStudies/hadronic/"));

	verboseLevel=ver;

	fMessenger->DeclareProperty("verboseLevel",JediHadronicPhysics::verboseLevel,"");

	auto fastSimCmd=fMessenger->DeclareProperty("fastsim",JediHadronicPhysics::fUseFastSim,"");
	//elaCmd.SetStates(G4State_PreInit, G4State_Idle);
	//brkpCmd.SetStates(G4State_PreInit, G4State_Idle);

	fastSimCmd.SetStates(G4State_PreInit);

	if(gVerbose>2)
		G4cout<<"JediHadronicPhysics::JediHadronicPhysics()"
		<<G4endl;
}

void JediHadronicPhysics::ConstructParticle() {
	G4GenericIon::GenericIonDefinition();
	G4Deuteron::DeuteronDefinition();
}

void JediHadronicPhysics::ConstructProcess() {

	//if ( wasActivated )
	//	return;

	wasActivated = true;
	auto elasticProcess=new JediElasticProcess;
	G4VProcess* theProcess=nullptr;
	elasticProcess->RegisterModel(fElastic.get());
	if(fUseFastSim){
		elasticProcess->AddDataSet(new JediFakeCrossSectionData);
		auto wrapperProcess=new JediStudiedProcess();
		wrapperProcess->RegisterProcess(elasticProcess);
		theProcess=wrapperProcess;
		if(gVerbose>2)
			G4cout<<"JediHadronicPhysics::ConstructProcess()"
			<<" using wrapper process"
			<<G4endl;
	}
	else{
		elasticProcess->AddDataSet(new JediElasticCrossSectionData(1));
		theProcess=elasticProcess;
		if(gVerbose>2)
			G4cout<<"JediHadronicPhysics::ConstructProcess()"
			<<" using elastic process"
			<<G4endl;
	}

	/*

	auto breakupProcess=new JediBreakupProcess;
	breakupProcess->RegisterModel(fBreakup.get());
	breakupProcess->AddDataSet(new JediBreakupCrossSectionData(0));
	 */
	auto particle=G4Deuteron::DeuteronDefinition();
	if ( particle )
	{
		G4ProcessManager *  processManager( particle->GetProcessManager() );
		processManager->AddDiscreteProcess( theProcess );
		//processManager->AddDiscreteProcess( breakupProcess );
	}

}
