#ifndef CathodeSD_h
#define CathodeSD_h 1

#include <G4VSensitiveDetector.hh>
#include "CathodeHit.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CathodeSensitiveDetector : public G4VSensitiveDetector
{
  public:
      CathodeSensitiveDetector(G4String);
     ~CathodeSensitiveDetector();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      CathodeHitsCollection* photonCollection;
      bool savetime;
      bool saveposition;
      bool saveenergy;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

