//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file DetectorHit.hh
/// \brief Definition of the DetectorHit class

#ifndef DetectorHit_h
#define DetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include <stdint.h>

/// Detector hit class
///
/// It defines data members to store the trackID, chamberNb, energy deposit,
/// and position of charged particles in a selected volume:
/// - fTrackID, fChamberNB, fEdep, fPos

class DetectorHit : public G4VHit
{
public:
	DetectorHit();
	DetectorHit(const DetectorHit&);
	virtual ~DetectorHit();

	// operators
	const DetectorHit& operator=(const DetectorHit&);
	G4int operator==(const DetectorHit&) const;

	inline void* operator new(size_t);
	inline void  operator delete(void*);

	// methods from base class
	virtual void Draw();
	virtual void Print();

	// Set methods
	void SetTrackID  (G4int track)      { fTrackID = track; };
	void SetDetId(uint32_t rawid)      { fId = rawid; };
	void SetEdep     (G4double de)      { fEdep = de; };
	void SetPos      (G4ThreeVector xyz){ fPos = xyz; };
	void SetTruePos      (G4ThreeVector xyz){ fTruePos = xyz; };
	void SetParticleId(G4int pid)        { fParticleId = pid; };
	void SetTof(G4double tof)			{ fTof=tof; };
	// Get methods
	G4int GetTrackID() const     { return fTrackID; };
	uint32_t GetDetId() const   { return fId; };
	G4double GetEdep() const     { return fEdep; };
	G4ThreeVector GetPos() const { return fPos; };
	G4ThreeVector GetTruePos() const { return fTruePos; };
	G4int GetParticleId() const      { return fParticleId; };
	G4double GetTof() const {
		return fTof;
	}
	G4double GetEtot() const {
		return fEkin;
	}

	void SetEkin(G4double etot) {
		fEkin = etot;
	}

	const G4ThreeVector& getMom() const {
		return fMom;
	}

	void setMom(const G4ThreeVector& mom) {
		fMom = mom;
	}

	;
private:

	G4int         fTrackID;
	G4int 		fParticleId;
	uint32_t      fId;
	G4double      fEdep;
	G4ThreeVector fPos;
	G4ThreeVector fTruePos;
	G4ThreeVector fMom;
	G4double      fTof;
	G4double	fEkin;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;

extern G4ThreadLocal G4Allocator<DetectorHit>* DetectorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* DetectorHit::operator new(size_t)
{
	if(!DetectorHitAllocator)
		DetectorHitAllocator = new G4Allocator<DetectorHit>;
	void *hit;
	hit = (void *) DetectorHitAllocator->MallocSingle();
	return hit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void DetectorHit::operator delete(void *hit)
{
	DetectorHitAllocator->FreeSingle((DetectorHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
