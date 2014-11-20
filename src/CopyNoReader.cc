
#include "CopyNoReader.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CopyNoReader::CopyNoReader()
  : G4GDMLReadStructure()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CopyNoReader::~CopyNoReader()
{
  std::map<G4String, G4VisAttributes*>::iterator pos;
  for (pos=fAttribs.begin(); pos!=fAttribs.end(); pos++)
  {  delete pos->second;  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CopyNoReader::ExtensionRead(const xercesc::DOMElement* const extElement)
{
   G4cout << "G4GDML: Reading GDML extension..." << G4endl;
/*
   for (xercesc::DOMNode* iter = extElement->getFirstChild();
        iter != 0; iter = iter->getNextSibling())
   {
      if (iter->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)  { continue; }

      const xercesc::DOMElement* const child
            = dynamic_cast<xercesc::DOMElement*>(iter);
      const G4String tag = Transcode(child->getTagName());

      if (tag=="copyNo") { CopyNoRead(child); }
      else
      {
        G4String error_msg = "Unknown tag in structure: " + tag;
        G4Exception("CopyNoReader::ExtensionRead()",
                    "ReadError", FatalException, error_msg);
      }
   }
   */
   return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CopyNoReader::VolumeRead(const xercesc::DOMElement* const volumeElement)
{
   G4VSolid* solidPtr = 0;
   G4Material* materialPtr = 0;
   G4VisAttributes* attrPtr = 0;
   G4int copyNo=0;
   G4GDMLAuxListType auxList;
   
   XMLCh *name_attr = xercesc::XMLString::transcode("name");
   const G4String name = Transcode(volumeElement->getAttribute(name_attr));
   G4cout<<"Attribute name "<<name<<G4endl;
   xercesc::XMLString::release(&name_attr);

   for (xercesc::DOMNode* iter = volumeElement->getFirstChild();
        iter != 0; iter = iter->getNextSibling())
   {

      if (iter->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)  { continue; }

      const xercesc::DOMElement* const child
            = dynamic_cast<xercesc::DOMElement*>(iter);
      const G4String tag = Transcode(child->getTagName());
      G4cout<<"Tag name "<<tag<<G4endl;

      if (tag=="auxiliary")
        { auxList.push_back(AuxiliaryRead(child)); } else
      if (tag=="materialref")
        { materialPtr = GetMaterial(GenerateName(RefRead(child),true)); } else
      if (tag=="solidref")
        { solidPtr = GetSolid(GenerateName(RefRead(child))); }
      if(tag=="copyno"){
    	  //G4cout<<eval.EvaluateInteger(child->getAttributes());
      }

   }


   pMotherLogical = new G4LogicalVolume(solidPtr,materialPtr,
                                        GenerateName(name),0,0,0);
   pMotherLogical->SetVisAttributes(attrPtr);

   if (!auxList.empty()) { auxMap[pMotherLogical] = auxList; }

   Volume_contentRead(volumeElement);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CopyNoReader::CopyNoRead(const xercesc::DOMElement* const colorElement)
{
G4cout<<"CopyNoRead()"<<G4endl;
return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VisAttributes* CopyNoReader::GetVisAttribute(const G4String& ref)
{
  G4VisAttributes* col = 0;
  std::map<G4String, G4VisAttributes*>::iterator pos = fAttribs.find(ref);

  if (pos != fAttribs.end())
  {
    col = pos->second;
  }
  else
  {
    G4String err_mess = "Attribute: " + ref + " NOT found !";
    G4Exception("CopyNoReader::GetVisAttribute()",
                "ReadError", FatalException, err_mess);
  }
  return col;
}
