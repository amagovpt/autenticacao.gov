Belgium eID Firefox Extension
=============================
How to create a development environment
---------------------------------------
(see also https://developer.mozilla.org/en/Setting_up_extension_development_environment)

1. Create a new Firefox profile
    * Run firefox.exe -P
	* Create a profile (Name=dev)
2. Start Firefox with the new profile
3. Close Firefox
4. Create a file belgiumeid@eid.belgium.be. This file should contain the path to the your development directory where install.rdf can be found. 
5. Search the profile directory. In Windows it is something like <User Folder>\AppData\Roaming\Mozilla\Firefox\Profiles\xxxxxxx.dev
     * Windows Vista, 7        Users\<UserName>\AppData\Roaming\Mozilla\Firefox\Profiles\xxxxxxx.dev
     * Windows 2000, XP 	    Documents and Settings\<UserName>\Application Data\Mozilla\Firefox\Profiles\xxxxxxx.dev
     * Mac OS X 	            ~/Library/Application Support/Firefox/Profiles/xxxxxxx.dev
     * Linux and Unix systems 	~/.mozilla/firefox/Profiles/xxxxxxx.dev
6. Create a folder "extensions" in the profile folder
7. Copy belgiumeid@eid.belgium.be to the extensions folder

Hint: you can create a shortcut to run the development environment: "%ProgramFiles%\Mozilla Firefox\firefox.exe" -no-remote -P dev

Nice developer extensions:
 * DOM Inspector
 * Extension Developer
 * Javascript Debugger
 
Development preferences:  Change in about:config
 # javascript.options.showInConsole = true. Logs errors in chrome files to the Error Console.
 # nglayout.debug.disable_xul_cache = true. Disables the XUL cache so that changes to windows and dialogs do not require a restart. This assumes you're using directories rather than JARs. Changes to XUL overlays will still require reloading of the document overlaid.
 # browser.dom.window.dump.enabled = true. Enables the use of the dump() statement to print to the standard console. See window.dump for more info. You can also use nsIConsoleService from a privileged script.
 # javascript.options.strict = true. Enables strict JavaScript warnings in the Error Console. Note that since many people have this setting turned off when developing, you will see lots of warnings for problems with their code in addition to warnings for your own extension. You can filter those with Console2.
 # extensions.logging.enabled = true. This will send more detailed information about installation and update problems to the Error Console.

How to build
------------
All builds support Windows, Linux and Mac
* Build on Windows
  -> Required: 7zip installed. Copy C:\Program Files\7-Zip\7z.exe to C:\WINDOWS\system32 
  Run build.bat. This will produce a file belgiumeid.xpi in the same folder

* Build on Linux
  Run build.sh. This will produce a file belgiumeid.xpi in the same folder. 

File Structure
--------------
belgiumeid_extension
  belgiumeid                    source code
    content
	  -- xul and js files
    defaults
      preferences
      belgiumeid.js             platform independent configuration defaults 
    locale
      <lang>
        belgiumeid.dtd          translation file
	belgiumeid.properties   translation file
     platform
       Darwin
         defaults
           preferences
             belgiumeid.js       Mac OS X configuration defaults 
       Linux
         defaults
           preferences
             belgiumeid.js       Linux configuration defaults 
       WINNT
         defaults
           preferences
             belgiumeid.js       Windows configuration defaults 
    skin
      icon.png                  icon that appears in addon list
      overlay.css               style file
    chrome.manifest           
    install.rdf               
  belgiumeid@eid.belgium.be     file to put in extensions directory for testing
  build.bat                     Windows build script
  build.sh                      Linux build script
  readme.txt                    This file

How to create a new version
---------------------------
* Edit VERSION:
  x.y.z
* Edit belgiumeid/install.rdf
  <em:version>x.y.z</em:version>
