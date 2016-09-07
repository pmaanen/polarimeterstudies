
/*

 * SensDet.hh
 *
 *  Created on: 29.04.2016
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDISENSITIVEDETECTOR_HH_1
#define INCLUDE_JEDISENSITIVEDETECTOR_HH_1

#include <G4VSensitiveDetector.hh>
#include <G4Cache.hh>
#include <G4LogicalVolume.hh>
#include <G4GenericMessenger.hh>
#include <vector>

typedef std::vector<G4LogicalVolume*> logVolVector;
enum class SDtype {kTracker, kCalorimeter, kPerfect, kUndefined};
class JediSensitiveDetector;
class SD{
public:
	SDtype fType;
	std::vector<G4LogicalVolume*> fLogVol;
	G4Cache<JediSensitiveDetector*> fSD;
};

class SensitiveDetectorMap{
public:
	void Update(G4String name,SDtype type,logVolVector vector);
	void Update(G4String name,SDtype type);
	const std::map<G4String, SD > getMap(){return fSDmap;};

private:
	std::map<G4String, SD> fSDmap;
};
#endif /* INCLUDE_JEDISENSITIVEDETECTOR_HH_1 */
