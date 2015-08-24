// File:	VertexGeneratorU.cc
// Date:	09-Feb-2015 G.Macharashvili Dubna

#include "VertexGeneratorU.hh"
#include "Randomize.hh"

G4ThreeVector	VertexGeneratorU::generateVertex() {
	G4double xx,yy,zz;
	xx=x;
	yy=y;
	zz=z;
	if (sx > 0.0) xx += sx*(G4UniformRand()-0.5) ;
	if (sy > 0.0) yy += sy*(G4UniformRand()-0.5) ;
	if (sz > 0.0) zz += sz*(G4UniformRand()-0.5) ;
	v.set(xx,yy,zz) ;
	return v ;
	}
	
// e-o-f
