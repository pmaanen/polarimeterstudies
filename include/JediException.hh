/*
 * JediException.hh
 *
 *  Created on: Oct 22, 2014
 *      Author: maanen
 */

#ifndef JediEXCEPTION_HH_
#define JediEXCEPTION_HH_
#include "G4ExceptionSeverity.hh"
#include <exception>
#include <string>
class JediException: public std::exception {
public:
	JediException(const char* originOfException,
				  const char* exceptionCode,
				  G4ExceptionSeverity severity,
				  const char* description
	);
	virtual ~JediException() throw(){};

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
