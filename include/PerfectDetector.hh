/*
 * PerfectDetector.hh
 *
 *  Created on: Mar 7, 2016
 *      Author: maanen
 */

#ifndef PERFECTDETECTOR_HH_
#define PERFECTDETECTOR_HH_

#include "TrackerSensitiveDetector.hh"

class PerfectDetector: public TrackerSensitiveDetector {
public:
	PerfectDetector(const G4String& name);

	// methods from base class
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

	virtual void EndOfEvent(G4HCofThisEvent* hitCollection);
private:
	std::vector<G4int> fTrackIds;
};

#endif /* PERFECTDETECTOR_HH_ */
