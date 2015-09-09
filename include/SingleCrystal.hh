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
		this->phi = phi;
		changedParameters=true;
	}

	void setPsi(G4double psi) {
		this->psi = psi;
		changedParameters=true;
	}

	void setTheta(G4double theta) {
		this->theta = theta;
		changedParameters=true;
	}
protected:
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSDandField();

	void defineSurfaces();

private:
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

	G4double airThickness;
	G4PVPlacement *physiScint, *physiAirGap,*physiCathode,*physiWindow,*physiGrease;

	G4Cache<CathodeSensitiveDetector*> CathodeSD;
	G4Cache<CaloSensitiveDetector*> CrystalSD;
	G4double theta,phi,psi;
};
#endif
