const Cc = Components.classes;
const Ci = Components.interfaces;

Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");

function CCKService() {}

CCKService.prototype = {
  observe: function(aSubject, aTopic, aData) {
    switch(aTopic) {
      case "profile-after-change":
        Components.utils.import("resource://cck-pteidcm/cckModule.jsm");
        break;
    }
  },
  classDescription: "CCK Service - pteidcm",
  contractID: "@mozilla.org/cck-service-pteidcm;2",
  classID: Components.ID("{0408b583-2f84-4e7b-9a56-e29b198b0a7e}"),
  QueryInterface: XPCOMUtils.generateQI([Ci.nsIObserver]),
  _xpcom_categories: [{category: "profile-after-change"}]
}

if (XPCOMUtils.generateNSGetFactory)
  var NSGetFactory = XPCOMUtils.generateNSGetFactory([CCKService]);
else
  var NSGetModule = XPCOMUtils.generateNSGetModule([CCKService]);

