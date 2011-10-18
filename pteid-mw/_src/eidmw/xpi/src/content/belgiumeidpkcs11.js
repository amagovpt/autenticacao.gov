/*
 * eID Middleware Project.
 * Copyright (C) 2008-2011 FedICT.
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
 */

if(!be) var be={};
if(!be.fedict) be.fedict={};
if(!be.fedict.belgiumeidpkcs11) be.fedict.belgiumeidpkcs11={};

be.fedict.belgiumeidpkcs11.notifyModuleNotFound = function() {
    var notificationBox = gBrowser.getNotificationBox();
	var strings = document.getElementById("belgiumeid-strings");

    var buttons = [{
         label: strings.getString("downloadmiddleware"),
         accessKey: strings.getString("downloadmiddleware.accessKey"),
         callback: function() { gBrowser.selectedTab = gBrowser.addTab(strings.getString("urldownloadmiddleware")); }
       },
       {
         label: strings.getString("donotshowagain"),
         accessKey: strings.getString("donotshowagain.accessKey"),
         callback: function() {
		    var beidPKCS11 = Components.classes['@eid.belgium.be/belgiumeidpkcs11;1']
                                    .getService().wrappedJSObject;
		     return beidPKCS11.setShouldShowModuleNotFoundNotification(false);
		}
       }];
    const priority = notificationBox.PRIORITY_WARNING_MEDIUM;
    var not = notificationBox.appendNotification(strings.getString("modulenotfoundonsystem"), "belgiumeid",
                                       "chrome://browser/skin/Info.png",
                                       priority, buttons);
    not.persistence = 3;
}

window.addEventListener("load", function(e) {
  try {
  
    var beidPKCS11 = Components.classes['@eid.belgium.be/belgiumeidpkcs11;1']
                                    .getService().wrappedJSObject;
	if (!beidPKCS11.notificationHasBeenShown && beidPKCS11.initDone)
	  if (!beidPKCS11.isModuleInstalled())
	    if (beidPKCS11.getShouldShowModuleNotFoundNotification()) {
	      be.fedict.belgiumeidpkcs11.notifyModuleNotFound();
		  beidPKCS11.notificationHasBeenShown = true;
        }
  } catch (anError) {
    Components.classes["@mozilla.org/consoleservice;1"]
      .getService(Components.interfaces.nsIConsoleService).logStringMessage("Belgium eID: " + anError);

  }
}, false);