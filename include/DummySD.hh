/*
 * DummySD.hh
 *
 *  Created on: Dec 4, 2013
 *      Author: maanen
 */

#ifndef DUMMYSD_HH_
#define DUMMYSD_HH_
// Dummy sensitive used only to flag sensitivity
// in cells of RO geometry.
//
#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4Step;

class DummySD : public G4VSensitiveDetector
{
public:
  DummySD(G4String name):G4VSensitiveDetector(name){};
  ~DummySD() {};

  void Initialize(G4HCofThisEvent* ) {};
  G4bool ProcessHits(G4Step*,G4TouchableHistory*) {return false;}
  void EndOfEvent(G4HCofThisEvent*) {};
  void clear() {};
  void DrawAll() {};
  void PrintAll() {};
};

#endif /* DUMMYSD_HH_ */
