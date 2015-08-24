// File:	VertexGeneratorO.cc
// Date:	23-Jan-2015 G.Macharashvili Dubna

#include "VertexGeneratorO.hh"
#include <Randomize.hh>

G4ThreeVector	VertexGeneratorO::generateVertex() {

	G4double xx,yy,zz;
	xx=x;
	yy=y;
	zz=z;
	if (sx > 0.0) xx += G4RandGauss::shoot(0,sx) ;
	if (sy > 0.0) yy += G4RandGauss::shoot(0,sy) ;
	if (sz > 0.0) zz += G4RandGauss::shoot(0,sz) ;
	v.set(xx,yy,zz) ;
	return v ;
}

// e-o-f
