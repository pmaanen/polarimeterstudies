/*
 * GDMLReader.cc
 *
 *  Created on: 18.09.2017
 *      Author: pmaanen
 */

#include "GDMLReader.hh"

GDMLReader::GDMLReader(G4String filename):G4VUserDetectorConstruction(),fParser() {
	fParser.Read(filename);

}

G4VPhysicalVolume* GDMLReader::Construct() {
	return fParser.GetWorldVolume();
}

void GDMLReader::ConstructSDandField() {
	  ///////////////////////////////////////////////////////////////////////
	  //
	  // Example how to retrieve Auxiliary Information for sensitive detector
	  //
	  const G4GDMLAuxMapType* auxmap = fParser.GetAuxMap();
	  G4cout << "Found " << auxmap->size()
	            << " volume(s) with auxiliary information."
	            << G4endl << G4endl;
	  for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
	      iter!=auxmap->end(); iter++)
	  {
	    G4cout << "Volume " << ((*iter).first)->GetName()
	           << " has the following list of auxiliary information: "
	           << G4endl << G4endl;
	    for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
	         vit!=(*iter).second.end(); vit++)
	    {
	      G4cout << "--> Type: " << (*vit).type
	                << " Value: " << (*vit).value << G4endl;
	    }
	  }
	  G4cout << G4endl;
/*
	  // The same as above, but now we are looking for
	  // sensitive detectors setting them for the volumes

	  for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
	      iter!=auxmap->end(); iter++)
	  {
	    G4cout << "Volume " << ((*iter).first)->GetName()
	           << " has the following list of auxiliary information: "
	           << G4endl << G4endl;
	    for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
	         vit!=(*iter).second.end();vit++)
	    {
	      if ((*vit).type=="SensDet")
	      {
	        G4cout << "Attaching sensitive detector " << (*vit).value
	               << " to volume " << ((*iter).first)->GetName()
	               <<  G4endl << G4endl;

	        G4VSensitiveDetector* mydet =
	          SDman->FindSensitiveDetector((*vit).value);
	        if(mydet)
	        {
	          G4LogicalVolume* myvol = (*iter).first;
	          myvol->SetSensitiveDetector(mydet);
	        }
	        else
	        {
	          G4cout << (*vit).value << " detector not found" << G4endl;
	        }
	      }
	    }
	  }
	  */
}
