var EXPORTED_SYMBOLS = [];

const Cc = Components.classes;
const Ci = Components.interfaces;
const Cr = Components.results;

const gPrefService = Cc["@mozilla.org/preferences-service;1"].getService(Ci.nsIPrefService);
const gPrefBranch = gPrefService.getBranch(null).QueryInterface(Ci.nsIPrefBranch2);
const gDefPrefBranch = gPrefService.getDefaultBranch(null);

const gObserver = Cc['@mozilla.org/observer-service;1'].getService(Ci.nsIObserverService);
const gPermMgr = Cc["@mozilla.org/permissionmanager;1"].getService(Ci.nsIPermissionManager);
const gIOService = Cc["@mozilla.org/network/io-service;1"].getService(Ci.nsIIOService);
const gBundleService = Cc["@mozilla.org/intl/stringbundle;1"].getService(Ci.nsIStringBundleService);
const gCCKBundle = gBundleService.createBundle("chrome://cck-pteidcm/content/cck.properties");

var em;
if (Cc["@mozilla.org/extensions/manager;1"]) {
  em = Cc["@mozilla.org/extensions/manager;1"].getService(Ci.nsIExtensionManager);
}

var bookmarks;
var livemarks;
var annoService;

var CCKService = {
  beingUninstalled: false,
  observe: function observe(subject, topic, data) {
    switch (topic) {
      case "profile-before-change":
        var i = 1;
        var prefName;
        do {
          prefName = getString("LockPref" + i);
          if (prefName && prefName.length) {
            gPrefBranch.unlockPref(prefName);
          }
          i++;
        } while (prefName && prefName.length);
        if (CCKService.beingUninstalled) {
          uninit();
        }
        break;
      case "final-ui-startup":
        bookmarks = Cc["@mozilla.org/browser/nav-bookmarks-service;1"].getService(Ci.nsINavBookmarksService);
        livemarks = Cc["@mozilla.org/browser/livemark-service;2"].getService(Ci.nsILivemarkService);
        annoService = Cc["@mozilla.org/browser/annotation-service;1"].getService(Ci.nsIAnnotationService);
        init();
        if (Cc["@mozilla.org/extensions/manager;1"]) {
          var location = em.getInstallLocation("pteidcertinstall@caixamagica.pt").getItemLocation("pteidcertinstall@caixamagica.pt");
          installXPIs(location);
        } else {
          Components.utils.import("resource://gre/modules/AddonManager.jsm");
          AddonManager.getAddonByID("pteidcertinstall@caixamagica.pt", function(addon) {
            var location = addon.getResourceURI("").QueryInterface(Components.interfaces.nsIFileURL).file;
            installXPIs(location);
          });
        }
        break;

      case "em-action-requested":
        subject.QueryInterface(Components.interfaces.nsIUpdateItem);
        if (subject.id == "pteidcertinstall@caixamagica.pt") {
          switch (data) {
            case "item-uninstalled":
              CCKService.beingUninstalled = true;
              break;
            case "item-cancel-action":
              CCKService.beingUninstalled = false;
              break;
          }
        }
        break;
    }
  }
}

function init() {
  var id = getString("id");
  var extVersion = getString("version");

  var installedVersion = null;

  if (gPrefBranch.prefHasUserValue("extensions.pteidcertinstall@caixamagica.pt.version")) {
    installedVersion = gPrefBranch.getCharPref("extensions.pteidcertinstall@caixamagica.pt.version");
  }

  var firstrun = false;

  if (!gPrefBranch.prefHasUserValue("extensions.pteidcertinstall@caixamagica.pt.install-event-fired")) {
    firstrun = true;
    gPrefBranch.setBoolPref("extensions.pteidcertinstall@caixamagica.pt.install-event-fired", true);
  }

  /* Always lock preferences */
  var prefName;
  var i=1;
  do {
    prefName = getString("LockPref" + i);
    if (prefName) {
      if ((prefName == "browser.startup.homepage") ||
          (prefName == "browser.search.defaultenginename") ||
          (prefName == "browser.search.order.1") ||
          (prefName == "browser.throbber.url")) {
        var url = getString(prefName);
        if (url) {
          gDefPrefBranch.setCharPref(prefName, url);
        } else {
          url = gDefPrefBranch.getComplexValue(prefName, Ci.nsIPrefLocalizedString).data;
          gDefPrefBranch.setCharPref(prefName, url);
        }
      }
      gPrefBranch.lockPref(prefName);
    }
    i++;
  } while (prefName);

  if (firstrun || extVersion != installedVersion)
  {
    // New install, update or re-enabled.  Currently there is no difference.
    /* Windows only */
    if ("@mozilla.org/windows-registry-key;1" in Cc) {
      var RegName, RootKey, Key, Name, NameValue, Type;
      i=1;
      do {
        RegName = getString("RegName" + i);
        if (!RegName) {
          break;
        }
        RootKey = getString("RootKey" + i);
        Key = getString("Key" + i);
        Name = getString("Name" + i);
        NameValue = getString("NameValue" + i);
        Type = getString("Type" + i);
        addRegistryKey(RootKey, Key, Name, NameValue, Type);
        i++;
      } while (RegName)
    }

    var CertName, CertTrust;
    i=1;
    do {
      CertName = getString("Cert" + i);
      if (!CertName)
        break;
      CertTrust = getString("CertTrust" + i);
      if (!CertTrust)
        CertTrust = "C,C,C";
      addCertificate(CertName, CertTrust);
      i++;
    } while (CertName)

    var sites;
    sites = getString("PopupAllowedSites");
    if (sites)
      updatePermissions(sites, "popup", 1);
    sites = getString("InstallAllowedSites");
    if (sites)
      updatePermissions(sites, "install", 1);
    sites = getString("CookieAllowedSites");
    if (sites)
      updatePermissions(sites, "cookie", 1);
    sites = getString("PopupDeniedSites");
    if (sites)
      updatePermissions(sites, "popup", 2);
    sites = getString("InstallDeniedSites");
    if (sites)
      updatePermissions(sites, "install", 2);
    sites = getString("CookieDeniedSites");
    if (sites)
      updatePermissions(sites, "cookie", 2);

    gPrefBranch.setCharPref("extensions.pteidcertinstall@caixamagica.pt.version", extVersion);

    // Setup bookmarks
    var annoId = id + "/" + extVersion;
    var mybookmarks = annoService.getItemsWithAnnotation(annoId, {});
    /* Don't recreate any bookmarks if some are already there. Note that */
    /* if the user delete them all, we recreate */
    if (mybookmarks.length > 0) {
      return;
    }

    var ToolbarLocation;
    ToolbarLocation = getString("ToolbarLocation");
    if ((ToolbarLocation) && (ToolbarLocation == "First")) {
      addBookmarks("Toolbar", bookmarks.toolbarFolder, 1, annoId);
      addFolder("Toolbar", bookmarks.toolbarFolder, 1, annoId);
    } else {
      addFolder("Toolbar", bookmarks.toolbarFolder, -1, annoId);
      addBookmarks("Toolbar", bookmarks.toolbarFolder, -1, annoId);
    }

    var BookmarkLocation;
    BookmarkLocation = getString("BookmarkLocation");
    if ((BookmarkLocation) && (BookmarkLocation == "First")) {
      addBookmarks("", bookmarks.bookmarksMenuFolder, 1, annoId);
      addFolder("Bookmark", bookmarks.bookmarksMenuFolder, 1, annoId);
    } else {
      addFolder("Bookmark", bookmarks.bookmarksMenuFolder, -1, annoId);
      addBookmarks("", bookmarks.bookmarksMenuFolder, -1, annoId);
    }
  }
}

function uninit() {
  if (gPrefBranch.prefHasUserValue("extensions.pteidcertinstall@caixamagica.pt.install-event-fired")) {
    gPrefBranch.clearUserPref("extensions.pteidcertinstall@caixamagica.pt.install-event-fired");
  }

  if (gPrefBranch.prefHasUserValue("extensions.pteidcertinstall@caixamagica.pt.version")) {
    gPrefBranch.clearUserPref("extensions.pteidcertinstall@caixamagica.pt.version");
  }

  var id = getString("id");
  var version = getString("version");
  removeBookmarks(id + "/" + version);
}

function addRegistryKey(RootKey, Key, Name, NameValue, Type) {
  const nsIWindowsRegKey = Ci.nsIWindowsRegKey;
  var key = null;

  try {
    key = Cc["@mozilla.org/windows-registry-key;1"]
                .createInstance(nsIWindowsRegKey);
    var rootKey;
    switch (RootKey) {
      case "HKEY_CLASSES_ROOT":
        rootKey = nsIWindowsRegKey.ROOT_KEY_CLASSES_ROOT;
        break;
      case "HKEY_CURRENT_USER":
        rootKey = nsIWindowsRegKey.ROOT_KEY_CURRENT_USER;
        break;
      default:
        rootKey = nsIWindowsRegKey.ROOT_KEY_LOCAL_MACHINE;
        break;
    }

    key.create(rootKey, Key, nsIWindowsRegKey.ACCESS_WRITE);

    switch (Type) {
      case "REG_DWORD":
        key.writeIntValue(Name, NameValue);
        break;
      case "REG_QWORD":
        key.writeInt64Value(Name, NameValue);
        break;
      case "REG_BINARY":
        key.writeBinaryValue(Name, NameValue);
        break;
      case "REG_SZ":
      default:
        key.writeStringValue(Name, NameValue);
        break;
    }
    key.close();
  } catch (ex) {
    /* This could fail if you don't have the right authority on Windows */
    if (key) {
      key.close();
    }
  }
}

// Helper function to delete a registry key.
// RootKey: One of the registry hive roots.
// ParentKey: The parent of the key you want to delete
// KeyToDelete: The name of the key you want to delete
function deleteRegistryKey(RootKey, ParentKey, KeyToDelete)
{
   const nsIWindowsRegKey = Ci.nsIWindowsRegKey;
   var key = null;
   var deleted = false;

   try
   {
      key = Cc["@mozilla.org/windows-registry-key;1"].createInstance(nsIWindowsRegKey);
      var rootKey;
      switch (RootKey) {
         case "HKEY_CLASSES_ROOT":
            rootKey = nsIWindowsRegKey.ROOT_KEY_CLASSES_ROOT;
            break;

         case "HKEY_CURRENT_USER":
            rootKey = nsIWindowsRegKey.ROOT_KEY_CURRENT_USER;
            break;

         default:
            rootKey = nsIWindowsRegKey.ROOT_KEY_LOCAL_MACHINE;
            break;
      }

      // Check to see if the key we're deleting has any child keys.
      // If so, recursively delete any child keys.
      key.open(rootKey, ParentKey + "\\" + KeyToDelete, nsIWindowsRegKey.ACCESS_READ);

      var keyName;
      while (key.childCount > 0) {
         keyName = key.getChildName(0);

         // If we failed to delete the registry key, abort or we'll go into an infinite recursive loop
         // since there will always be a child key.
         if ( !deleteRegistryKey(RootKey, ParentKey + "\\" + KeyToDelete, keyName) ) {
            log("[DEBUG] Failed to delete registry key: " + KeyToDelete + ".  Aborting deletion of registry key.\n");
            if (key) {
               key.close();
            }

            return false;
         }
      }

      key.close();

      key.open(rootKey, ParentKey, nsIWindowsRegKey.ACCESS_READ);
      key.removeChild(KeyToDelete);
      key.close();
      deleted = true;
   }
   catch (ex) {
      log("[DEBUG] delete registry key failed:\nParent Key: " + RootKey + "\\" + ParentKey + "\nKeyToDelete: " + KeyToDelete + "\n" + ex + "\n");
      if (key) {
         key.close();
      }
   }

   return deleted;
}


function addCertificate(CertName, CertTrust) {
  var certDB = Cc["@mozilla.org/security/x509certdb;1"].getService(Ci.nsIX509CertDB2);
  var scriptableStream=Cc["@mozilla.org/scriptableinputstream;1"].getService(Ci.nsIScriptableInputStream);
  var channel = gIOService.newChannel("chrome://cck-pteidcm/content/" + CertName, null, null);
  var input=channel.open();
  scriptableStream.init(input);
  var certfile=scriptableStream.read(input.available());
  scriptableStream.close();
  input.close();

  var beginCert = "-----BEGIN CERTIFICATE-----";
  var endCert = "-----END CERTIFICATE-----";

  certfile = certfile.replace(/[\r\n]/g, "");
  var begin = certfile.indexOf(beginCert);
  var end = certfile.indexOf(endCert);
  var cert = certfile.substring(begin + beginCert.length, end);
  try {
    certDB.addCertFromBase64(cert, CertTrust, "");
  } catch (ex) {
    Components.utils.reportError("addCert failed: " + ex);
  }
}

function updatePermissions(sites, type, permission) {
  var sitesArray = sites.split(",");
  for (var i=0; i < sitesArray.length; i++) {
    try {
      var uri = gIOService.newURI("http://" + sitesArray[i], null, null);
      gPermMgr.add(uri, type, permission);
    } catch (ex) {}
  }
}

function addBookmarks(prefix, container, location, inId) {
  var BookmarkTitle;
  var BookmarkURL;

  // items are added in reverse order if we are set to "First"
  var start, end, increment;

  var i = 1;
  var numBookmarks = 0;
  var curtem;
  while (1) {
    var title = getString(prefix + "BookmarkTitle" + i);
    if (!title)
      break;
    i++;
    numBookmarks++;
  }

  if (location == -1) {
    start = 1;
    end = numBookmarks+1;
    increment = 1;
  } else {
    start = numBookmarks;
    end = 0;
    increment = -1;
  }

  for (var i=start; i!=end; i+=increment) {
    BookmarkTitle = getString(prefix + "BookmarkTitle" + i);
    if (BookmarkTitle) {
      BookmarkURL = getString(prefix + "BookmarkURL" + i);
      var bmtype = getString(prefix + "BookmarkType" + i);
      var curitem;
      if (bmtype == "separator") {
        curitem = bookmarks.insertSeparator(container, location);
      } else if (BookmarkURL) {
        if (bmtype == "live") {
          curitem = livemarks.createLivemark(container, BookmarkTitle, null, makeURI(BookmarkURL), location);
        }else {
          curitem = bookmarks.insertBookmark(container, makeURI(BookmarkURL), location, BookmarkTitle);
        }
      }

      if (curitem) {
        annoService.setItemAnnotation(curitem, inId, "true", 0, annoService.EXPIRE_NEVER);
      }
    }
  }
}

function addFolder(prefix, container, location, inId) {
  var BookmarkFolder;

  // items are added in reverse order if we are set to "First"
  var start, end, increment;

  if (location == -1) {
    start = 1;
    end = 6;
    increment = 1;
  } else {
    start = 5;
    end = 0;
    increment = -1;
  }

  // Bookmarks folder with bookmarks
  for (var i=start; i!=end; i+=increment) {
    BookmarkFolder = getString(prefix + "Folder" + i);
    if (BookmarkFolder) {
      var newfolder = bookmarks.createFolder(container, BookmarkFolder, location);
      annoService.setItemAnnotation(newfolder, inId, "true", 0, annoService.EXPIRE_NEVER);
      addBookmarks(prefix + "Folder" + i + ".", newfolder, 0, inId);
    }
  }
}


// Helper function to clean up any bookmarks associated with a specific annoation.
function removeBookmarks(annoId)
{
  // Remove all bookmarks first.
  // The user may have added their own bookmarks to the folder.  If the folder
  // is not completely empty (including other bookmarks, folders or separators),
  // the folder will not be removed (but the annotation will and it will become
  // a "normal" folder.
  var bmType = null;
  var cckBookmarks = annoService.getItemsWithAnnotation(annoId, {});
  for (var i = 0; i < cckBookmarks.length; i++) {
     try
     {
        bmType = bookmarks.getItemType(cckBookmarks[i]);
        if ( (bmType == bookmarks.TYPE_BOOKMARK) || (bmType == bookmarks.TYPE_SEPARATOR) ) {
          bookmarks.removeItem(cckBookmarks[i]);
        }
     }
      catch (ex) {
         log("Exception removing item (" + cckBookmarks[i] + "): " + ex + "\n");
     }
  }

  // Iterate a second time and remove the folders, if empty.
  cckBookmarks = annoService.getItemsWithAnnotation(annoId, {});
  for (i = 0; i < cckBookmarks.length; i++)
  {
    try
    {
      bmType = bookmarks.getItemType(cckBookmarks[i]);

      if (bmType == bookmarks.TYPE_FOLDER)
      {
         var itemId = bookmarks.getIdForItemAt(cckBookmarks[i], bookmarks.DEFAULT_INDEX);
         if (itemId == -1)
         {
           bookmarks.removeItem(cckBookmarks[i]);
         }
         else {
           annoService.removeItemAnnotation(cckBookmarks[i], annoId);
         }
      }
    }
    catch(ex) {
      log("Exception checking if folder is empty: " + ex + "\n");
    }
  }
}

function getString(id) {
  try {
    var string = gCCKBundle.GetStringFromName(id);
    if (string.length) {
      return string;
    }
  } catch (ex) {}
  return undefined;
}

function makeURI(aURL, aOriginCharset, aBaseURI) {
  try {
    var uri = gIOService.newURI(aURL, aOriginCharset, aBaseURI);
    return uri;
  } catch (ex) {
    log(ex);
    log(aURL);
  }
  return null;
}


function installXPIs(location) {
  var xpisToInstall = [];
  var numXPIsInstalled = 0;

  var e = location.directoryEntries;
  while (e.hasMoreElements()) {
    var f = e.getNext().QueryInterface(Components.interfaces.nsIFile);
    var splitpath = f.path.split(".");
    if (splitpath[splitpath.length-1] == "xpi") {
      xpisToInstall.push(f);
    }
  }
  for (let i=0; i < xpisToInstall.length; i++) {
    if (Cc["@mozilla.org/extensions/manager;1"]) {
      em.installItemFromFile(xpisToInstall[i], "app-profile");
    } else {
      AddonManager.getInstallForFile(xpisToInstall[i], function(addonInstall) {
        let listener = {
          onInstallEnded: function(addon) {
            numXPIsInstalled++;
            if (numXPIsInstalled == xpisToInstall.length) {
              restartApp(xpisToInstall);
            }
          }
        }
        addonInstall.addListener(listener);
        addonInstall.install();
      });
    }
  }
  if (Cc["@mozilla.org/extensions/manager;1"]) {
    if (xpisToInstall.length > 0) {
      restartApp(xpisToInstall);
    }
  }
}

function restartApp(xpisToRemove) {
  for (let i=0; i < xpisToRemove.length; i++) {
    try {
      xpisToRemove[i].remove(true);
    } catch (ex) {}
  }
  var nsIAppStartup = Cc["@mozilla.org/toolkit/app-startup;1"].getService(Components.interfaces.nsIAppStartup);
  nsIAppStartup.quit(nsIAppStartup.eRestart | nsIAppStartup.eAttemptQuit);
}

function log(string) {
  Components.utils.reportError(string);
  dump(string);
}

gObserver.addObserver(CCKService, "em-action-requested", false);
gObserver.addObserver(CCKService, "profile-before-change", false);
gObserver.addObserver(CCKService, "profile-after-change", false);
gObserver.addObserver(CCKService, "final-ui-startup", false);

let listener = {
  onUninstalling: function(addon) {
    if (addon.id == "pteidcertinstall@caixamagica.pt") {
      CCKService.beingUninstalled = true;
    }
  },
  onOperationCancelled: function(addon) {
    if (addon.id == "pteidcertinstall@caixamagica.pt") {
      CCKService.beingUninstalled = false;
    }
  }
}

try {
  Components.utils.import("resource://gre/modules/AddonManager.jsm");
  AddonManager.addAddonListener(listener);
} catch (ex) {}
