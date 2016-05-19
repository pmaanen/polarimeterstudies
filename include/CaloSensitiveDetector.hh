/*
 * CaloSensitiveDetector.hh
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#ifndef CALOSENSITIVEDETECTOR_HH_
#define CALOSENSITIVEDETECTOR_HH_

#include <G4MultiFunctionalDetector.hh>
#include "TTree.h"
#include <vector>
#include "hit.hh"
class CaloSensitiveDetector: public G4MultiFunctionalDetector {
public:
	class JediSensitiveDetector;
	friend JediSensitiveDetector;
	CaloSensitiveDetector(G4String name, G4int depth=0);
	virtual ~CaloSensitiveDetector();
	virtual void EndOfEvent(G4HCofThisEvent* HC);
	virtual void Initialize(G4HCofThisEvent* HC);
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

	std::vector<calorhit_t>* getVect() {
		return vect;
	}

private:
	std::vector<calorhit_t> * vect;
};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
