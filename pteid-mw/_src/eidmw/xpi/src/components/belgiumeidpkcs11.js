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
Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");

/***********************************************************
class definition
***********************************************************/

//class constructor
function BelgiumEidPKCS11() {
// If you only need to access your component from Javascript, uncomment the following line:
  this.wrappedJSObject = this;
}

// class definition
BelgiumEidPKCS11.prototype = {
  // properties required for XPCOM registration:
  classDescription: "Belgium eID PKCS11 module registration",
  classID:          Components.ID("{26c4340c-ec4a-4173-bc6e-3d2023012177}"),
  contractID:       "@eid.belgium.be/belgiumeidpkcs11;1",

  _xpcom_categories: [{category: "profile-after-change", service: true }, // Gecko 2.0 (FF 4.0)
					  {category: "final-ui-startup", service: true },     // Gecko <2.0 (FF <4.0)
                      {category: "app-startup", service: true }],         // Gecko <2.0 (FF <4.0)

  // QueryInterface implementation, e.g. using the generateQI helper (remove argument if skipped steps above)
  QueryInterface: XPCOMUtils.generateQI([Components.interfaces.nsIObserver]),

  // ...component implementation...
  // define the function we want to expose in our interface
  observe: function(aSubject, aTopic, aData) {
 	switch(aTopic) {
	   case "profile-after-change":  // Gecko 2.0 (FF 4.0)
       case "final-ui-startup":      // Gecko <2.0 (FF <4.0)
	     this.init();
         break;
	   case "app-startup":           // Gecko <2.0 (FF <4.0)
	     Components.classes['@mozilla.org/observer-service;1'].getService(Components.interfaces.nsIObserverService)
		   .addObserver(this,"final-ui-startup",false);
	     break;
    }
  },
  getPrefs: function() {
    var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                    .getService(Components.interfaces.nsIPrefService);
    return prefs.getBranch("extensions.belgiumeid.");
  },
  defaultPrefs: {
    moduleLocation: function() {
	  var osString = Components.classes["@mozilla.org/xre/app-info;1"]
                 .getService(Components.interfaces.nsIXULRuntime).OS;
	  if (osString == "WINNT")
	    return "beidpkcs11.dll";
	  if (osString == "Darwin")
	    return "/Library/Frameworks/BeId.framework/Versions/4.0/lib/libbeidpkcs11.dylib;/usr/local/lib/beid-pkcs11.bundle;/usr/local/lib/beid-pkcs11.bundle/Contents/MacOS/libbeidpkcs11.dylib";
	  if (osString == "Linux")
	    return "libbeidpkcs11.so;/usr/local/lib/libbeidpkcs11.so";
	}	
  },
  getPKCS11ModuleDB: function() {
    return Components.classes["@mozilla.org/security/pkcs11moduledb;1"].getService(Components.interfaces.nsIPKCS11ModuleDB);
  },
  getModuleLocation: function() {
    return this.getPrefs().getCharPref("modulelocation");
  },
  getModuleName: function() {
    return this.getPrefs().getCharPref("modulename");
  },
  errorLog: function(aMessage) {
	var consoleService = Components.classes["@mozilla.org/consoleservice;1"]
                                 .getService(Components.interfaces.nsIConsoleService);
	consoleService.logStringMessage("Belgium eID: " + aMessage);
  },

  /* Searches for module with the given name
   * and returns true if the module and a slot is found
   * otherwise, returns false
   */
  slotFound: function(modulename){
	var found;
	var belgiumEidPKCS11Module = this.findModuleByName(modulename)
	try {
	   slot = belgiumEidPKCS11Module.listSlots().currentItem();
	   found = true;
	} catch (e)
	{
		found = false;
	}
	return found;
  },

  /* Registers PKCS11 module
   *    returns
   *       true   if no modulelocation is found in the preferences
   *              -or- if the module is already found
   *       false  if the registration failed
   */
  registerModule: function() {
    const PKCS11_PUB_READABLE_CERT_FLAG  =  0x1<<28; //Stored certs can be read off the token w/o logging in
    if (this.getModuleLocation() == "") {
		this.errorLog("belgiumeid.modulelocation is not set");
		return true; // unsupported platform
	}
    var installSucceeded = false;
	var moduleLocations = this.getModuleLocation().split(";");
	for (x in moduleLocations) {
		// check if the module is already installed
		if (this.findModuleByLibName(moduleLocations[x]) == null) {
			try {
				// PKCS11_PUB_READABLE_CERT_FLAG: we instruct FF not to ask for PIN when reading certificates
				// This only makes sense when the CKF_LOGIN_REQUIRED flag is set on the token in P11 module
				var modulename = this.getModuleName() + " - " + moduleLocations[x];
				this.addModule(modulename, moduleLocations[x], PKCS11_PUB_READABLE_CERT_FLAG,0);
				this.errorLog("Added PKCS11 module " + modulename + " Location: " + moduleLocations[x]);
				installSucceeded = true;
				break;
			} catch (e) {
				this.errorLog("Failed to load module " + moduleLocations[x] + " with the name " + 
						modulename + ": Error " + e.name + ": " + e.message);
				continue;
			}
		} 
	}
	return installSucceeded;
  },
  /* Looks in the list of registered PKCS11 modules for a module with a specified name
   *    return module if found
   *           null if not found
   */
  findModuleByName: function (modulename) {
    try {
      return this.getPKCS11ModuleDB().findModuleByName(modulename);
    } catch (e) {
		this.errorLog("module with name " + modulename + " not found");
      return null;
    }
  },

  /* Looks in the list of registered PKCS11 modules for a module with a specified system path
   *   return module if found
   *           null if not found
   */
  findModuleByLibName: function (modulelibname) {
	const nsIPKCS11Module = Components.interfaces.nsIPKCS11Module;
    var modules = this.getPKCS11ModuleDB().listModules();
    try {
      modules.isDone();
    } catch (e) { 
      return null;
    }
    while (true) {
      var module = modules.currentItem().QueryInterface(nsIPKCS11Module);
      if (module) {
        if (module.libName == modulelibname) {
          return module;
        }
      }
      try {
        modules.next();
      } catch (e) { 
        break;
      }
    }
    return null;
  },
  setShouldShowModuleNotFoundNotification: function (shouldshow) {
    if (shouldshow != undefined) {
      this.getPrefs().setBoolPref("showmodulenotfoundnotification", shouldshow);
    }
    return this.getPrefs().getBoolPref("showmodulenotfoundnotification");
  },
  getShouldShowModuleNotFoundNotification: function () {
    return this.getPrefs().getBoolPref("showmodulenotfoundnotification");
  },
  setDefaultPrefs: function() {
    if (this.defaultPrefs.moduleLocation()) {
		var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                    .getService(Components.interfaces.nsIPrefService);
		var defaultBelgiumeidPrefs = prefs.getDefaultBranch("extensions.belgiumeid.");
		defaultBelgiumeidPrefs.setCharPref("modulelocation",this.defaultPrefs.moduleLocation());
	}
  },
  init: function() {
    try {
      this.setDefaultPrefs();
	  this.registerModule();
    } catch (anError) {
        this.errorLog(anError);
    }
	this.initDone = true;
  },
  /* Looks in the list of registered PKCS11 modules if a Belgium eID module is installed
   *   return true if at least one is found
   *          false if none is found
   */
  isModuleInstalled: function() {
    var modules = this.getPKCS11ModuleDB().listModules();
    try {
      modules.isDone();
    } catch (e) {
      return false;
    }
    while (true) {
      var module = modules.currentItem().QueryInterface(Components.interfaces.nsIPKCS11Module);
      if (module) {
	    // OK if module name starts with the module name provided in the preferences
        if (module.name.indexOf(this.getModuleName()) == 0) 
          return true;
        
		// OK if module location of the module is in our list
		if (this.getModuleLocation().indexOf(module.libName) != -1 )
		  return true;
      }
      try {
        modules.next();
      } catch (e) {
        break;
      }
    }
	// no valid module found
    return false;
  },
  initDone: false,
  notificationHasBeenShown: false
};
try {
  BelgiumEidPKCS11.prototype.addModule = Components.classes["@mozilla.org/security/pkcs11;1"].getService(Components.interfaces.nsIPKCS11).addModule;
}  
catch (e) {
    // pre-firefox 3.5 interface
  BelgiumEidPKCS11.prototype.addModule = Components.classes["@mozilla.org/security/pkcs11;1"].getService(Components.interfaces.nsIDOMPkcs11).addmodule;
};
/**
* XPCOMUtils.generateNSGetFactory was introduced in Mozilla 2 (Firefox 4).
* XPCOMUtils.generateNSGetModule is for Mozilla 1.9.2 (Firefox 3.6).
*/
if (XPCOMUtils.generateNSGetFactory)
    var NSGetFactory = XPCOMUtils.generateNSGetFactory([BelgiumEidPKCS11]);
else
    var NSGetModule = XPCOMUtils.generateNSGetModule([BelgiumEidPKCS11]);
