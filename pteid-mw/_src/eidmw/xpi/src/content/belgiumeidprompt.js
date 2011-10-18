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

be.fedict.belgiumeidpkcs11.belgiumeidprompt = function() {

  var infobody = document.getElementById("info.body");

  // only change labels when asked for a BELPIC PIN
  if (infobody.textContent.search("BELPIC") != -1) {
    // look for main window to get the localisation strings
    var wm = Components.classes["@mozilla.org/appshell/window-mediator;1"]
                   .getService(Components.interfaces.nsIWindowMediator);
    var mainWindow = wm.getMostRecentWindow("navigator:browser");
    var strings = mainWindow.document.getElementById("belgiumeid-strings");
    var infoicon = document.getElementById("info.icon");
    var commondialog = document.getElementById("commonDialog");
    
    // set icon
    infoicon.setAttribute("src", "chrome://belgiumeid/skin/icon40x40.png");
    // replace "Enter master password" label with a more explanatory label
    infobody.textContent = strings.getString("enterpin");
    // replace dialog title
    commondialog.setAttribute("title",  strings.getString("enterpin"));
  }
}

// add on load event handler
window.addEventListener("load", function() { be.fedict.belgiumeidpkcs11.belgiumeidprompt(); }, false);
