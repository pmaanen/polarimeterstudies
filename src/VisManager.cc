// ********************************************************************
// *                                                                  *
// *                          EDM Polarimetry                         *
// *                                                                  *
// * Detector physics                                                 *
// * RWTH Aachen 24.10.2012                                           *
// * Fabian Hinder                                                    *
// ********************************************************************
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifdef G4VIS_USE

#include "VisManager.hh"
#include "G4HepRepFile.hh"


namespace CLHEP {}
using namespace CLHEP; 



#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif

polarimeterStudiesVisManager::polarimeterStudiesVisManager () {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void polarimeterStudiesVisManager::RegisterGraphicsSystems () {

	// Graphics Systems not needing external packages or libraries...

	RegisterGraphicsSystem (new G4HepRepFile);

	// Graphics systems needing external packages or libraries...

	#ifdef G4VIS_USE_OPENGLX
	RegisterGraphicsSystem (new G4OpenGLImmediateX);
	RegisterGraphicsSystem (new G4OpenGLStoredX);
	#endif


	if (fVerbose > 0) {
	G4cout << "\nYou have successfully chosen to use the following graphics systems." << G4endl;
	PrintAvailableGraphicsSystems ();
	}
}

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
