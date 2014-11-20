/*
 * G4Exception.cc
 *
 *  Created on: Oct 22, 2014
 *      Author: maanen
 */

#include "G4Exception.hh"

myG4Exception::myG4Exception(const char* originOfException,
		const char* exceptionCode,G4ExceptionSeverity severity,
		const char* description):_s(""),_originOfException(originOfException),_exceptionCode(exceptionCode),_severity(severity){}
