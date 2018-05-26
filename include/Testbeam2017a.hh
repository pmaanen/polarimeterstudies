/*
 * Testbeam2017a.hh
 *
 *  Created on: 29.09.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_TESTBEAM2017A_HH_
#define INCLUDE_TESTBEAM2017A_HH_

#include "Testbeam2016b.hh"

class Testbeam2017a: public Testbeam2016b {
public:
	Testbeam2017a();
	virtual ~Testbeam2017a()=default;

	virtual G4VPhysicalVolume* Construct();

protected:

	virtual void DefineCommands(){};

private:

	void BuildDetector();

};

#endif /* INCLUDE_TESTBEAM2017A_HH_ */
