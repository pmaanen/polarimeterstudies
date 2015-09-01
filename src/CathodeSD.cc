#include "CathodeSD.hh"
#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4ThreeVector.hh>
#include <G4Track.hh>
#include <G4DynamicParticle.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CathodeSensitiveDetector::CathodeSensitiveDetector(G4String name)
  : G4VSensitiveDetector(name), savetime(true), saveposition(false), saveenergy(false)
{
  G4String HCname;
  collectionName.insert(HCname="pmtCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CathodeSensitiveDetector::~CathodeSensitiveDetector(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CathodeSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  photonCollection = new CathodeHitsCollection
                          (SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection( HCID, photonCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CathodeSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  if (aStep->GetTrack()->GetDefinition()->GetParticleType() != "opticalphoton")
    return false;

  if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()!="Cathode")
    return false;

  G4double energy = aStep->GetPostStepPoint()->GetTotalEnergy();

  if(energy==0.)
    return false;

  CathodeHit* newHit = new CathodeHit;
  newHit->SetEnergy(energy);
  newHit->SetTime  (aStep->GetPostStepPoint()->GetGlobalTime());
  newHit->SetPos   (aStep->GetPostStepPoint()->GetPosition());
  photonCollection->insert( newHit );

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CathodeSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  G4int NbHits = photonCollection->entries();
    G4cout << "=============" << G4endl << "\nCathode: " << NbHits << " optical photons detected" << G4endl;

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

