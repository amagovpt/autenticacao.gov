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

#ifndef __METARULE__
#define __METARULE__

#include <string>
#include "repository.h"
#include "MetaRuleVerdict.h"

class MetaRule
{
public:
								MetaRule(const std::wstring name, const std::wstring description);
	virtual					   ~MetaRule() throw();
	virtual MetaRuleVerdict		verdict (Repository evidence) const=0;
	const	std::wstring&		name(void) const;
	const	std::wstring&		description(void) const;
private:
	std::wstring				m_name;
	std::wstring				m_description;
};

#endif