/*
 * JediVSensitiveDetector.hh
 *
 *  Created on: 19.05.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISENSITIVEDETECTOR_HH_
#define INCLUDE_JEDISENSITIVEDETECTOR_HH_

#include <G4VSensitiveDetector.hh>
#include <SensitiveDetectorManager.hh>
#include <memory>
#include "Analysis.hh"
#include "JediVSensitiveDetector.hh"
class G4Run;

class JediSensitiveDetector : public G4VSensitiveDetector
{
public:
	JediSensitiveDetector(const G4String& name, const SDtype& type);
	JediSensitiveDetector(const G4String& name);
	virtual ~JediSensitiveDetector();
	void Print(){};
	void SetType(G4String command);
	G4String GetName()const{return fName;};
	SDtype GetType() const;

	virtual void Initialize(G4HCofThisEvent*);
	virtual void EndOfEvent(G4HCofThisEvent* hitCollection);
	virtual void clear();
	virtual void DrawAll();
	virtual void PrintAll();

	void AddSD(JediVSensitiveDetector* sd);
	void WriteHitsToFile(TTree* aTree, const G4Run* aRun) const;
	virtual void CopyHitsToRun(simevent_t* anEvent);
	virtual void CopyHitsToRun(SimEvent* anEvent){fSD->CopyHitsToRun(anEvent);};

protected:
	// methods from base class
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

private:
	JediSensitiveDetector(const JediSensitiveDetector& rhs);
	JediSensitiveDetector& operator=(const JediSensitiveDetector& rhs);

	void SetType_impl(SDtype type);
	std::unique_ptr<JediVSensitiveDetector> fSD;
	SDtype fType;
	std::unique_ptr<G4GenericMessenger> fMessenger;
	G4String fName;
	G4String fHitsCollectionName;
	void DefineCommands();
};
#endif /* INCLUDE_JEDISENSITIVEDETECTOR_HH_ */
