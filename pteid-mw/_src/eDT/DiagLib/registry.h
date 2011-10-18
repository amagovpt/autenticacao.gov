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
#ifndef __DIAGLIB_REGISTRY_H__
#define __DIAGLIB_REGISTRY_H__

#include <string.h>
#include <vector>

typedef enum e_Reg_Key_TYPE
{
	USER_KEY_TYPE,
	SYSTEM_KEY_TYPE
} Reg_Key_TYPE;

typedef struct t_Reg_ID {
	Reg_Key_TYPE Type;
	std::wstring Key;
    std::wstring Name;
} Reg_ID;

typedef std::vector<Reg_ID> Reg_LIST;

typedef enum e_Reg_Value_TYPE
{
	STRING_VALUE_TYPE,
	INT_VALUE_TYPE
} Reg_Value_TYPE;

typedef struct t_Reg_INFO {
	Reg_ID id;
	Reg_Value_TYPE ValueType;
    std::wstring ValueStr;
    int ValueInt;
} Reg_INFO;

int registryExist(Reg_ID key, bool *exist);
int registryGetInfo(Reg_ID key, Reg_INFO *info);

int registryGetSubKey(Reg_ID key, Reg_LIST *regList, const wchar_t *matchingPatern = NULL);

int registrySetInfo(Reg_INFO info);
int registryDelete(Reg_ID key);

#endif //__DIAGLIB_REGISTRY_H__
