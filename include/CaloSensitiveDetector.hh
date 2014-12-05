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
	CaloSensitiveDetector(G4String name);
	virtual void EndOfEvent(G4HCofThisEvent* HC);
	virtual void Initialize(G4HCofThisEvent* HC);
private:
	std::vector<G4int> myTupleId;
};

#endif /* CALOSENSITIVEDETECTOR_HH_ */
