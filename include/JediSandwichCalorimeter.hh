/*
 * SandwichCalorimeter.hh
 *
 *  Created on: 21.04.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISANDWICHCALORIMETER_HH_
#define INCLUDE_JEDISANDWICHCALORIMETER_HH_

#include <SingleCrystal.hh>
#include "JediCubicPolarimeter.hh"
class JediSandwichCalorimeter: public JediCubicPolarimeter {
public:
	JediSandwichCalorimeter();
	virtual ~JediSandwichCalorimeter();


	void setAbsorberMaterialName(const G4String& absorberMaterialName) {
		auto oldName=fAbsorberMaterial->GetName();
		auto newMat=G4NistManager::Instance()->FindOrBuildMaterial(absorberMaterialName);
		if(!newMat){
			G4Exception("JediPolarimeter::setAbsorberMaterialName","MatNotFound",G4ExceptionSeverity::JustWarning,"Material not found! Material not changed.");
			return;
		}
		fAbsorberMaterial=newMat;
		fAbsorberMaterialName=absorberMaterialName;
		if(JediConfigurationManager::Instance()->GetVerbose()==3)
			G4cout<<"Changing Material from "<<oldName<<" to "<<fAbsorberMaterial->GetName()<<G4endl;
		return;
	}

	void setNumLayers(G4double numLayers) {
		fNumLayers = numLayers;
		fGeometryHasBeenChanged=true;
	}

	G4double getAbsorberLength() const {
		return fAbsorberLength;
	}

	void setAbsorberLength(G4double absorberLength) {
		fAbsorberLength = absorberLength;
		fGeometryHasBeenChanged=true;
	}


	virtual void DefineCommands();
	virtual G4LogicalVolume* MakeCaloCrystal();

	virtual G4VPhysicalVolume* Construct();
protected:
	G4int fNumLayers;
	G4double fAbsorberLength;
	G4Material* fAbsorberMaterial;
	G4String fAbsorberMaterialName;
};

#endif /* INCLUDE_JEDISANDWICHCALORIMETER_HH_ */
