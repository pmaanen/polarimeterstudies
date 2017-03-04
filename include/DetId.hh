/*
 * DetId.hh
 *
 *  Created on: Jun 14, 2013
 *      Author: maanen
 */

#ifndef DETID_HH_
#define DETID_HH_

#include <ostream>
//#include <stdint.h>
#include "TH1F.h"
/** \class DetId

Parent class for all detector ids  The DetId is a 32-bit
unsigned integer.  The four most significant bits ([31:28]) identify
the large-scale detector e.g. tracker, cal while the next four
bits ([27:24]) identify a part of the detector (such as inner tracker/outer tracker)

 */
class DetId {
public:

	static const int kDetNbits=4;
	static const int kSubDetNbits=4;

	static const int kDetOffset          = 32-kDetNbits;
	static const int kSubdetOffset       = 32-kDetNbits-kSubDetNbits;

	static const int kDetMask=((uint32_t)1 << kDetNbits) - 1;
	static const int kSubDetMask=((uint32_t)1 << kSubDetNbits) - 1;
	enum Detector { kTracker=1,kDeltaE=2,kCalo=3};
	/// Create an empty or null id
	DetId()  : fId(0) { }
	/// Create an id from a raw number
	DetId(UInt_t id) : fId(id) { }
	/// Create an id, filling the detector and subdetector fields as specified
	DetId(Detector det, int subdet)  {
		fId=((det&kDetMask)<<kDetOffset)|((subdet&kSubDetMask)<<kSubdetOffset);
	}

	/// get the detector field from this detid
	Detector det() const { return Detector((fId>>kDetOffset)&kDetMask); }
	/// get the contents of the subdetector field (not cast into any detector's numbering enum)
	int subdetId() const { return ((fId>>kSubdetOffset)&kSubDetMask); }

	UInt_t operator()() const { return fId; }
	operator UInt_t() const { return fId; }

	/// get the raw id
	UInt_t rawId() const { return fId; }
	/// is this a null id ?
	bool null() const { return fId==0; }

	/// equality
	bool operator==(DetId id) const { return fId==id.fId; }
	/// inequality
	bool operator!=(DetId id) const { return fId!=id.fId; }
	/// comparison
	bool operator<(DetId id) const { return fId<id.fId; }

protected:
	UInt_t fId;
};

/// equality
inline bool operator==(UInt_t i, DetId id)  { return i==id(); }
inline bool operator==(DetId id, UInt_t i)  { return i==id(); }
/// inequality
inline bool operator!=(UInt_t i, DetId id)  { return i!=id(); }
inline bool operator!=(DetId id, UInt_t i) { return i!=id(); }
/// comparison
inline bool operator<(UInt_t i, DetId id) { return i<id(); }
inline bool operator<(DetId id, UInt_t i) { return id()<i; }

#endif /* DETID_HH_ */
