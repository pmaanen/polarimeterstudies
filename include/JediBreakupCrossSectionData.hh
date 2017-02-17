/*
 * JediBreakupCrossSectionData.hh
 *
 *  Created on: 18.01.2017
 *      Author: pmaanen
 */

#ifndef SRC_JEDIBREAKUPCROSSSECTIONDATA_HH_
#define SRC_JEDIBREAKUPCROSSSECTIONDATA_HH_

#include <G4VCrossSectionDataSet.hh>
#include <G4SystemOfUnits.hh>
#include <G4Deuteron.hh>
class  G4DynamicParticle;
class  G4Element;
class  G4ParticleDefinition;
#include <G4DynamicParticle.hh>
class JediBreakupCrossSectionData: public G4VCrossSectionDataSet {
public:
	JediBreakupCrossSectionData(G4double factor):G4VCrossSectionDataSet("breakup"),fFactor(factor){};
	virtual ~JediBreakupCrossSectionData()=default;

	void      BuildPhysicsTable( const G4ParticleDefinition & )
	{}

	void      DumpPhysicsTable( const G4ParticleDefinition & )
	{}

	virtual G4bool IsElementApplicable( const G4DynamicParticle *, G4int, const G4Material*)
	{
		return true;
	}

	virtual G4double GetElementCrossSection( const G4DynamicParticle * particle, G4int Z, const G4Material*){
		G4double xsec=0;
		if(*particle->GetParticleDefinition()==*G4Deuteron::DeuteronDefinition() and Z==6)
			xsec=fFactor*250*CLHEP::millibarn;
		return xsec;
	}

private:
	G4double fFactor;
};

#endif /* SRC_JEDIBREAKUPCROSSSECTIONDATA_HH_ */
