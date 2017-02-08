/*
 * DCInelasticEventGenerator.hh
 *
 *  Created on: 18.03.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DCINELASTICEVENTGENERATOR_HH_
#define INCLUDE_DCINELASTICEVENTGENERATOR_HH_

#include <PhaseSpaceGenerator.hh>
#include <memory>
#include "DeuteronCarbonInelasticScatteringModel.hh"
class TF1;
class TF2;
class DCInelasticEventGenerator: public PhaseSpaceGenerator {
public:
	DCInelasticEventGenerator();
	virtual ~DCInelasticEventGenerator()=default;
	virtual void Initialize();
	virtual genevent_t Generate();
private:
	//Double_t fBeamPolarization;
	std::unique_ptr<DeuteronCarbonInelasticScatteringModel> fScatteringModel;
	std::unique_ptr<TF2> fQEx;
	std::unique_ptr<TF1> fPhi;
};
#endif /* INCLUDE_DCINELASTICEVENTGENERATOR_HH_ */
