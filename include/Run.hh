/*
 * Run.hh
 *
 *  Created on: 21.11.2014
 *      Author: pmaanen
 */

#ifndef RUN_HH_
#define RUN_HH_

#include <G4Run.hh>

class Run: public G4Run {
public:
	Run();
	virtual void RecordEvent(const G4Event* evt);
	virtual void Merge(const G4Run*);
};

#endif /* RUN_HH_ */
