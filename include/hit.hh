/*
 * hit.hh
 *
 *  Created on: Jul 24, 2013
 *      Author: maanen
 */
#include "Rtypes.h"
#ifndef HIT_HH_
#define HIT_HH_
typedef struct {
	Double_t pos[3];
	Double_t truepos[3];
	Double_t edep;
	Double_t pid;
	Double_t trid;
	Double_t tof;
} trackerhit_t;

typedef struct {
	Double_t edep;
	Int_t crystal;
} calorhit_t;

typedef struct {
	Double_t x;
	Double_t y;
	Double_t z;
} vertex_t;

typedef struct {
	Double_t Pm_d;
	Double_t Pm_r;
	Double_t th_d;
	Double_t th_r;
	Double_t phi_d;
	Double_t phi_r;
	Double_t vx;
	Double_t vy;
	Double_t vz;
	Double_t xp;
	Double_t yp;
} gen_t;

#endif /* HIT_HH_ */
