/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 André Guerreiro - <aguerreiro1985@gmail.com>
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
#include "Context.h"
#include "Config.h"

namespace eIDMW
{
	CContext::CContext()
	{
		m_bSSO = false; 

		m_ulConnectionDelay = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDCONNDELAY);
	}

	CContext::~CContext()
	{
		m_oThreadPool.FinishThreads();

		m_oPCSC.ReleaseContext();
	}
}
