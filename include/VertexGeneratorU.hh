// File:	VertexGeneratorU.hh
// Date:	09-Feb-2015 G.Macharashvili Dubna

#ifndef VertexGeneratorU_h
#define VertexGeneratorU_h 1

#include <iomanip> 
using namespace std ;

#include <G4ThreeVector.hh>

#include "VVertexGenerator.hh"

class VertexGeneratorU : public VVertexGenerator {

private:

	VertexGeneratorU() : VVertexGenerator() { }
	virtual	~VertexGeneratorU() { }

public:
	static VertexGeneratorU* GetInstance() {
		static G4ThreadLocalSingleton<VertexGeneratorU> theInstance;
		return theInstance.Instance();
	}
	friend class G4ThreadLocalSingleton<VertexGeneratorU>;
	virtual	G4ThreeVector	generateVertex() ;
	virtual	void		print() { cout << "VertexGeneratorU: uniform ..." << endl ; }

} ;

#endif

// e-o-f
