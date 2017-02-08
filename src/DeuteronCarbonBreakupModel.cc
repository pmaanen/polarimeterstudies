/*
 * DeuteronCarbonBreakupModel.cc
 *
 *  Created on: 02.12.2016
 *      Author: pmaanen
 */
#include "DeuteronCarbonBreakupModel.hh"
#include "TMath.h"
#include <math.h>
Double_t JediBreakupModelOld::SigmaUnpol(Double_t E, Double_t theta,
		Double_t Ex) {
	if(Ex>(c2(theta,E)-c3(theta)*c4(theta)))
		return c1(theta,E)*exp(-pow(Ex-c2(theta,E),2)/(2*c3(theta)*c3(theta)));
	else
		return c1(theta,E)*exp(-c4(theta)*c4(theta)/2.)*exp(-(c5(theta)*(c2(theta,E)-c4(theta)*c3(theta)-Ex))/c3(theta));
}

Double_t JediBreakupModelOld::ThetaEx(Double_t* x, Double_t* par) {
	/*
	 * x[0]=theta
	 * x[1]=Ex
	 * par[0]=E
	 *
	 * par[1,2]=Theta Min,Max
	 * par[3,4]=Ex Min,Max
	 *
	 *
	 */
	if (x[0]<par[1] or x[0]>par[2] or x[1]<par[3] or x[1]>par[4])
		return 0;
	if(x[1]>(c2(x[0],par[0])-c3(x[0])*c4(x[0])))
		return c1(x[0],par[0])*exp(-pow(x[1]-c2(x[0],par[0]),2)/(2*c3(x[0])*c3(x[0])));
	else
		return c1(x[0],par[0])*exp(-c4(x[0])*c4(x[0])/2.)*exp(-(c5(x[0])*(c2(x[0],par[0])-c4(x[0])*c3(x[0])-x[1]))/c3(x[0]));
}

double JediBreakupModelOld::Phi(Double_t* x, Double_t* par) {
	/*
	 * x[0]=Phi
	 * par[0]=Py
	 * par[1]=E
	 * par[2]=theta
	 * par[3]=Ex
	 */
	return 1+par[0]*Ay(par[1],par[2]*TMath::RadToDeg(),par[3])*cos(x[0]);
}

Double_t JediBreakupModelOld::c1(Double_t theta,Double_t E) {
	return sqrt(113./E)*exp(1.5586-0.07178*(theta-40.)+4.0733e-4*pow(theta-40.,2));
}

Double_t JediBreakupModelOld::c2(Double_t theta, Double_t E) {
	return E/113*(62.214+0.30061*(theta-40)-0.004704*pow(theta-40.,2));
}

Double_t JediBreakupModelOld::c3(Double_t theta) {
	return 14.384-0.048676*(theta-40);
}

Double_t JediBreakupModelOld::c4(Double_t theta) {
	return 0.74342+0.0069414*(theta-40);
}

Double_t JediBreakupModelOld::c5(Double_t theta) {
	return c3(theta)*(0.070367-3.3534e-4*(theta-40)+1.1845e-5*(theta-40)*(theta-40)+1.1436e-6*pow(theta-40,3));
}

Double_t JediBreakupModelOld::Ay(Double_t E, Double_t theta,
		Double_t Ex) {
	return d1(theta,E)+d2(theta,E)*(Ex-E/2.);
}

Double_t JediBreakupModelOld::d1(Double_t theta, Double_t E) {
	return -1280./E/E+0.528*theta/E;
}

Double_t JediBreakupModelOld::d2(Double_t theta, Double_t E) {
	return -7.35/pow(E,3./2.)+6.2e-5*(theta-40)+11.7/pow(E,3)*pow(theta-40,2)-3.1e-7*pow(theta-40,3);
}



