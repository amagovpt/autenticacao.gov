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
#pragma once

#include "FileSystemCard.h"

#define MAX_EMU_KEYS  10

namespace eIDMW
{

#define ALGO_RSA_PKCS1_MD5         1
#define ALGO_RSA_PKCS1_SHA1        2
#define ALGO_RSA_PKCS1_SHA256      4
#define ALGO_RSA_PKCS1             8
#define ALGO_RSA_PSS_SHA1          16

typedef struct {
	CByteArray oRsaExp;
	CByteArray oRsaMod;
	std::string csPath;
	unsigned long ulRef;
	unsigned long ulPinRef;
	bool bUserConsent;
	unsigned long ulAlgos;
}
tCardKey;

class CEmuationPkiCard : public CFileSystemCard
{
public:
	CEmuationPkiCard();

	virtual void PutRsaKey(const CByteArray & oPrivExp, const CByteArray & oMod,
		const std::string &csPath, unsigned long ulRef,
		unsigned long ulPinRef, bool bUserConsent,
		unsigned long ulAlgos);

	virtual CByteArray Sign(const CByteArray & oData,
		const tCardKey *poKey, unsigned long ulAlgo);

protected:
	tCardKey m_CardKeys[MAX_EMU_KEYS];
	int m_iKeyCount;

	virtual tCardKey * FindKey(unsigned long ulKeyRef);
};

}
