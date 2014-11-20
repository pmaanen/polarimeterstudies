/*
 * StripROGeometry.h
 *
 *  Created on: Dec 4, 2013
 *      Author: maanen
 */

#ifndef STRIPROGEOMETRY_H_
#define STRIPROGEOMETRY_H_
#include <G4VReadOutGeometry.hh>
#include "DetectorConstruction.hh"
#include "G4String.hh"
class StripROGeometry: public G4VReadOutGeometry {
public:
	StripROGeometry();
	StripROGeometry(G4String aString,G4double length,G4double width ,G4double thickness, G4double pitch);
	virtual ~StripROGeometry();

protected:
	  G4VPhysicalVolume* Build();

private:
	  DetectorConstruction* detector;
	  G4double Length,Width,Thickness,Pitch;
};

#endif /* STRIPROGEOMETRY_H_ */
