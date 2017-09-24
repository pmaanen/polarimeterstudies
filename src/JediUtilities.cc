#include "JediUtilities.hh"
#include "JediConfigurationManager.hh"

void JediLog(const G4String& message, const G4int verboseLevel){
  if(JediConfigurationManager::Instance()->GetVerbose()>verboseLevel)
    G4cout<<message<<G4endl;
}
