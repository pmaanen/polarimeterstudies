/*
 * DeuteronCarbonBreakupModel.h
 *
 *  Created on: 05.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DEUTERONCARBONBREAKUPMODEL_HH_
#define INCLUDE_DEUTERONCARBONBREAKUPMODEL_HH_
#include "RTypes.h"
class DeuteronCarbonBreakupModel {
public:
	DeuteronCarbonBreakupModel();
	virtual ~DeuteronCarbonBreakupModel(){};

	Double_t sigma(Double_t theta, Double_t phi, Double_t Ex);
	DeuteronCarbonBreakupModel(Double_t beamEnergy, Double_t beamPolarization):fBeamEnergy(beamEnergy),fBeamPolarization(beamPolarization){}

	Double_t SigmaUnpol(Double_t, Double_t, Double_t);

	void setBeamEnergy(Double_t beamEnergy) {
		fBeamEnergy = beamEnergy;
	}

	void setBeamPolarization(Double_t beamPolarization) {
		fBeamPolarization = beamPolarization;
	}

private:
	Double_t fBeamEnergy,fBeamPolarization;
	Double_t c1(Double_t theta, Double_t E);
	Double_t c2(Double_t theta, Double_t E);
	Double_t c3(Double_t theta);
	Double_t c4(Double_t theta);
	Double_t c5(Double_t theta);

	Double_t Ay(Double_t, Double_t, Double_t);
	Double_t d1(Double_t theta, Double_t E);
	Double_t d2(Double_t theta, Double_t E);

};

#include <math.h>
Double_t DeuteronCarbonBreakupModel::sigma(Double_t theta, Double_t phi, Double_t Ex) {
	/*
	for(int i=0;i<100;i++){
		G4cout<<0.8*i<<" "<<SigmaUnpol(113,18,0.8*i)<<G4endl;
	}
	 */
	return SigmaUnpol(fBeamEnergy,theta,Ex)*(1+fBeamPolarization*Ay(fBeamEnergy,theta,Ex)*cos(phi));
}

Double_t DeuteronCarbonBreakupModel::SigmaUnpol(Double_t E, Double_t theta,
		Double_t Ex) {
	if(Ex>(c2(theta,E)-c3(theta)*c4(theta)))
		return c1(theta,E)*exp(-pow(Ex-c2(theta,E),2)/(2*c3(theta)*c3(theta)));
	else
		return c1(theta,E)*exp(-c4(theta)*c4(theta)/2.)*exp(-(c5(theta)*(c2(theta,E)-c4(theta)*c3(theta)-Ex))/c3(theta));

}

Double_t DeuteronCarbonBreakupModel::c1(Double_t theta,Double_t E) {
	return sqrt(113./E)*exp(1.5586-0.07178*(theta-40.)+4.0733e-4*pow(theta-40.,2));
}

Double_t DeuteronCarbonBreakupModel::c2(Double_t theta, Double_t E) {
	return E/113*(62.214+0.30061*(theta-40)-0.004704*pow(theta-40.,2));
}

Double_t DeuteronCarbonBreakupModel::c3(Double_t theta) {
	return 14.384-0.048676*(theta-40);
}

Double_t DeuteronCarbonBreakupModel::c4(Double_t theta) {
	return 0.74342+0.0069414*(theta-40);
}

Double_t DeuteronCarbonBreakupModel::c5(Double_t theta) {
	return c3(theta)*(0.070367-3.3534e-4*(theta-40)+1.1845e-5*(theta-40)*(theta-40)+1.1436e-6*pow(theta-40,3));
}

Double_t DeuteronCarbonBreakupModel::Ay(Double_t E, Double_t theta,
		Double_t Ex) {
	return d1(theta,E)+d2(theta,E)*(Ex-E/2.);
}

Double_t DeuteronCarbonBreakupModel::d1(Double_t theta, Double_t E) {
	return -1280./E/E+0.528*theta/E;
}

Double_t DeuteronCarbonBreakupModel::d2(Double_t theta, Double_t E) {
	return -7.35/pow(E,3./2.)+6.2e-5*(theta-40)+11.7/pow(E,3)*pow(theta-40,2)-3.1e-7*pow(theta-40,3);
}


#endif /* INCLUDE_DEUTERONCARBONBREAKUPMODEL_HH_ */
