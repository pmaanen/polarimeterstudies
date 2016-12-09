/*
 * DeuteronCarbonInelasticScatteringModel.hh
 *
 *  Created on: 05.12.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DEUTERONCARBONINELASTICSCATTERINGMODEL_HH_
#define INCLUDE_DEUTERONCARBONINELASTICSCATTERINGMODEL_HH_
#include "Rtypes.h"
class DeuteronCarbonInelasticScatteringModel {
public:
	DeuteronCarbonInelasticScatteringModel()=default;
	virtual ~DeuteronCarbonInelasticScatteringModel()=default;

	Double_t QEx(Double_t* x, Double_t* par);
	Double_t Phi(Double_t* x, Double_t* par);

private:

	Double_t f1(Double_t Ex);
	Double_t f2(Double_t Ex);
	Double_t f3(Double_t Ex);

	Double_t Ay(Double_t q, Double_t Ex);

	Double_t g1(Double_t Ex);
	Double_t g2(Double_t Ex);
	Double_t g3(Double_t Ex);
};

#endif /* INCLUDE_DEUTERONCARBONINELASTICSCATTERINGMODEL_HH_ */
