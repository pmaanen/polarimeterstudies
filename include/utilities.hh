#ifndef utilities
#define utilities
#include "JediConfigurationManager.hh"
void Log(const G4String message, const G4int verboseLevel){
  if(JediConfigurationManager::Instance()->GetVerbose()>verboseLevel)
    G4cout<<message<<G4endl;
}
#endif
