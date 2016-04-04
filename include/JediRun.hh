/*
 * JediRun.hh
 *
 *  Created on: Mar 6, 2016
 *      Author: maanen
 */

#ifndef JEDIRUN_HH_
#define JEDIRUN_HH_

#include <G4Run.hh>
#include "hit.hh"

#include <map>
#include <vector>
class JediRun: public G4Run {
public:
	JediRun();
	virtual ~JediRun();

	virtual void RecordEvent(const G4Event*);
	void Merge(const G4Run*);

	const std::vector<simevent_t>& getEvents() const {
		return fEvents;
	}

	const std::vector<genevent_t>& getGenEvents() const {
		return fGenEvents;
	}

	std::vector<simevent_t> fEvents;
	std::vector<genevent_t> fGenEvents;
};

#endif /* JEDIRUN_HH_ */
