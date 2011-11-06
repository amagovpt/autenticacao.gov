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

#include <set>
#include "repository.h"
#include "MetaRule.h"
#include "MetaRuleVerdict.h"

class HPProtectToolsRule : public MetaRule
{
public:
	HPProtectToolsRule();
	~HPProtectToolsRule() throw();
	MetaRuleVerdict verdict(Repository evidence) const;
private:
	std::set<std::wstring> m_ptkeys;
};