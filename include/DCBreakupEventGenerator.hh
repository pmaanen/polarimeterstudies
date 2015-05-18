/*
 * DCBreakupEventGenerator.hh
 *
 *  Created on: 17.02.2015
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#define INCLUDE_DCBREAKUPEVENTGENERATOR_HH_
#include "PhaseSpaceGenerator.hh"

class G4ParticleGun;
class DCBreakupEventGenerator : public PhaseSpaceGenerator {
public:
	DCBreakupEventGenerator(G4ParticleGun* pgun=0);
	virtual ~DCBreakupEventGenerator();

private:
	virtual TF2* BuildFunction();
	virtual void Initialize();
	virtual PrimaryEvent Generate();
	virtual void Generate(G4Event* E);
	virtual void beginOfRun(){};
};

#endif /* INCLUDE_DCBREAKUPEVENTGENERATOR_HH_ */
