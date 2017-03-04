/*
 * JediVSensitiveDetector.hh
 *
 *  Created on: 04.03.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIVSENSITIVEDETECTOR_HH_
#define INCLUDE_JEDIVSENSITIVEDETECTOR_HH_

#include <G4VSensitiveDetector.hh>

class TTree;
class G4Run;

class simevent_t;
class JediVSensitiveDetector : public G4VSensitiveDetector
{
public:
	JediVSensitiveDetector(const G4String& name):G4VSensitiveDetector(name){};
	virtual ~JediVSensitiveDetector() = default;

	virtual void WriteHitsToFile(TTree* aTree, const G4Run* aRun) const = 0;
	virtual void CopyHitsToRun(simevent_t* anEvent) const = 0;

private:
	JediVSensitiveDetector(const JediVSensitiveDetector& rhs);
	JediVSensitiveDetector& operator=(const JediVSensitiveDetector& rhs);
};


#endif /* INCLUDE_JEDIVSENSITIVEDETECTOR_HH_ */
