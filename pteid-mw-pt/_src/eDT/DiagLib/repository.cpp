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
#ifdef WIN32
#include <windows.h>
#elif defined __APPLE__
#include "Mac/mac_helper.h"
#endif

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <algorithm>

#include "diaglib.h"
#include "report.h"
#include "file.h"

#include "error.h"
#include "log.h"
#include "folder.h"
#include <map>
#include "repository.h"
#include "util.h"

Repository::Repository()
:m_contributions(),m_current_prefix(L"default"),m_prefixes()
{
}

/*
	prefixChanged recalculates the textual dot-separated label prefix.
	private.
*/
void Repository::prefixChanged(void)
{
	m_current_prefix.clear();
	for(PrefixList::size_type i=0;i<m_prefixes.size();i++)
	{
		m_current_prefix.append(m_prefixes[i]);
		if(i<m_prefixes.size())
			m_current_prefix.append(L".");
	}	
}

/*
	enter a deeper level in the namespace, indicating that between here and the corresponding call to ünprefix(),
	all keys contributed should be grouped under this subkey. This version allows printf-like templating.
	Every prefix call should be balanced by a corresponding unprefix() call
*/
void Repository::prefix(const std::wstring format, ...)
{
	wchar_t val[1024];
	va_list args;
	va_start(args,format);
	std::wstring cleanFormat(format);wstring_clean(cleanFormat);
	vswprintf(val,sizeof(val)-2,cleanFormat.c_str(),args);
	va_end(args);
	m_prefixes.push_back(val);
	prefixChanged();
}

/*
	enter a deeper level in the namespace, indicating that between here and the corresponding call to ünprefix(),
	all keys contributed should be grouped under this subkey. 
	Every prefix call should be balanced by a corresponding unprefix() call
*/
void Repository::prefix(const std::string newPrefix)
{
	prefix(wstring_From_string(newPrefix));
}

/* 
	return to a more shallow level in the namespace. complementary to the prefix() calls
	call when finished contributing a series of keys that were grouped under a subkey, as initiated by a prefix() call.
*/
void Repository::unprefix()
{
	m_prefixes.pop_back();
	prefixChanged();
}

/*
	indicate whether the current subkey in the namespace contains useful information.
	call with parameter false, when after entering a deeper subkey level using prefix(), there are no useful values to contribute.
	call with true when useful values have been contributed.
	This allows us to see that a certain test ran, even if there are no results.
*/
void Repository::available(bool avail)
{
	contribute(L"available",avail?L"true":L"false");
}

/*
	contribute a value, under the current subkey. Several values may be contributed under the same key
	This version allows printf-like templating.
*/
void Repository::contribute(const ContributionKey key, const std::wstring format, ...)
{
	wchar_t _val[1024];
	va_list args;
	va_start(args,format);
	vswprintf(_val,sizeof(_val)-2,format.c_str(),args);
	va_end(args);
	std::wstring val(_val);

	if(val.length()>0)
	{
		std::wstring	 fullKey(m_current_prefix);
						 fullKey.append(key);
		wstring_to_lower(fullKey);
		m_contributions.insert(ContributionEntry(fullKey,val));
	}
}

void Repository::contribute(const MetaRuleVerdict verdict)
{
	for(VerdictList::const_iterator v=m_verdicts.begin();v!=m_verdicts.end();v++)	// every MetaRule gets to contribute one verdict only.
		if((*v).rulename()==verdict.rulename())
			return;
	m_verdicts.push_back(verdict);
}

/* 
	retrieve all values for a certain key
	returns: a set of values, empty set if none found
*/
ContributionSet	Repository::values(const ContributionKey key) const
{
	std::set<std::wstring> vls;
	for(ContributionMap::const_iterator i=m_contributions.lower_bound(key);i!=m_contributions.upper_bound(key);i++)
		vls.insert(i->second);
	return vls;
}

Contribution Repository::value(const ContributionKey key) const
{
	ContributionMap::const_iterator i=m_contributions.lower_bound(key);
	if(i==m_contributions.end())
		return NULL;
	else
		return i->second;
}

/* 
	return true if key exists (at least once)
*/
bool Repository::exists(const ContributionKey key) const
{
	return (m_contributions.find(key)!=m_contributions.end());
}

/*
	returns all the contributions
*/
const ContributionMap& Repository::results(void) const
{
	return m_contributions;
}

/*
	count of verdicts
*/

size_t Repository::verdictCount(void) const
{
	return m_verdicts.size();
}

/*
	return any verdicts
*/
MetaRuleVerdict Repository::verdict(unsigned int which) const
{
	return m_verdicts[which];
}