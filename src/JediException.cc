/*
 * G4Exception.cc
 *
 *  Created on: Oct 22, 2014
 *      Author: maanen
 */

#include <JediException.hh>

JediException::JediException(const char* originOfException,
		const char* exceptionCode,G4ExceptionSeverity severity,
		const char* description):fWhat(""),fOriginOfException(originOfException),fExceptionCode(exceptionCode),fDescription(description),fSeverity(severity){}
