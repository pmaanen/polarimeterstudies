/*
 * JediVSensitiveDetector.hh
 *
 *  Created on: 19.05.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISENSITIVEDETECTOR_HH_
#define INCLUDE_JEDISENSITIVEDETECTOR_HH_

#include <G4VSensitiveDetector.hh>
#include <memory>
#include "SensitiveDetectorConfiguration.hh"


class JediSensitiveDetector_impl: public G4VSensitiveDetector {
public:
	JediSensitiveDetector_impl(const G4String& name):G4VSensitiveDetector(name){};
	virtual ~JediSensitiveDetector_impl(){};

	// methods from base class
	virtual void   Initialize(G4HCofThisEvent* hitCollection)=0;
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history)=0;
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection)=0;
	virtual void BeginOfRun()=0;
	virtual void EndOfRun()=0;

};

class JediSensitiveDetector : public G4VSensitiveDetector {
public:
	JediSensitiveDetector(const G4String& name, const SDtype& type);
	virtual ~JediSensitiveDetector(){}
	void Print();
	void SetType(G4String command);
	SDtype GetType(){return fType;};


	// methods from base class
	virtual void   Initialize(G4HCofThisEvent* hitCollection){if(fSD) fSD->Initialize(hitCollection);};
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history){if(fSD) return fSD->ProcessHits(step,history); return false;};
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection){if(fSD) fSD->EndOfEvent(hitCollection);};
	virtual void BeginOfRun(){if(fSD) fSD->BeginOfRun();};
	virtual void EndOfRun(){if(fSD) fSD->EndOfRun();};



private:
	void SetType_impl(SDtype type);
	std::unique_ptr<JediSensitiveDetector_impl> fSD;
	SDtype fType;
	std::unique_ptr<G4GenericMessenger> fMessenger;
	G4String fName;
	G4String fHitsCollectionName;

	void DefineCommands();
};

#endif /* INCLUDE_JEDISENSITIVEDETECTOR_HH_ */
