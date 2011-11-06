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
#ifndef __DIAGLIB_REPOSITORY_H__
#define __DIAGLIB_REPOSITORY_H__

#include <stdio.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "MetaRuleVerdict.h"

using namespace std;

typedef	std::wstring								ContributionKey;
typedef	std::wstring								Contribution;
typedef std::multimap<ContributionKey,Contribution>	ContributionMap;
typedef std::set<Contribution>						ContributionSet;
typedef std::pair<ContributionKey,Contribution>		ContributionEntry;
typedef std::vector<std::wstring>					PrefixList;
typedef std::vector<MetaRuleVerdict>				VerdictList;

class Repository
{
public:
	void					prefix		(const std::wstring format, ...);
	void					prefix		(const std::string newPrefix);
	void					unprefix	(void);
	void					available	(bool avail);
	void					contribute	(const ContributionKey key, const std::wstring format, ...);
	void					contribute	(const MetaRuleVerdict verdict);

	Contribution			value		(const ContributionKey key) const;
	ContributionSet			values		(const ContributionKey key) const;
	bool					exists		(const ContributionKey key) const;
	const ContributionMap&	results		(void) const;
	size_t					verdictCount(void) const;
	MetaRuleVerdict			verdict		(unsigned int which) const;

	// Meyers Singleton
	static Repository& instance()	
	{
		static Repository theRepository;
		return theRepository;
	}

private:
	Repository();
	void prefixChanged(void);
	std::wstring		m_current_prefix;
	PrefixList			m_prefixes;
	ContributionMap		m_contributions;
	VerdictList			m_verdicts;
	static Repository*	m_singleton;
};

// readability convenience macros
#define REPOSITORY			Repository::instance()
#define REP_CONTRIBUTE		Repository::instance().contribute
#define REP_PREFIX			Repository::instance().prefix
#define REP_UNPREFIX		Repository::instance().unprefix
#define REP_AVAILABLE		Repository::instance().available
#define REP_RESULTS			Repository::instance().results
#define REP_VALUES			Repository::instance().values
#define REP_VERDICT_COUNT	Repository::instance().verdictCount
#define REP_VERDICT			Repository::instance().verdict

#endif //__DIAGLIB_REPOSITORY_H__
