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
class G4Run;


class JediSensitiveDetector_impl{
public:
	JediSensitiveDetector_impl(const G4String& name):fName(name){};
	virtual ~JediSensitiveDetector_impl()=default;

	// methods from base class
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history)=0;
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection)=0;
	const G4String& GetName() const {return fName;}

	virtual void WriteHitsToFile(TTree& aTree, const G4Run* aRun) const=0;
	virtual void CopyHitsToRun(simevent_t& anEvent) const=0;
protected:
	G4String fName;
};

class JediSensitiveDetector : public G4VSensitiveDetector {
public:
	JediSensitiveDetector(const G4String& name, const SDtype& type);
	virtual ~JediSensitiveDetector(){Analysis::Instance()->UnRegisterMe(this);};

	void Print() {};
	void SetType(G4String command);
	SDtype GetType(){return fType;};

	void WriteHitsToFile(TTree& aTree, const G4Run* aRun) const {if(fSD) fSD->WriteHitsToFile(aTree,aRun);} ;

	// methods from base class
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history){if(fSD) return fSD->ProcessHits(step,history); return false;};
	virtual void   EndOfEvent(G4HCofThisEvent* hitCollection){if(fSD) fSD->EndOfEvent(hitCollection);};
	virtual void CopyHitsToRun(simevent_t& anEvent){if(fSD) fSD->CopyHitsToRun(anEvent);}
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
