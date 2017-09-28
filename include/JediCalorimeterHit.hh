/*
 * JediCalorimeterHit.hh
 *
 *  Created on: 04.03.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDICALORIMETERHIT_HH_
#define INCLUDE_JEDICALORIMETERHIT_HH_

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>

class JediCalorimeterHit: public G4VHit {
public:
	JediCalorimeterHit();
	virtual ~JediCalorimeterHit()=default;

	JediCalorimeterHit(const JediCalorimeterHit&);
	const JediCalorimeterHit& operator=(const
			JediCalorimeterHit&);
	int operator==(const JediCalorimeterHit&) const;

	inline void* operator new(size_t);
	inline void  operator delete(void*);

	void Draw();
	void Print();

	inline void AddEnergy(G4double de) {fEdep += de;};
	inline void SetId(G4int id) {fDetId = id;};
	inline void SetPos(G4ThreeVector xyz){ fPos = xyz; }

	inline G4ThreeVector GetPos() { return fPos; };
	inline G4double GetEdep()     { return fEdep; };
	inline G4int GetId()          { return fDetId; };

private:
	G4int fDetId;
	G4double fEdep;
	G4ThreeVector fPos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

typedef G4THitsCollection<JediCalorimeterHit> JediCalorimeterHitsCollection;

extern G4ThreadLocal G4Allocator<JediCalorimeterHit> *JediCalorimeterHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void* JediCalorimeterHit::operator new(size_t)
{
	if (!JediCalorimeterHitAllocator)
		JediCalorimeterHitAllocator = new G4Allocator<JediCalorimeterHit>;
	return (void*) JediCalorimeterHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline JediCalorimeterHit::JediCalorimeterHit(const JediCalorimeterHit& rhs):G4VHit() {
	fEdep=rhs.fEdep;
	fDetId=rhs.fDetId;
	fPos=rhs.fPos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline const JediCalorimeterHit& JediCalorimeterHit::operator =(
		const JediCalorimeterHit& rhs) {

	fEdep=rhs.fEdep;
	fDetId=rhs.fDetId;
	fPos=rhs.fPos;
	return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline int JediCalorimeterHit::operator ==(const JediCalorimeterHit& rhs) const {
	return ( this == &rhs ) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void JediCalorimeterHit::operator delete(void* aHit)
{
	JediCalorimeterHitAllocator->FreeSingle((JediCalorimeterHit*) aHit);
}

#endif /* INCLUDE_JEDICALORIMETERHIT_HH_ */
