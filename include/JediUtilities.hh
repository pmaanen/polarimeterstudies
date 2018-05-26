/*                                                                                                                                                                                                                                                                                   
 * JediUtilities                                                                                                                                                                                                                                                 
 *                                                                                                                                                                                                                                                                                         
 * Created on: 24.09.2017                                                                                                                                                                                                                                                                  
 *      Author: pmaanen                                                                                                                                                                                                                                                                     
 */

#ifndef INCLUDE_JEDIUTILITIES_HH_
#define INCLUDE_JEDIUTILITIES_HH_

#include "JediConfigurationManager.hh"
template<typename T>
void TestLog(T message, const G4int verboseLevel){
	if(JediConfigurationManager::Instance()->GetVerbose()>verboseLevel)
		G4cout<<message<<G4endl;
}


void JediLog(const G4String& message, const G4int verboseLevel);
#endif /* INCLUDE_JEDIUTILITIES_HH_ */
