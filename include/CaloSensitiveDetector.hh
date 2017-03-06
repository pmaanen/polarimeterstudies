/*
 * CaloSensitiveDetector.hh
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#ifndef CALOSENSITIVEDETECTOR_HH_
#define CALOSENSITIVEDETECTOR_HH_

#include <G4MultiFunctionalDetector.hh>
#include <JediClasses.hh>
#include "JediSensitiveDetector.hh"
#include "TTree.h"
#include <vector>
#include <memory>

class G4HCofThisEvent;
class G4Step;



class G4Run;
class CaloSensitiveDetector: public JediVSensitiveDetector {
	class JediSensitiveDetector;
	friend JediSensitiveDetector;
public:
	CaloSensitiveDetector(const G4String& name,G4int depth=0);
	virtual ~CaloSensitiveDetector()=default;
	virtual void EndOfEvent(G4HCofThisEvent* HC);
	void Initialize(G4HCofThisEvent*);
	virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* history);
	virtual void WriteHitsToFile(TTree* aTree, const G4Run* aRun) const;
	virtual void CopyHitsToRun(simevent_t* anEvent) const;
	virtual void CopyHitsToRun(SimEvent* anEvent) const;

	G4int getDepth() const {
		return fDepth;
	}

	void setDepth(G4int depth) {
		fDepth = depth;
	}

protected:

	G4int GetIndex(G4Step* aStep);
private:

	JediCalorimeterHitsCollection* CalorimeterCollection;
	std::map<G4int, G4double> fHitMap;
	std::map<G4int, G4int> fHitId;
	std::unique_ptr<std::vector<calorhit_t>> fHits;
	std::unique_ptr<std::vector<JediCalorimeterHit>> fCaloHits;
	G4int fDepth;

};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
