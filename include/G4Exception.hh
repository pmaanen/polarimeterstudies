/*
 * G4Exception.hh
 *
 *  Created on: Oct 22, 2014
 *      Author: maanen
 */

#ifndef G4EXCEPTION_HH_
#define G4EXCEPTION_HH_
#include "G4ExceptionSeverity.hh"
#include <exception>
#include <string>
class myG4Exception: public std::exception {
public:
	myG4Exception(const char* originOfException,
				  const char* exceptionCode,
				  G4ExceptionSeverity severity,
				  const char* description
	);
	virtual ~myG4Exception() throw(){};

	virtual const char * what() const throw()
	        		{return fWhat.c_str();}


	std::string getOriginOfException() const {return this->fOriginOfException;};
	std::string getExceptionCode() const {return this->fExceptionCode;};
	std::string getDescription() const {return this->fDescription;};
	G4ExceptionSeverity getSeverity() const {return this->fSeverity;};

private:
	std::string fWhat;
	std::string fOriginOfException,fExceptionCode,fDescription;
	G4ExceptionSeverity fSeverity;
};

#endif /* G4EXCEPTION_HH_ */
