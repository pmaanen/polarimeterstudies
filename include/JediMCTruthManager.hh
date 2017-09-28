///*
// * MCTruthManager.hh
// *
// *  Created on: 25.02.2017
// *      Author: pmaanen
// */
//
//#ifndef INCLUDE_JEDIMCTRUTHMANAGER_HH_
//#define INCLUDE_JEDIMCTRUTHMANAGER_HH_
//
//
//#include "hit.hh"
//#include <G4ThreadLocalSingleton.hh>
//
//class GenEventProducer;
//class G4Run;
//class TTree;
//
//class JediMCTruthManager {
//public:
//
//	friend class G4ThreadLocalSingleton<JediMCTruthManager>;
//	static JediMCTruthManager* Instance() {
//		static G4ThreadLocalSingleton<JediMCTruthManager> theInstance;
//		return theInstance.Instance();
//	}
//
//	virtual ~JediMCTruthManager()=default;
//
//	virtual void WriteHitsToFile(TTree& aTree, const G4Run* aRun) const;
//	virtual void CopyHitsToRun(genevent_t& anEvent) const;
//	virtual const std::vector<const GenEventProducer*> GetGenerators() const;
//
//
//	void RegisterMe(GenEventProducer*);
//	void UnRegisterMe(GenEventProducer*);
//
//private:
//	JediMCTruthManager();
//	std::vector<const GenEventProducer*> fGenerators;
//};
//
//#endif /* INCLUDE_JEDIMCTRUTHMANAGER_HH_ */
