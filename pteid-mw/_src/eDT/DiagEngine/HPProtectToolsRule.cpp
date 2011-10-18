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
#include "HPProtectToolsRule.h"

HPProtectToolsRule::HPProtectToolsRule()
:MetaRule(L"hp.protecttools.present",L"Is HP ProtectTools (aka \"ActivCard\" Running?"),m_ptkeys()

// list of processes belonging to ProtectTools
{
	m_ptkeys.insert	(L"accoca_exe");	// ActivCard Cache Server
	m_ptkeys.insert	(L"pthosttr_exe");	// ProtectTools Host..?
	m_ptkeys.insert	(L"accrdsub_exe");
	m_ptkeys.insert	(L"acevents_exe");	// ActivCard Events..?
	m_ptkeys.insert	(L"asghost_exe");
	m_ptkeys.insert	(L"ac_sharedstore_exe");
}

HPProtectToolsRule::~HPProtectToolsRule() throw()
{
}

MetaRuleVerdict HPProtectToolsRule::verdict(Repository evidence) const
{
	ContributionSet pt_processes_running;
	for(ContributionSet::const_iterator i=m_ptkeys.begin();i!=m_ptkeys.end();i++)
		if(evidence.exists(L"process_info." + *i + L".running"))
			pt_processes_running.insert(*i);
	
	// if any are none, protecttools appears not to be present, and so this rule judges "not guilty"
	if(pt_processes_running.empty())
	{
		return MetaRuleVerdict(false);
	}
	// if any are some, protecttools is present and so this rule judges "guilty"
	else
	{
		std::wstring	guilties		(L"[");
						guilties.append	(join<std::wstring,ContributionSet>(pt_processes_running,L","));
						guilties.append	(L"]");
						return MetaRuleVerdict(true,	L"HP ProtectTools (a.k.a. \"ActivCard\"  is Running",	// verdict
														L"The following running processes:\n\n" +				// details
														guilties + 
														 L"\n\n"
														L"Indicate the presence of the ProtectTools/ActivCard software, which is known to lock resources required by Belgian e-ID components.",
														L"Stop and Disable ProtectTools/ActivCard",				// suggestion
														name());
	}
}