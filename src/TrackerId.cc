/*
 * TrackerId.cc
 *
 *  Created on: Aug 14, 2013
 *      Author: maanen
 */

#include "TrackerId.hh"

TrackerId::TrackerId():DetId(0) {
	// TODO Auto-generated constructor stub

}

TrackerId::TrackerId(int Subdet,int SuperLayer):DetId(DetId::Tracker,Subdet)
{
	id_|=(SuperLayer&slMask)<<superlayerStartBit;
}

TrackerId::TrackerId(DetId id){
	id_=id.rawId();
}

TrackerId::TrackerId(UInt_t rawid):DetId(rawid){
}

TrackerId::~TrackerId() {
}
