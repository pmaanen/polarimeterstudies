/*
 * DeuteronCarbonBreakupModel.hh
 *
 *  Created on: 13.01.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDIBREAKUPMODEL_HH_
#define INCLUDE_JEDIBREAKUPMODEL_HH_

#include <G4HadronicInteraction.hh>

class JediBreakupModel: public G4HadronicInteraction {
public:
	JediBreakupModel();
	virtual ~JediBreakupModel();

	G4HadFinalState * ApplyYourself(const G4HadProjectile & aTrack, G4Nucleus & targetNucleus);
};

#endif /* INCLUDE_JEDIBREAKUPMODEL_HH_ */
