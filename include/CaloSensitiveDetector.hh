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
	CaloSensitiveDetector(G4String name, G4int depth=0);
	virtual void EndOfEvent(G4HCofThisEvent* HC);
	virtual void Initialize(G4HCofThisEvent* HC);
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
	void BeginOfRun();
	void EndOfRun();
	std::vector<calorhit_t> * GetHitVector();
private:
	G4String fName;
	TTree* fOutTree;
	std::vector<calorhit_t> * fHitVector;
};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
