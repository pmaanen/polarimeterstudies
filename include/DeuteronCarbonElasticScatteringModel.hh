/*
 * DeuteronCarbonElasticScatteringModel.hh
 *
 *  Created on: 05.07.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_DEUTERONCARBONELASTICSCATTERINGMODEL_HH_
#define INCLUDE_DEUTERONCARBONELASTICSCATTERINGMODEL_HH_
#include "Rtypes.h"
#include "TLorentzVector.h" d
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

	virtual ~DeuteronCarbonElasticScatteringModel(){};
	Double_t sigma(TLorentzVector,TLorentzVector);
	Double_t SigmaUnpol(Double_t, Double_t);
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
DeuteronCarbonElasticScatteringModel::DeuteronCarbonElasticScatteringModel(Double_t beamEnergy,Double_t beamPolarization):fBeamEnergy(beamEnergy),fBeamPolarization(beamPolarization){}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Double_t DeuteronCarbonElasticScatteringModel::sigma(TLorentzVector in, TLorentzVector out) {
	auto q=sqrt(-(out-in).M2());
	auto phi=out.Vect().Phi();
	auto theta=out.Vect().Theta();
	return SigmaUnpol(fBeamEnergy,q)*(1+fBeamPolarization*Ay(fBeamEnergy,theta*180./3.1415)*cos(phi));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double DeuteronCarbonElasticScatteringModel::SigmaUnpol(Double_t E,Double_t q) {
	double w=log(E);
	return pow(10,a1(w)+a2(w)*q+(1+a5(w)*q)*(a3(w)*sin(a6(w)*q)+a4(w)*cos(a6(w)*q)));
}

double DeuteronCarbonElasticScatteringModel::a1(Double_t x) {
	return -79.173+46.741*x-8.662*x*x+0.52986*x*x*x;
}

double DeuteronCarbonElasticScatteringModel::a2(Double_t x) {
	return 78.051-34.263*x+3.341*x*x;

}

double DeuteronCarbonElasticScatteringModel::a3(Double_t x) {
	return -38.231+19.774*x-3.1487*x*x+0.14791*x*x*x;
}

double DeuteronCarbonElasticScatteringModel::a4(Double_t x) {
	return 9.1111-3.2832*x+0.28525*x*x;
}

double DeuteronCarbonElasticScatteringModel::a5(Double_t x) {
	return -3.6126+0.18644*x;
}

double DeuteronCarbonElasticScatteringModel::a6(Double_t x) {
	return 23.91+0.66962*x;
}

double DeuteronCarbonElasticScatteringModel::Ay(Double_t E, Double_t theta) {
	return (1/(1+900./pow(theta,4)))*
			(b4(E)-b1(E)/(1+exp((theta-b2(E))/b3(E)))+b5(E)*(1-theta/b6(E))*sin(b7(E)+b8(E)*theta));
}

double DeuteronCarbonElasticScatteringModel::b1(Double_t E) {
	return  1.1556+0.007182*(E-150)+1.3524e-5*pow(( E-150),2)-5.5448e-7*pow(( E-150),3);

}

double DeuteronCarbonElasticScatteringModel::b2(Double_t E) {
	return 16.029-0.24658*(E-150)+8.6972e-4*pow((E-150),2);

}

double DeuteronCarbonElasticScatteringModel::b3(Double_t E) {
	return 6.8319+0.052974*( E-150)+6.4864e-4*pow(E-150,2)-4.7648e-6*pow(E-150,3);

}

double DeuteronCarbonElasticScatteringModel::b4(Double_t E) {
	return 0.94964+8.2885e-4*( E-150)-5.4014e-6*pow(E-150,2);

}

double DeuteronCarbonElasticScatteringModel::b5(Double_t E) {
	return 0.21588+1.3908e-4*( E-150);

}

double DeuteronCarbonElasticScatteringModel::b6(Double_t E) {
	return 42.467-0.25468*( E-150)+0.0033973*pow(E-150,2);

}

double DeuteronCarbonElasticScatteringModel::b7(Double_t E) {
	return 58.573-0.41812*( E-150);

}

double DeuteronCarbonElasticScatteringModel::b8(Double_t E) {
	return 25.698+0.091205*( E-150)-1.8594e-4*pow(E-150,2);

}


#endif /* INCLUDE_DEUTERONCARBONELASTICSCATTERINGMODEL_HH_ */
