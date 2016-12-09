/*
 * DeuteronCarbonInelasticScatteringModel.cc
 *
 *  Created on: 05.12.2016
 *      Author: pmaanen
 */

#include <DeuteronCarbonInelasticScatteringModel.hh>
#include <math.h>
Double_t DeuteronCarbonInelasticScatteringModel::QEx(Double_t* x,
		Double_t* par) {
	/*
	 * x[0]=Ex
	 * x[1]=q
	 */
	return pow(10,f1(x[0])+f2(x[0])*(x[1]-0.25)+f3(x[0])*pow(x[1]-0.25,2));
}

Double_t DeuteronCarbonInelasticScatteringModel::Phi(Double_t* x,
		Double_t* par) {
	/*
	 * x[0]=Phi
	 *par[0]=Py
	 *par[1]=q
	 *par[2]=Ex
	 */
	return 1+par[0]*Ay(par[1],par[2])*cos(x[0]);
}

Double_t DeuteronCarbonInelasticScatteringModel::f1(Double_t Ex) {
	return 0.81403*8.5131e-4*Ex;
}

Double_t DeuteronCarbonInelasticScatteringModel::f2(Double_t Ex) {
	return -3.1246-0.068376*Ex+0.0014052*Ex*Ex;
}

Double_t DeuteronCarbonInelasticScatteringModel::f3(Double_t Ex) {
	return -4.8934+0.028654*Ex;
}

Double_t DeuteronCarbonInelasticScatteringModel::Ay(Double_t q,
		Double_t Ex) {
	return g1(Ex)*(1-1/pow(1+exp((q-g2(Ex))/g3(Ex)),0.1));
}

Double_t DeuteronCarbonInelasticScatteringModel::g1(Double_t Ex) {
	return 0.82579-0.0188*Ex+1.3928e-4*Ex*Ex;
}

Double_t DeuteronCarbonInelasticScatteringModel::g2(Double_t Ex) {
	return 0.10278+0.0043333*Ex;
}

Double_t DeuteronCarbonInelasticScatteringModel::g3(Double_t Ex) {
	return (16.933+0.24*Ex)/1000;
}
