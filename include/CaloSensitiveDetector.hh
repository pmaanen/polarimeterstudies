/*
 * CaloSensitiveDetector.hh
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#ifndef CALOSENSITIVEDETECTOR_HH_
#define CALOSENSITIVEDETECTOR_HH_

#include <G4MultiFunctionalDetector.hh>
#include "JediSensitiveDetector.hh"
#include "TTree.h"
#include <vector>
#include <memory>
#include "hit.hh"
class G4Run;
class CaloSensitiveDetector: public JediSensitiveDetector_impl {
public:
	CaloSensitiveDetector(const G4String& name);
	virtual ~CaloSensitiveDetector();
	virtual void EndOfEvent(G4HCofThisEvent* HC);

	virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* history);
	virtual void WriteHitsToFile(TTree& aTree, const G4Run* aRun) const;
	std::vector<calorhit_t>* getVect() {return vect.get();}

private:
	std::map<G4int, G4double> fHitMap;
	std::unique_ptr<std::vector<calorhit_t>> vect;
};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
