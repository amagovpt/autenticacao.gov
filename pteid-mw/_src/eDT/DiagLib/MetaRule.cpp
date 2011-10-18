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

#include "MetaRule.h"

MetaRule::MetaRule(const std::wstring name, const std::wstring description)
:m_name(name),m_description(description)
{
}

MetaRule::~MetaRule() throw()
{
}

const std::wstring& MetaRule::name(void) const
{
	return m_name;
}

const std::wstring& MetaRule::description(void) const
{
	return m_description;
}

/* example
MetaRuleVerdict MetaRule::verdict (Repository evidence) const
{
	return MetaRuleVerdict(TRUE,"eidmw not installed","install eid-mw",name());
};
*/