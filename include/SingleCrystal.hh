/*
 * SingleCrystal.hh
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_SINGLECRYSTAL_HH_
#define INCLUDE_SINGLECRYSTAL_HH_

#include <JediPolarimeter.hh>
class CathodeSensitiveDetector;
class SingleCrystal: public JediPolarimeter {
public:
	SingleCrystal();
	virtual ~SingleCrystal();
	void setPhi(G4double phi) {
		this->fPhi = phi;
		fChangedParameters=true;
	}

	void setPsi(G4double psi) {
		this->fPsi = psi;
		fChangedParameters=true;
	}

	void setTheta(G4double theta) {
		this->fTheta = theta;
		fChangedParameters=true;
	}

	void setCathodeThickness(G4double cathodeThickness) {
		this->cathodeThickness = cathodeThickness;
		changedParameters=true;
	}

	void setGreaseThickness(G4double greaseThickness) {
		this->greaseThickness = greaseThickness;
		changedParameters=true;
	}

	void setWindowThickness(G4double windowThickness) {
		this->windowThickness = windowThickness;
		changedParameters=true;
	}

protected:
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4VPhysicalVolume* Construct();
<<<<<<< HEAD
	virtual void ConstructSDandField();

	void defineSurfaces();

  virtual void DefineCommands();
	// surfaces
	G4OpticalSurface *LYSOPolishedAirTeflon; // polished LYSO surface wrapped with teflon
	G4OpticalSurface *LYSOGroundAirTeflon;   // ground LYSO surface wrapped with teflon

	G4OpticalSurface *polishedAir;          // polished crystal surface, not wrapped
	G4OpticalSurface *groundAir;            // ground crystal surface, not wrapped
	G4OpticalSurface *polishedWhitePainted; // polished crystal surface painted white
	G4OpticalSurface *groundWhitePainted;   // ground crystal surface painted white
	G4OpticalSurface *polishedBlackPainted; // polished crystal surface painted black
	G4OpticalSurface *groundBlackPainted;   // ground crystal surface painted black

	G4OpticalSurface *airGroundAluminum; // ground aluminm surface
	G4OpticalSurface *silicaCathodeMaterial; // surface between window and cathode


	G4OpticalSurface *BGOPolishedAirTeflon; // polished BGO surface wrapped with teflon
	G4OpticalSurface *BGOGroundAirTeflon;   // ground BGO surface wrapped with teflon

	G4double airThickness,greaseThickness,windowThickness,cathodeThickness;
	G4PVPlacement *physiScint, *physiAirGap,*physiCathode,*physiWindow,*physiGrease;

	G4Cache<CathodeSensitiveDetector*> CathodeSD;
	G4Cache<CaloSensitiveDetector*> CrystalSD;
	G4double theta,phi,psi;
=======
	virtual void DefineCommands();
	G4double fTheta,fPhi,fPsi;
>>>>>>> master
};
#endif
