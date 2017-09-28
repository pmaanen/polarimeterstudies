/*
 * GDMLReader.hh
 *
 *  Created on: 18.09.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_GDMLREADER_HH_
#define INCLUDE_GDMLREADER_HH_

#include <G4VUserDetectorConstruction.hh>
#include <G4GDMLParser.hh>
class GDMLReader: public G4VUserDetectorConstruction {
public:
	GDMLReader(const G4String filename);
	virtual G4VPhysicalVolume *Construct();
	virtual void ConstructSDandField();

private:
	G4GDMLParser fParser;
};

#endif /* INCLUDE_GDMLREADER_HH_ */
