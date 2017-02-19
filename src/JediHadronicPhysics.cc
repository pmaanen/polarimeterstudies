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
JediHadronicPhysics::JediHadronicPhysics(G4int ver):G4VPhysicsConstructor("Jedi"),wasActivated(false) {

	fElastic=std::unique_ptr<JediElasticModel>(new JediElasticModel);
	fBreakup=std::unique_ptr<JediBreakupModel>(new JediBreakupModel);
	fMessenger=std::unique_ptr<G4GenericMessenger>(new G4GenericMessenger(this,"/PolarimeterStudies/hadronic/"));

	verboseLevel=ver;

	fMessenger->DeclareProperty("verboseLevel",JediHadronicPhysics::verboseLevel,"");

	fUseFastSim=gConfig["physics.use_fast_sim"].as<bool>();


	if(gVerbose>2)
		G4cout<<"JediHadronicPhysics::JediHadronicPhysics()"
		<<G4endl;
}

void JediHadronicPhysics::ConstructParticle() {
	G4GenericIon::GenericIonDefinition();
	G4Deuteron::DeuteronDefinition();
}

void JediHadronicPhysics::ConstructProcess() {

	auto elasticProcess=new JediElasticProcess;
	elasticProcess->RegisterModel(fElastic.get());
	JediStudiedProcess* wrapperProcess=nullptr;
	if(fUseFastSim){
		elasticProcess->AddDataSet(new JediFakeCrossSectionData);
		wrapperProcess=new JediStudiedProcess;
		wrapperProcess->RegisterProcess(elasticProcess);
		if(gVerbose>2)
			G4cout<<"JediHadronicPhysics::ConstructProcess()"
			<<" using wrapper process"
			<<G4endl;
	}
	else{
		elasticProcess->AddDataSet(new JediElasticCrossSectionData(1));
		if(gVerbose>2)
			G4cout<<"JediHadronicPhysics::ConstructProcess()"
			<<" using elastic process"
			<<G4endl;
	}

	auto particle=G4Deuteron::DeuteronDefinition();
	if ( particle )
	{
		G4ProcessManager *  processManager( particle->GetProcessManager() );
		if(fUseFastSim)
			processManager->AddDiscreteProcess( wrapperProcess );
		else
			processManager->AddDiscreteProcess(elasticProcess);
	}

}
