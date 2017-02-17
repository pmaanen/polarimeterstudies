/*
 * JediTrackInfo.hh
 *
 *  Created on: 17.01.2017
 *      Author: pmaanen
 */

#ifndef INCLUDE_JEDITRACKINFO_HH_
#define INCLUDE_JEDITRACKINFO_HH_

#include <G4VUserTrackInformation.hh>

class JediTrackInfo: public G4VUserTrackInformation {
public:
	explicit JediTrackInfo();
	virtual ~JediTrackInfo()=default;



	G4double  GetCurrentTrackLengthInTarget( void ) const;

	void      AddTrackLengthInTarget( G4double  value );

	void      SetNeedsTrackLengthResampling( G4bool  on = true );

	G4double  GetFinalTrackLengthInTarget( void ) const;

	void      SetFinalTrackLengthInTarget( G4double  value );

	void      ResetCurrentTrackLengthInTarget( void );

	G4bool    NeedsTrackLengthResampling( void ) const;

	G4bool    IsStudiedProcessActivated( void ) const;

	G4bool  IsStudiedProcessAllowed( void ) const;
	void      ActivateStudiedProcess( G4bool  on = true );

	void      AllowStudiedProcess( G4bool  on = true );


private:

	G4int           GetCopyNumber( void ) const;

private:

	G4int           copyNumber;

	G4double  currentTrackLengthInTarget;

	G4double  finalTrackLengthInTarget;

	G4bool    isStudiedProcessActivated;

	G4bool isStudiedProcessAllowed;

	G4bool    needsTrackLengthResampling;
};



inline G4int  JediTrackInfo::GetCopyNumber( void ) const
{
	return copyNumber;
}


inline G4double  JediTrackInfo::GetCurrentTrackLengthInTarget(
		void ) const
{
	return currentTrackLengthInTarget;
}


inline void  JediTrackInfo::AddTrackLengthInTarget(
		G4double  value )
{
	currentTrackLengthInTarget += value;
}


inline void  JediTrackInfo::SetNeedsTrackLengthResampling(
		G4bool  on )
{
	needsTrackLengthResampling = on;
}


inline G4double  JediTrackInfo::GetFinalTrackLengthInTarget(
		void ) const
{
	return finalTrackLengthInTarget;
}


inline void  JediTrackInfo::SetFinalTrackLengthInTarget(
		G4double  value )
{
	finalTrackLengthInTarget = value;
}


inline void  JediTrackInfo::ResetCurrentTrackLengthInTarget(
		void )
{
	currentTrackLengthInTarget = 0.;
}


inline G4bool  JediTrackInfo::NeedsTrackLengthResampling(
		void ) const
{
	return needsTrackLengthResampling;
}


inline G4bool  JediTrackInfo::IsStudiedProcessActivated(
		void ) const
{
	return isStudiedProcessActivated;
}


inline void  JediTrackInfo::ActivateStudiedProcess(
		G4bool  on )
{
	isStudiedProcessActivated = on;
}

inline void  JediTrackInfo::AllowStudiedProcess(
		G4bool  on )
{
	isStudiedProcessAllowed = on;
}

inline G4bool  JediTrackInfo::IsStudiedProcessAllowed(
		void ) const
{
	return isStudiedProcessAllowed;
}
#endif /* INCLUDE_JEDITRACKINFO_HH_ */

