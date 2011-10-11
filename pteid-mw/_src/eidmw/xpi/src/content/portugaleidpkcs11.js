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
if(!be.fedict.portugaleidpkcs11) be.fedict.portugaleidpkcs11={};

be.fedict.portugaleidpkcs11.notifyModuleNotFound = function() {
    var notificationBox = gBrowser.getNotificationBox();
	var strings = document.getElementById("portugaleid-strings");

    var buttons = [{
         label: strings.getString("downloadmiddleware"),
         accessKey: strings.getString("downloadmiddleware.accessKey"),
         callback: function() { gBrowser.selectedTab = gBrowser.addTab(strings.getString("urldownloadmiddleware")); }
       },
       {
         label: strings.getString("donotshowagain"),
         accessKey: strings.getString("donotshowagain.accessKey"),
         callback: function() {
		    var pteidPKCS11 = Components.classes['@eid.portugal.be/portugaleidpkcs11;1']
                                    .getService().wrappedJSObject;
		     return pteidPKCS11.setShouldShowModuleNotFoundNotification(false);
		}
       }];
    const priority = notificationBox.PRIORITY_WARNING_MEDIUM;
    var not = notificationBox.appendNotification(strings.getString("modulenotfoundonsystem"), "portugaleid",
                                       "chrome://browser/skin/Info.png",
                                       priority, buttons);
    not.persistence = 3;
}

window.addEventListener("load", function(e) {
  try {
  
    var pteidPKCS11 = Components.classes['@eid.portugal.be/portugaleidpkcs11;1']
                                    .getService().wrappedJSObject;
	if (!pteidPKCS11.notificationHasBeenShown && pteidPKCS11.initDone)
	  if (!pteidPKCS11.isModuleInstalled())
	    if (pteidPKCS11.getShouldShowModuleNotFoundNotification()) {
	      be.fedict.portugaleidpkcs11.notifyModuleNotFound();
		  pteidPKCS11.notificationHasBeenShown = true;
        }
  } catch (anError) {
    Components.classes["@mozilla.org/consoleservice;1"]
      .getService(Components.interfaces.nsIConsoleService).logStringMessage("Portugal eID: " + anError);

  }
}, false);