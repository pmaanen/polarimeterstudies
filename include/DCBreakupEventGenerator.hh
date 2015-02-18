/*
 * DCBreakupEventGenerator.hh
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#define INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"
class DCBreakupEventGenerator : public PhaseSpaceGenerator {
public:
	DCBreakupEventGenerator();
	virtual ~DCBreakupEventGenerator();

private:
	virtual TF2* BuildFunction();
	virtual void Initialize();
	virtual ParticleMomentumVector GenerateEvent();
};

#endif /* INCLUDE_DCBREAKUPEVENTGENERATOR_HH_ */
