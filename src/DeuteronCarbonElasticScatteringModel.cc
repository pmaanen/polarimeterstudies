/*
 * DeuteronCarbonElasticScatteringModel.cc
 *
 *  Created on: 02.12.2016
 *      Author: pmaanen
 */


#include "DeuteronCarbonElasticScatteringModel.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
double DeuteronCarbonElasticScatteringModel::SigmaUnpol(Double_t E,Double_t q) {
	double w=log(E);
	return pow(10,a1(w)+a2(w)*q+(1+a5(w)*q)*(a3(w)*sin(a6(w)*q)+a4(w)*cos(a6(w)*q)));
}
*/

double JediElasticModel::q(Double_t* x, Double_t* par) {
	double w=log(par[0]);
	return pow(10,a1(w)+a2(w)*x[0]+(1+a5(w)*x[0])*(a3(w)*sin(a6(w)*x[0])+a4(w)*cos(a6(w)*x[0])));
}
double JediElasticModel::Phi(Double_t* x, Double_t* par) {
	return 1+par[1]*Ay(par[0],par[2]*180./TMath::Pi())*cos(x[0]);
}

double JediElasticModel::a1(Double_t x) {
	return -79.173+46.741*x-8.662*x*x+0.52986*x*x*x;
}

double JediElasticModel::a2(Double_t x) {
	return 78.051-34.263*x+3.341*x*x;

}

double JediElasticModel::a3(Double_t x) {
	return -38.231+19.774*x-3.1487*x*x+0.14791*x*x*x;
}

double JediElasticModel::a4(Double_t x) {
	return 9.1111-3.2832*x+0.28525*x*x;
}

double JediElasticModel::a5(Double_t x) {
	return -3.6126+0.18644*x;
}

double JediElasticModel::a6(Double_t x) {
	return 23.91+0.66962*x;
}

double JediElasticModel::Ay(Double_t E, Double_t theta) {
	return (1/(1+900./pow(theta,4)))*
			(b4(E)-b1(E)/(1+exp((theta-b2(E))/b3(E)))+b5(E)*(1-theta/b6(E))*sin(b7(E)+b8(E)*theta));
}

double JediElasticModel::b1(Double_t E) {
	return  1.1556+0.007182*(E-150)+1.3524e-5*pow(( E-150),2)-5.5448e-7*pow(( E-150),3);

}

double JediElasticModel::b2(Double_t E) {
	return 16.029-0.24658*(E-150)+8.6972e-4*pow((E-150),2);

}

double JediElasticModel::b3(Double_t E) {
	return 6.8319+0.052974*( E-150)+6.4864e-4*pow(E-150,2)-4.7648e-6*pow(E-150,3);

}

double JediElasticModel::b4(Double_t E) {
	return 0.94964+8.2885e-4*( E-150)-5.4014e-6*pow(E-150,2);

}

double JediElasticModel::b5(Double_t E) {
	return 0.21588+1.3908e-4*( E-150);

}

double JediElasticModel::b6(Double_t E) {
	return 42.467-0.25468*( E-150)+0.0033973*pow(E-150,2);

}

double JediElasticModel::b7(Double_t E) {
	return 58.573-0.41812*( E-150);

}

double JediElasticModel::b8(Double_t E) {
	return 25.698+0.091205*( E-150)-1.8594e-4*pow(E-150,2);

}




