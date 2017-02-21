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
	class JediSensitiveDetector;
	friend JediSensitiveDetector;
public:
	CaloSensitiveDetector(const G4String& name);
	virtual ~CaloSensitiveDetector()=default;
	virtual void EndOfEvent(G4HCofThisEvent* HC);

	virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* history);
	virtual void WriteHitsToFile(TTree& aTree, const G4Run* aRun) const;
	virtual void CopyHitsToRun(simevent_t& anEvent) const;

private:
	std::map<G4int, G4double> fHitMap;
	std::unique_ptr<std::vector<calorhit_t>> fHits;
};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
