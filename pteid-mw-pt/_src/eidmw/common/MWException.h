/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#pragma once
#ifndef MWEXCEPTION_H
#define MWEXCEPTION_H

#include "Export.h"

#include <exception>
#include <iostream>
#include <string>

using namespace std;

namespace eIDMW
{

class EIDMW_CMN_API CMWException: public std::exception
{
public:
    //CMWException(long lError);
    CMWException(long lError, const char *cpFile, long lLine);
    ~CMWException () throw(){};
    virtual const char* what() throw();

    long GetError() const {return m_lError;};
    std::string GetFile() const {return m_sFile;};
    long GetLine()const {return m_lLine;};

protected:
    long m_lError;
    std::string m_sFile;
    long m_lLine;
};

class EIDMW_CMN_API CNotAuthenticatedException: public CMWException
{
public:
	CNotAuthenticatedException(
		long lError, long lPinRef);

	long GetPinRef() const;

protected:
	long m_lPinRef;
};

class EIDMW_CMN_API CBatchSignFailedException: public CMWException
{
public:
    CBatchSignFailedException(long lError, unsigned int failedSignatureIndex) :
        CMWException(lError, "", 0),
        m_failedSignatureIndex(failedSignatureIndex){};

    unsigned int GetFailedSignatureIndex() const { return m_failedSignatureIndex; };

protected:
    unsigned int m_failedSignatureIndex;
};



#ifdef _WIN32
#define DIR_SEP '\\'

//Reduced form of the standard __FILE__ macro: remove all directories from the path
inline const char *basename_for_logging(const char * path) {
	return (strrchr(path, DIR_SEP) ? strrchr(path, DIR_SEP) + 1 : path);
}
#define CMWEXCEPTION(i)	CMWException(i, basename_for_logging(__FILE__), __LINE__)
#else
#define CMWEXCEPTION(i)	CMWException(i, __FILE__, __LINE__)
#endif


}
#endif
