/*
 * JediVSensitiveDetector.hh
 *
 *  Created on: 04.03.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIVSENSITIVEDETECTOR_HH_
#define INCLUDE_JEDIVSENSITIVEDETECTOR_HH_

#include <G4VSensitiveDetector.hh>
#include "SensitiveDetectorManager.hh"
class TTree;
class G4Run;

class simevent_t;
class SimEvent;
class JediVSensitiveDetector : public G4VSensitiveDetector
{
public:
	JediVSensitiveDetector(const G4String& name):G4VSensitiveDetector(name),fType(SDtype::kUndefined){};
	virtual ~JediVSensitiveDetector() = default;

	virtual void WriteHitsToFile(TTree* aTree, const G4Run* aRun) const = 0;
	virtual void CopyHitsToRun(simevent_t* anEvent) const = 0;
	virtual void CopyHitsToRun(SimEvent*) const {};
	SDtype GetType() const {return fType;}

protected:
	virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*)=0;
	SDtype fType;

};


#endif /* INCLUDE_JEDIVSENSITIVEDETECTOR_HH_ */
