/*
 * SingleSandwichModule.hh
 *
 *  Created on: Nov 3, 2015
 *      Author: maanen
 */

#ifndef SINGLESANDWICHMODULE_HH_
#define SINGLESANDWICHMODULE_HH_

#include "SingleCrystal.hh"

class SingleSandwichModule : public SingleCrystal {
public:
	SingleSandwichModule();
	virtual ~SingleSandwichModule();

	void setAbsorberMaterialName(G4String absorberMaterialName) {
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

	void setAbsorberLength(const G4double absorberLength) {
		fAbsorberLength = absorberLength;
		fGeometryHasBeenChanged=true;
	}

protected:
	virtual G4LogicalVolume* MakeCaloCrystal();
	virtual G4VPhysicalVolume* Construct();
	virtual void DefineCommands();
	G4int fNumLayers;
	G4double fAbsorberLength;
	G4Material* fAbsorberMaterial;
	G4String fAbsorberMaterialName;
};

#endif /* SINGLESANDWICHMODULE_HH_ */
