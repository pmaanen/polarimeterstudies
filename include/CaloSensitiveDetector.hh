/*
 * CaloSensitiveDetector.hh
 *
 *  Created on: 05.11.2014
 *      Author: pmaanen
 */

#ifndef CALOSENSITIVEDETECTOR_HH_
#define CALOSENSITIVEDETECTOR_HH_

#include <G4MultiFunctionalDetector.hh>

class CaloSensitiveDetector: public G4MultiFunctionalDetector {
public:
	CaloSensitiveDetector(G4String name, G4int depth=0);
	virtual void EndOfEvent(G4HCofThisEvent* HC);
	virtual void Initialize(G4HCofThisEvent* HC);

	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
private:
	std::vector<G4int> fMyTupleId;
	G4bool fRunInitialized;
	G4String fName;
};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
