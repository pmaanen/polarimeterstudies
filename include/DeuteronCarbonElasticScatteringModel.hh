/*
 * DeuteronCarbonElasticScatteringModel.hh
 *
 *  Created on: 05.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DEUTERONCARBONELASTICSCATTERINGMODEL_HH_
#define INCLUDE_DEUTERONCARBONELASTICSCATTERINGMODEL_HH_
#include "Rtypes.h"
#include "TLorentzVector.h"
//This is the Function to build the TF2.
//x[0] is theta_cm, x[1] is phi par[0] is beam energy. par[1] is beam momentum in cms
//par[2] is polarization
//Units: E:MeV Theta:deg q:GeV/c
//Note: The root classes use GeV
//Sigma=SigmaUnpol*(1+Ay*Py*cos(phi))

class DeuteronCarbonElasticScatteringModel{
public:
	DeuteronCarbonElasticScatteringModel(Double_t,Double_t);
	DeuteronCarbonElasticScatteringModel() :
		fBeamEnergy(0), fBeamPolarization(0) {
	}

	virtual ~DeuteronCarbonElasticScatteringModel()=default;
	Double_t sigma(TLorentzVector,TLorentzVector);
	Double_t SigmaUnpol(Double_t, Double_t);
	Double_t q(Double_t*, Double_t*);
	Double_t Phi(Double_t*, Double_t*);
	void setBeamEnergy(Double_t beamEnergy) {
		fBeamEnergy = beamEnergy;
	}

	void setBeamPolarization(Double_t beamPolarization) {
		fBeamPolarization = beamPolarization;
	}

private:

	Double_t fBeamEnergy,fBeamPolarization;

	//Parameters for ds/dOmega. x is ln(E)
	double a1(Double_t x);
	double a2(Double_t x);
	double a3(Double_t x);
	double a4(Double_t x);
	double a5(Double_t x);
	double a6(Double_t x);

	//Parameters for Ay. Conversion from theta_cm to theta_lab is missing, but this does not affect us very much.
	double Ay(Double_t E, Double_t theta);
	double b1(Double_t E);
	double b2(Double_t E);
	double b3(Double_t E);
	double b4(Double_t E);
	double b5(Double_t E);
	double b6(Double_t E);
	double b7(Double_t E);
	double b8(Double_t E);
};
#endif /* INCLUDE_DEUTERONCARBONELASTICSCATTERINGMODEL_HH_ */
