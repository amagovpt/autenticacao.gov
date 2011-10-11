/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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

#ifndef __METARULEVERDICT__
#define __METARULEVERDICT__

#include <string>

class MetaRuleVerdict
{
public:
							MetaRuleVerdict(bool guilty, std::wstring verdict=L"none", std::wstring details=L"none", std::wstring corrective=L"none", std::wstring rulename=L"none");
	virtual				   ~MetaRuleVerdict() throw();
			bool			guilty		(void) const;
			std::wstring	verdict		(void) const;
			std::wstring	details		(void) const;
			std::wstring	corrective	(void) const;
			std::wstring	rulename	(void) const;
private:
	bool			m_guilty;
	std::wstring	m_verdict;
	std::wstring	m_details;
	std::wstring	m_corrective;
	std::wstring	m_rulename;
};

#endif