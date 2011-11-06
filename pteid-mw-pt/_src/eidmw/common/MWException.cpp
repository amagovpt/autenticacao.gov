/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#include "MWException.h"
#include "Util.h"

using namespace std;

namespace eIDMW
{

//CMWException::CMWException(long lError)
//: m_lError(lError),
//  m_sFile(""),
//  m_sLine(O)
//{
//}

// CMWEXCEPTION::CMWEXCEPTION(long lError, const char *cpFile, long lLine)
CMWException::CMWException(long lError, const char *cpFile, long lLine)
: m_lError(lError),
  m_sFile(cpFile),
  m_lLine(lLine)

{
}

const char* CMWException::what() throw()
{
	return "CMWException, error code strings to be implemented";
}

// CNotAuthenticatedException::CNotAuthenticatedException(...)
CNotAuthenticatedException::CNotAuthenticatedException(
	long lError, long lPinRef)
: CMWException(lError,"", 0), m_lPinRef(lPinRef)
{
}

// long CNotAuthenticatedException::GetPinRef()
long CNotAuthenticatedException::GetPinRef() const
{
	return m_lPinRef;
}

}
