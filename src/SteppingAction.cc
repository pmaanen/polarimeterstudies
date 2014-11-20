/*
 * SteppingAction.cc
 *
 *  Created on: Oct 20, 2014
 *      Author: maanen
 */

#include <SteppingAction.hh>
#include "G4Step.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Track.hh"
#include "TH2F.h"
#include "TNtuple.h"
#include "Analysis.hh"
#include "SFEventGenerator.hh"
using namespace CLHEP;
SteppingAction* SteppingAction::singleton=0;


SteppingAction::SteppingAction() {
}

SteppingAction::~SteppingAction() {
}

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
	return;
}
