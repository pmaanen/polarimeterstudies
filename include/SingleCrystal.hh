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
protected:
<<<<<<< HEAD
  virtual void ConstructSDandField();
  virtual G4LogicalVolume* MakeCaloCrystal();
  virtual G4VPhysicalVolume* Construct();
=======
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4VPhysicalVolume* Construct();
	virtual void DefineCommands();
<<<<<<< HEAD
	G4double theta,phi,psi;
>>>>>>> master
=======
	G4double fTheta,fPhi,fPsi;
>>>>>>> master
};
#endif
