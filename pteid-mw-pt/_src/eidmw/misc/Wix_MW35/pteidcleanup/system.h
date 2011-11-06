/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#pragma once

enum InstallType
{
	INSTALLTYPE_MSI,
	INSTALLTYPE_IS,
	INSTALLTYPE_IS_MSI
};

bool isUserAdmin();
int Uninstall(const wchar_t *defaultname,const wchar_t *guid, InstallType type, int issResource, long lTimeout, const wchar_t *keepguid, bool *pbRebootNeeded);
int SearchAndUninstall(const wchar_t *defaultname,const wchar_t *wzGuidPattern, long lTimeout, const wchar_t *keepguid, bool *pbRebootNeeded);