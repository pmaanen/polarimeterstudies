/*
 * TrackerDetId.hh
 *
 *  Created on: Aug 14, 2013
 *      Author: maanen
 */

#ifndef TRACKERDETID_HH_
#define TRACKERDETID_HH_

#include "DetId.hh"

class TrackerId: public DetId {
public:

	//Standard Constructor, sets detector and subdetector fields and zeroes all other parts
	TrackerId();

	//Constructor from packed id
	TrackerId(UInt_t id);
	TrackerId(DetId id);

	//Constructor from Index
	TrackerId(int SubDet,int SuperLayer);
	virtual ~TrackerId();

	int superlayer() const {
		return int((id_>>superlayerStartBit)&slMask);
	}


protected:

	static const int superlayerNumBits=2;
	static const int superlayerStartBit=23;

	static const UInt_t slMask=0x3;
};
class TrackerSubDetId{
public:
	static const int Barrel=1;
	static const int Endcap=2;
};
#endif /* TRACKERDETID_HH_ */
