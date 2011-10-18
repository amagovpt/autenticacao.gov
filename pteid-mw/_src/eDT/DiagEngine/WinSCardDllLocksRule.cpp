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

#include <algorithm>
#include "repository.h"
#include "util.h"
#include "WinSCardDllLocksRule.h"

WinSCardDllLocksRule::WinSCardDllLocksRule()
:MetaRule(L"winscard.dll.lockers",L"Are Unexpected Processes Locking winscard.dll?"),
	m_key(L"module_info.winscard_dll.lockedby"),
	m_expected()

// list of applications expected to open winscard.dll
{
	m_expected.insert	(L"winlogon.exe");
	m_expected.insert	(L"explorer.exe");
	m_expected.insert	(L"svchost.exe");
	m_expected.insert	(L"firefox.exe");
	m_expected.insert	(L"beid35gui.exe");
	m_expected.insert	(L"thunderbird.exe");
}

WinSCardDllLocksRule::~WinSCardDllLocksRule() throw()
{
}

MetaRuleVerdict WinSCardDllLocksRule::verdict(Repository evidence) const
{
	// get all processes that use winscard.dll (from the contributed output of the module_info module)
	ContributionSet found=evidence.values(m_key);

	// subtract all expected applications from the ones we found
	ContributionSet unexpected;
	std::set_difference(found.begin(),found.end(),m_expected.begin(),m_expected.end(),std::inserter(unexpected,unexpected.end()));

	// if any are none, winscard.dll appears OK, and so this rule judges "not guilty"
	if(unexpected.empty())
	{
		return MetaRuleVerdict(false);
	}
	// if any are some, winscard.dll may be unduly locked and so this rule judges "guilty"
	else
	{
		std::wstring	guilties		(L"[");
						guilties.append	(join<std::wstring,ContributionSet>(unexpected,L","));
						guilties.append	(L"]");
		return MetaRuleVerdict(true,
								L"Card In Use",															//diagnostic
								L"The following processes are unexpectedly locking winscard.dll : " +	// details
								guilties,
								L"Find out if these are necessary and/or try stopping them",			// suggestion
								name());
	}
}