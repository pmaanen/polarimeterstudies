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
	        		{return _s.c_str();}


	std::string getOriginOfException() const {return this->_originOfException;};
	std::string getExceptionCode() const {return this->_exceptionCode;};
	std::string getDescription() const {return this->_description;};
	G4ExceptionSeverity getSeverity() const {return this->_severity;};

private:
	std::string _s;
	std::string _originOfException,_exceptionCode,_description;
	G4ExceptionSeverity _severity;
};

#endif /* G4EXCEPTION_HH_ */
