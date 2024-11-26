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
#include "ByteArray.h"

#include <exception>
#include <iostream>
#include <string>

namespace eIDMW {

class EIDMW_CMN_API CMWException : public std::exception {
public:
	// CMWException(long lError);
	CMWException(long lError, const char *cpFile, long lLine);
	~CMWException() throw() {};
	virtual const char *what() throw();

	long GetError() const { return m_lError; };
	std::string GetFile() const { return m_sFile; };
	long GetLine() const { return m_lLine; };

protected:
	long m_lError;
	std::string m_sFile;
	long m_lLine;
};

class EIDMW_CMN_API CNotAuthenticatedException : public CMWException {
public:
	CNotAuthenticatedException(long lError, long lPinRef);

	long GetPinRef() const;

protected:
	long m_lPinRef;
};

class EIDMW_CMN_API CBatchSignFailedException : public CMWException {
public:
	CBatchSignFailedException(long lError, unsigned int failedSignatureIndex)
		: CMWException(lError, "", 0), m_failedSignatureIndex(failedSignatureIndex) {};

	unsigned int GetFailedSignatureIndex() const { return m_failedSignatureIndex; };

protected:
	unsigned int m_failedSignatureIndex;
};

#ifdef _WIN32
#define DIR_SEP '\\'

// Reduced form of the standard __FILE__ macro: remove all directories from the path
inline const char *basename_for_logging(const char *path) {
	return (strrchr(path, DIR_SEP) ? strrchr(path, DIR_SEP) + 1 : path);
}
#define CMWEXCEPTION(i) CMWException(i, basename_for_logging(__FILE__), __LINE__)
#else
#define CMWEXCEPTION(i) CMWException(i, __FILE__, __LINE__)
#endif

enum class EIDMW_CMN_API EIDMW_ReportType { Success, Error };

struct EIDMW_CMN_API EIDMW_Report {
	unsigned int error_code = 0;
	EIDMW_ReportType type = EIDMW_ReportType::Success;
};

struct EIDMW_CMN_API EIDMW_ActiveAuthenticationReport : public EIDMW_Report {
	CByteArray dg14;		   // Security Options file
	CByteArray storedHashDg14; // Security Options file hash from SOD
	CByteArray hashDg14;	   // Hash of current Security Options file

	CByteArray dg15;		   // DG15 (Public Key)
	CByteArray storedHashDg15; // DG15 file hash from SOD
	CByteArray hashDg15;	   // Hash of current DG15

	std::string oid; // Active Authentication algorithm OID
};

struct EIDMW_CMN_API EIDMW_ChipAuthenticationReport : public EIDMW_Report {
	CByteArray pubKey; // Public Key for Chip Authentication
	std::string oid;   // Chip Authentication algorithm OID
};

class EIDMW_CMN_API EIDMW_PipelineReport {
public:
	void setActiveAuthenticationReport(const EIDMW_ActiveAuthenticationReport &report) {
		if (report.type == EIDMW_ReportType::Error) {
			m_hasFailed = true;
		}

		m_activeAuthenticationReport = report;
	}

	const EIDMW_ActiveAuthenticationReport &getActiveAuthenticationReport() const {
		return m_activeAuthenticationReport;
	}

	void setChipAuthenticationReport(const EIDMW_ChipAuthenticationReport &report) {
		if (report.type == EIDMW_ReportType::Error) {
			m_hasFailed = true;
		}

		m_chipAuthenticationReport = report;
	}

	const EIDMW_ChipAuthenticationReport &getChipAuthenticationReport() const { return m_chipAuthenticationReport; }

	bool HasFailed() { return m_hasFailed; }

private:
	bool m_hasFailed = false;
	EIDMW_ActiveAuthenticationReport m_activeAuthenticationReport;
	EIDMW_ChipAuthenticationReport m_chipAuthenticationReport;
};

} // namespace eIDMW
#endif