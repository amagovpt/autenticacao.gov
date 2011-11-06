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
#ifndef __DIAGLIB_USER_H__
#define __DIAGLIB_USER_H__

#include <string.h>
#include <vector>

#define CURRENT_USER_ID -1

typedef int User_ID;

typedef struct t_User_INFO {
	User_ID id;
	std::wstring Name;
	bool IsAdmin;
} User_INFO;

int userGetInfo(User_ID user, User_INFO *info);
int userIsAdministrator(User_ID user, bool *admin);

#endif //__DIAGLIB_USER_H__

