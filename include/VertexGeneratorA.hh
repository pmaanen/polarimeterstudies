// File:	VertexGeneratorA.hh
// Date:	01-Feb-2015 G.Macharashvili Dubna

#ifndef VertexGeneratorA_hh
#define VertexGeneratorA_hh 1

#include <iomanip> 
using namespace std ;

#include <G4ThreeVector.hh>

#include "VVertexGenerator.hh"

class VertexGeneratorA : public VVertexGenerator {
private:

	VertexGeneratorA() : VVertexGenerator(),qx(0),qy(0) {}
	virtual	~VertexGeneratorA() {}

public:
	static VertexGeneratorA* GetInstance() {
		static G4ThreadLocalSingleton<VertexGeneratorA> theInstance;
		return theInstance.Instance();
	}
	friend class G4ThreadLocalSingleton<VertexGeneratorA>;

	virtual	G4ThreeVector	generateVertex() ;
	virtual	void		print() { cout << "VertexGeneratorA: parabolic profile ..." << endl ; }

protected:

	G4double 	qx , qy ;

} ;

#endif

// e-o-f
