/*
 * SingleCrystal.hh
 *
 *  Created on: 18.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_SINGLECRYSTAL_HH_
#define INCLUDE_SINGLECRYSTAL_HH_

#include <JediPolarimeter.hh>

class SingleCrystal: public JediPolarimeter {
public:
	SingleCrystal();
	virtual ~SingleCrystal();
	void setPhi(G4double phi) {
		this->fPhi = phi;
		fGeometryHasBeenChanged=true;
	}

	void setPsi(G4double psi) {
		this->fPsi = psi;
		fGeometryHasBeenChanged=true;
	}

	void setTheta(G4double theta) {
		this->fTheta = theta;
		fGeometryHasBeenChanged=true;
	}

protected:
	virtual G4LogicalVolume* BuildCaloCrystal();
	virtual G4LogicalVolume* MakeDetector(G4String name, G4Material* mat,G4double halfSizeX, G4double halfSizeY, G4double halfSizeZ);
	virtual G4VPhysicalVolume* Construct();
	virtual void DefineCommands();
	G4double fTheta,fPhi,fPsi;
};
#endif
