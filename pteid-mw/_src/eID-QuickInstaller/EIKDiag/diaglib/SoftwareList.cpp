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

#include "StdAfx.h"
#include "softwarelist.h"
#include "wildcards.h"

CSoftwareList::CSoftwareList(void) {

  this->_LastError = "";

}

CSoftwareList::~CSoftwareList(void) {
}

InstalledSoftware& CSoftwareList::GetInstalledSoftware() {

  const char *regkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";


  this->_InstalledSoftware.clear();

  for(int i=0;i<2;i++)
  {
	  HKEY RootKey;
	  if(i==0)
		  RootKey=HKEY_LOCAL_MACHINE;
	  else
		  RootKey=HKEY_CURRENT_USER;

	  CRegistry software(RootKey, regkey);

	  if (!software.GetSubKeys()) {
		this->_LastError = software.LastError();
		continue;
		//return this->_InstalledSoftware;
	  }

	  RegSubKeys& sk = software.SubKeys();
	  RegSubKeysIterator last = sk.end();
	  for (RegSubKeysIterator it = sk.begin(); it != last; ++it) {
		string key = regkey;
		key += "\\";
		key += (*it);
		CRegistry product(RootKey, key.c_str());
		string name = "";
		string version = "";
		string uninstallstring = "";
		if (product.GetStringValue("DisplayName")) {
		  name = product.StringValue();
		  if (!product.GetStringValue("DisplayVersion")) {
			version = "";
		  } else {
			version = product.StringValue();
		  } 
		  if (!product.GetStringValue("UninstallString")) {
			  // soms is het een Expandedstring, we gaan dus eens kijken of er niet toch een waarde is.
			  if (!product.GetExpandStringValue("UninstallString")) {
				  uninstallstring = "";
			  }
			  else {
				 uninstallstring = product.StringValue();	  
			  }
		  }
		  else {
				 uninstallstring = product.StringValue();	  
		  }
		}
		CSoftwareProduct sp((*it).c_str(), name.c_str(), version.c_str(), uninstallstring.c_str());
		this->_InstalledSoftware.push_back(sp);
	  }
  }
  return this->_InstalledSoftware;
}

SoftwareFound& CSoftwareList::FindSoftware(
    const char *ProductName,
    const char *DisplayName,
    const char *DisplayVersion)
{
  this->_SoftwareFound.clear();

  if (this->_InstalledSoftware.size() == 0)
    return this->_SoftwareFound;

  InstalledSoftwareIterator last = this->_InstalledSoftware.end();

  for (InstalledSoftwareIterator it = this->_InstalledSoftware.begin(); it != last; ++it) {

    if (ProductName != NULL) {
      if (! Wildcard::wildcardfit(ProductName, it->ProductName().c_str())) {
        continue;
      }
    }
    if (DisplayName != NULL) {
      if (! Wildcard::wildcardfit(DisplayName, it->DisplayName().c_str())) {
        continue;
      }
    }
    if (DisplayVersion != NULL) {
      if (! Wildcard::wildcardfit(DisplayVersion, it->DisplayVersion().c_str())) {
        continue;
      }
    }

    this->_SoftwareFound.push_back(*it);

  }

  return this->_SoftwareFound;

}
