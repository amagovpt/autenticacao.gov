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
#ifndef _CALLBACKDATA_H_
#define _CALLBACKDATA_H_

//************************************************
// base class for callback information
// The application using the engine can:
// - derive a class from this baseclass
// - fill in the callback function
// - fill in user data pointer if necessary
// - pass the base pointer to the engine to set the callbacks
//************************************************

class CallbackData
{
public:
	CallbackData()
		: m_callback(NULL)
		, m_userData(NULL)
	{

	}
	~CallbackData()
	{
	}

public:
	int (*m_callback)(void*, std::string , size_t );		// callback function placeholder
	void* m_userData;										// callback user data placeholder
};

#endif