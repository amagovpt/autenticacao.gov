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

#include "MetaRuleVerdict.h"

MetaRuleVerdict::MetaRuleVerdict(bool guilty, std::wstring verdict, std::wstring details, std::wstring corrective, std::wstring rulename)
:m_guilty(guilty),m_verdict(verdict),m_details(details),m_corrective(corrective),m_rulename(rulename)
{
};

MetaRuleVerdict::~MetaRuleVerdict() throw()
{
};
	
bool MetaRuleVerdict::guilty(void) const
{
	return m_guilty;
};

std::wstring MetaRuleVerdict::verdict(void) const
{
	return m_verdict;
};

std::wstring MetaRuleVerdict::details(void) const
{
	return m_details;
};

std::wstring MetaRuleVerdict::corrective(void) const
{
	return m_corrective;
};

std::wstring MetaRuleVerdict::rulename(void) const
{
	return m_rulename;
};
