/*
 * DeuteronCarbonBreakupModel.h
 *
 *  Created on: 05.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DEUTERONCARBONBREAKUPMODEL_HH_
#define INCLUDE_DEUTERONCARBONBREAKUPMODEL_HH_
#include "Rtypes.h"
class JediBreakupModelOld {
public:
	JediBreakupModelOld()=default;
	virtual ~JediBreakupModelOld()=default;

	Double_t sigma(Double_t theta, Double_t phi, Double_t Ex);
	Double_t SigmaUnpol(Double_t, Double_t, Double_t);
	Double_t ThetaEx(Double_t* x, Double_t* par);
	Double_t Phi(Double_t*, Double_t*);

private:
	Double_t c1(Double_t theta, Double_t E);
	Double_t c2(Double_t theta, Double_t E);
	Double_t c3(Double_t theta);
	Double_t c4(Double_t theta);
	Double_t c5(Double_t theta);

	Double_t Ay(Double_t, Double_t, Double_t);
	Double_t d1(Double_t theta, Double_t E);
	Double_t d2(Double_t theta, Double_t E);

};
#endif /* INCLUDE_DEUTERONCARBONBREAKUPMODEL_HH_ */
