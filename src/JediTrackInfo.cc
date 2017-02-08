/*
 * JediTrackInfo.cc
 *
 *  Created on: 17.01.2017
 *      Author: pmaanen
 */

#include <JediTrackInfo.hh>

JediTrackInfo::JediTrackInfo():copyNumber( -1 ),currentTrackLengthInTarget( 0. ),
finalTrackLengthInTarget( 0. ), isStudiedProcessActivated( false ), isStudiedProcessAllowed(true),
needsTrackLengthResampling( false ) {

}
