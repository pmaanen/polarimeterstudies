/*
 * JediFakeCrossSectionData.hh
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 *
 * This class provides a cross-section data store where all cross sections
 * for all nuclei are zero. This is necessary when process biasing via
 * G4VWrapperProcess is used to make sure the process just happens when the
 * randomized point in the target is reached. Just adding no cross section
 * data store causes geant to complain.
 */

#ifndef INCLUDE_JEDIFAKECROSSSECTIONDATA_HH_
#define INCLUDE_JEDIFAKECROSSSECTIONDATA_HH_


#include <G4VCrossSectionDataSet.hh>
#include <G4SystemOfUnits.hh>
class  G4DynamicParticle;
class  G4Element;
class  G4ParticleDefinition;

#include <limits>

class  JediFakeCrossSectionData : public G4VCrossSectionDataSet
{
public:
	JediFakeCrossSectionData();

	virtual G4bool IsElementApplicable( const G4DynamicParticle *, G4int, const G4Material*)
	{
		return false;
	}

	virtual G4double GetElementCrossSection( const G4DynamicParticle *, G4int, const G4Material*){
		return 0;
	}

	void      BuildPhysicsTable( const G4ParticleDefinition & )
	{}

	void      DumpPhysicsTable( const G4ParticleDefinition & )
	{}
};




JediFakeCrossSectionData::JediFakeCrossSectionData():G4VCrossSectionDataSet("fake"){}
#endif /* INCLUDE_JEDIFAKECROSSSECTIONDATA_HH_ */
