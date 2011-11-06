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
#include "progress.h"

#include "error.h"
#include "log.h"

#include <vector>

static progressCallback g_progressCallback = NULL;
static std::vector<int>  g_currentValue;
static std::vector<int> g_maxValue;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void progressSetCallback(progressCallback fct)
{
	g_progressCallback = fct;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void progressIncrement(void)
{
	g_currentValue.back()++;

	if(g_progressCallback)
	{
		g_progressCallback(progressGetValue());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void progressInit(int max)
{
	int currentValue = 0;
	int maxValue = max;

	if(g_maxValue.size() > 0)
	{
		currentValue= max * (g_currentValue.back());
		maxValue = max * (g_maxValue.back());
	}

	g_currentValue.push_back(currentValue);
	g_maxValue.push_back(maxValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void progressRelease(void)
{
	g_currentValue.pop_back();
	g_maxValue.pop_back();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int progressGetValue(void)
{
	if(g_maxValue.size() > 0)
	{
		return (100 * (g_currentValue.back())/(g_maxValue.back()));
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
