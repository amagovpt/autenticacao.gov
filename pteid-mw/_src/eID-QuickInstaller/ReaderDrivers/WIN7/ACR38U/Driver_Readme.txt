Advanced Card Systems Ltd.
http://www.acs.com.hk 
Tel: +852 2796 7873
Fax: +852 2796 1286
e-mail: info@acs.com.hk


Driver For
- ACR38 Smart Card Reader
(Manual Installation)
---------------------------------------


Contents
--------

   1. Release notes
   2. File Contents
   3. Installation Notes
   4. New Installation
      4.1. New Installation (Windows 98/Me)
      4.2. New Installation (Windows 2000)
      4.3. New Installation (Windows XP)
      4.4. New Installation (Windows Vista)
      4.5. New Installation (Windows 7)
   5. Driver Update    
      5.1. Driver Update    (Windows 98/Me)
      5.2. Driver Update    (Windows 2000)
      5.3. Driver Update    (Windows XP)
      5.4. Driver Update    (Windows Vista)
      5.5. Driver Update    (Windows 7)
   6. Check Installation
   7. Driver Removal
   8. Troubleshooting
   9. Support


1. Release notes
----------------

   This driver package supports the following operating systems:
   - Windows 98/Me
   - Windows 2000
   - Windows XP    (x86 & x64)
   - Windows Vista (x86 & x64)
   - Windows 2003  (x86 & x64)
   - Windows 2008  (x86 & x64)
   - Windows 7     (x86 & x64)

2. File Contents
----------------

   a38usb98.sys		0.9.0.8
   a38usb.cat		x.x.x.x
   a38usb.inf		x.x.x.x
   a38usb.sys		1.1.6.1
   a38usbx64.sys	1.1.6.1
   acr38svr.sys		0.9.0.8
   Usbr38.dll   	1.0.5.0
   Usbr38x64.dll   	1.0.5.0
   Release.txt  


3. Installation Notes
---------------------

   This document describes manual installation/uninstallation of the drivers for an ACS Smart Card 
   Reader. Please note that in order to install the drivers you have to be logged in as an 
   Administrator (or at least have access to an Administrator account in Windows XP or newer OS's).


4. New Installation
-------------------

4.1. New Installation (Windows 98/Me)
-------------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Plug the smart card reader into a free USB port.

   3) The 'Add New Hardware Wizard' will appear, please follow the following steps:

      a) In the first screen, click 'Next'.

      b) In the second screen, select 'Search for the best driver for your device (Recommended)' and
         click 'Next'.
 
      c) In the next screen, check 'Specify a location' and enter the location of the driver package 
         saved in step 1, or choose 'Browse' to select the location. 
         Click 'Next' to continue.

      d) In the next screen (confirming the driver), click 'Next' to start installation.
   
      e) The wizard will start to install the driver at this time. 

      f) After the installation has finished, click 'Finish' to close the wizard.

      g) The Wizard will ask to restart the PC. Click 'Yes' to do so (make sure all applications are 
         closed though and all documents are saved).
      
   4) Installation is now complete and the smart card reader is ready for use.
   
   Note: Please note that for Windows 98, the Smart Card Base Components need to be installed in 
         order for a smart card reader to be used. These components can be downloaded from 
         Microsoft.      
   

4.2. New Installation (Windows 2000)
------------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Plug the smart card reader into a free USB port.

   3) The 'Found New Hardware' wizard will appear, please follow the following steps:

     a) in the first screen, click 'Next'.

     b) In the second screen, select 'Search for a suitable driver for my device (recommended)' and
        click 'Next'.
 
     c) In the next screen, check 'Specify a location' and click 'Next'.

       Note: if is known that the driver is available through Windows Update, 
             'Microsoft Windows Update' can be checked as well. 

     d) In the dialog box that opens, enter the location of the driver package saved in step 1, 
        or choose 'Browse' to select the location. Press 'OK' to close the dialog.

     e) In the next screen (confirming the driver), click 'Next' to start installation.
   
     f) The wizard will start to install the driver at this time. 

        Note: A window may appear that the chosen driver is not digitally signed, you will be 
              asked if you want to continue the installation. Click on the 'Yes' button to continue.

     g) After the installation has finished, click 'Finish' to close the wizard.

   4) Installation is now complete and the smart card reader is ready for use.


4.3. New Installation (Windows XP)
----------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Plug the smart card reader into a free USB port.

   3) The 'Found New Hardware Wizard' will appear, please follow the following steps:

      a) In the first screen, choose 'Yes, this time only' if you know the driver is available on 
         Windows Update or choose 'No, not this time' to skip searching Windows Update. 
         Click 'Next' to continue.

      b) In the second screen, select 'Install from a list or specific location (Advanced)' and
         click 'Next'.
 
      c) In the third screen, select 'Search for the best driver in these locations'. 
         Then check 'Include this location in the search:' and enter the location of the driver 
         package saved in step 1, or choose 'Browse' to select the location. 
         Click 'Next' to continue...

      d) The wizard will start to install the driver at this time. 

         Note: A window may appear that the chosen driver is not digitally signed, you will be 
               asked if you want to continue the installation. 
               Click on the 'Continue Anyway' button to continue.

      e) After the installation has finished, click 'Finish' to close the wizard.
        
   4) Installation is now complete and the smart card reader is ready for use.
  
   Note: If after step d in the 'Found New Hardware Wizard', the driver could not be installed, 
         please follow the following steps:
  
            a) Click 'Back', the wizard will go back to step c.

            b) Choose 'Don't search, I will choose the driver to install' and click 'Next'.

            c) In the next screen, press 'Have Disk...'. In the dialog box that opens, enter the 
               location of the driver package saved in step 1, or choose 'Browse' to select the 
               location. Press 'OK' to close the dialog.

            d) Select the displayed model, and click 'Next' to start installation.

            e) The wizard will start to install the driver at this time. 

               Note: A window may appear that the chosen driver is not digitally signed, you will be 
                     asked if you want to continue the installation. 
                     Click on the 'Yes' button to continue.

            f) After the installation has finished, click 'Finish' to close the wizard.

         Installation is now complete and the smart card reader is ready for use.


4.4. New Installation (Windows Vista)
-------------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Plug the smart card reader into a free USB port.

   3) The 'Found New Hardware Wizard' will appear, please follow the following steps:

      a) In the first screen, click 'Locate and install driver software (recommended)'.

         Note 1: If User Account Control is enabled, a dialog will appear asking for permission.
                 Press 'Continue' to continue the installation.

         Note 2: By default, Windows Vista will try to find the driver on Windows Update first...
                 If this driver is available, the installation will automatically download and
                 install the driver. After installation is complete, press 'Close' to close the 
                 dialog. No steps have to be performed after this.

      b) If the driver is not available through Windows Update, a screen will appear asking for the 
         disk that came with the smart card reader. 
         Press 'I don't have the disc. Show me other options' to continue...

      c) In the next screen, choose 'Browse my computer for driver software (advanced)' to select
         the driver package.

      d) In the next screen, enter the location of the driver package saved in step 1, or choose 
         'Browse' to select the location in the dialog box that opens. 
         Then click 'Next' to continue...
 
      e) The wizard will start to install the driver at this time. 

         Note: A window may appear that the chosen driver is not digitally signed or that the driver
               has been signed by ACS, you will be asked if you want to continue the installation. 
               Click on the 'Install' button to continue.

      f) After the installation has finished, click 'Close' to close the wizard.

   4) Installation is now complete and the smart card reader is ready for use.


4.5. New Installation (Windows 7)
---------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Plug the smart card reader into a free USB port.

   3) By default, Windows 7 will try to find the driver on Windows Update first...
      If this driver is available, the installation will automatically download and
      install the driver. After installation is complete, press 'Close' to close the 
      dialog. No steps have to be performed after this.

      If the driver is not available, the installation will fail and a window will show 
      stating 'No driver found'. 
      In that case please follow the steps under 5.5. Driver Update (windows 7)!

   4) Installation is now complete and the smart card reader is ready for use.


5. Driver Update
----------------

5.1. Driver Update (Windows 98/Me)
----------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Make sure the smart card reader is plugged into a USB port.

   3) Open the Device Manager as follows:
   
      a) Press 'Start', choose 'Settings' and 'Control Panel' in the start menu.
	  
	    b) In the dialog that opens, choose 'System'.
   
      c) In the dialog that opens, choose the tab page 'Device Manager'.

      d) In the treeview that opens, choose 'Smart card readers', click the '+' sign to display
	       the installed smart card readers.

      e) The smart card reader to update should be present here.

   4) Rightclick on the smart card reader to update and choose 'Properties'.
   
   5) In the dialog that opens, select the tab-page called 'Driver' and click 'Update Driver...'.

   6) The 'Update Device Driver Wizard' will appear, please follow the following steps:
        
      a) In the first screen, click 'Next' to continue.
	   
      b) In the second screen, select 'Display a list of all the drivers in a specisif location, so 
	       you can select the driver you want.' and click 'Next'.

      c) In the next screen, press 'Have Disk...'. In the dialog box that opens, enter the location 
         of the driver package saved in step 1, or choose 'Browse' to select the location. 
         Press 'OK' to close the dialog.

      d) Select the displayed model, and click 'Next'.
	  
	    e) The next screen will show a confirmation, click 'Next' to start installation.

      f) The wizard will start to install the driver at this time. 

      g) After the installation has finished, click 'Finish' to close the wizard.

         Note: The wizard will ask you to restart the computer in order to use the new driver. 
               Click 'Yes' to do so (make sure all applications are closed though and all documents 
               are saved).
                           
   7) The driver update is now complete and the smart card reader is ready for use.


5.2. Driver Update (Windows 2000)
---------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Make sure the smart card reader is plugged into a USB port.

   3) Open the Device Manager as follows:
   
      a) Press 'Start', choose 'Settings' and 'Control Panel' in the start menu.
	  
	    b) In the dialog that opens, choose 'Administrative Tools', then 'Computer Management'.
   
      c) In the dialog that opens, choose 'Device Manager' in the menu on the left.

      d) In the treeview that opens, choose 'Smart card readers', click the '+' sign to display
	       the installed smart card readers.

      e) The smart card reader to update should be present here.

   4) Rightclick on the smart card reader to update and choose 'Properties'.
   
   5) In the dialog that opens, select the tab-page called 'Driver' and click 'Update Driver...'.

   6) The 'Upgrade Device Driver Wizard' will appear, please follow the following steps:
        
      a) In the first screen, click 'Next' to continue.
	   
      b) In the second screen, select 'Display a list of the known drivers for this device so that 
	       I can choose a specific driver' and click 'Next'.

      c) In the next screen, press 'Have Disk...'. In the dialog box that opens, enter the location 
         of the driver package saved in step 1, or choose 'Browse' to select the location. 
         Press 'OK' to close the dialog.

      d) Select the displayed model, and click 'Next'.
	  
	    e) The next screen will show a confirmation, click 'Next' to start installation.

      f) The wizard will start to install the driver at this time. 

         Note: A window may appear that the chosen driver is not digitally signed, you will be 
               asked if you want to continue the installation. 
			   Click on the 'Yes' button to continue.

      g) After the installation has finished, click 'Finish' to close the wizard.
            
   7) The driver update is now complete and the smart card reader is ready for use.
   
   Note: The wizard might ask you to restart the computer in order to use the new driver. 
         Click 'Yes' to do so (make sure all applications are closed though and all documents 
         are saved).    


5.3. Driver Update (Windows XP)
-------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Make sure the smart card reader is plugged into a USB port.

   3) Open the Device Manager as follows:
   
      a) Press 'Start', and choose 'Control Panel' in the start menu.
	  
	    b) In the dialog that opens:
         - If Category View is enabled; choose 'Performance and Maintenance', in the view that 
           opens, choose 'Administrative Tools', then 'Computer Management'.
         - If Classic View is enabled; choose 'Administrative Tools', then 'Computer Management'.
         
      c) In the dialog that opens, choose 'Device Manager' in the menu on the left.

      d) In the treeview that opens, choose 'Smart card readers', click the '+' sign to display
	       the installed smart card readers.

      e) The smart card reader to update should be present here.

   4) Rightclick on the smart card reader and choose 'Update Driver...'.

   5) The 'Hardware Update Wizard' will appear, please follow the following steps:
         
      a) In the first screen, choose 'No, not this time' to skip searching Windows Update. 
         click 'Next' to continue.      

      b) In the second screen, select 'Install from a list or specific location (Advanced)' and
         click 'Next'.

      c) Choose 'Don't search, I will choose the driver to install' and press 'Next'.

      d) In the next screen, press 'Have Disk...'. In the dialog box that opens, enter the location 
         of the driver package saved in step 1, or choose 'Browse' to select the location. 
         Press 'OK' to close the dialog.

      e) Select the displayed model, and click 'Next' to start installation.

      f) The wizard will start to install the driver at this time. 

         Note: A window may appear that the chosen driver is not digitally signed, you will be 
               asked if you want to continue the installation. Click on the 'Yes' button to continue.

      g) After the installation has finished, click 'Finish' to close the wizard.
            
   6) The driver update is now complete and the smart card reader is ready for use.

   Note: The wizard might ask you to restart the computer in order to use the new driver. 
         Click 'Yes' to do so (make sure all applications are closed though and all documents 
         are saved). 


5.4. Driver Update (Windows Vista)
----------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Make sure the smart card reader is plugged into a USB port.

   3) Open the Device Manager as follows:
   
      a) Press 'Start' and rightclick on 'Computer', in the menu that opens, choose 'Manage'
   
         Note: If User Account Control is enabled, a dialog will appear asking for 
               permission. Press 'Continue' to continue...

      b) In the dialog that opens, choose 'Device Manager' in the menu on the left.

      c) In the treeview that opens, choose 'Smart card readers'.

      d) The smart card reader should be present here.

   4) Rightclick on the smart card reader and choose 'Update Driver Software'.

   5) The 'Update Driver Software Wizard' will appear, please follow the following steps:
         
      a) In the first screen, choose 'Browse my computer for driver software'.

      b) In the next screen, choose 'Let me pick from a list of device drivers on my computer'.
   
      c) In the next screen, press 'Have Disk...'. In the dialog box that opens, enter the location 
         of the driver package saved in step 1, or choose 'Browse' to select the location. 
         Press 'OK' to close the dialog.

      d) Select the displayed model, and click 'Next' to start installation.

      e) The wizard will start to install the driver at this time. 

         Note: A window may appear that the chosen driver is not digitally signed, you will be 
               asked if you want to continue the installation. 
               Click on the 'Yes' button to continue.

      f) After the installation has finished, choose 'Finish' to close the wizard.

   6) The driver update is now complete and the smart card reader is ready for use.

   Note: The wizard might ask you to restart the computer in order to use the new driver. 
         Click 'Yes' to do so (make sure all applications are closed though and all documents 
         are saved). 
         

5.5. Driver Update (Windows 7)
----------------------------------

   1) Unzip the package to an easy to find location (e.g. the desktop).

   2) Make sure the smart card reader is plugged into a USB port.

   3) Open the Device Manager as follows:
   
      a) Press 'Start' and rightclick on 'Computer', in the menu that opens, choose 'Manage'
   
         Note: If User Account Control is enabled, a dialog will appear asking for 
               permission. Press 'Continue' to continue...

      b) In the dialog that opens, choose 'Device Manager' in the menu on the left.

      c) In the treeview that opens, choose 'Smart card readers'.
    
         Note: If this is an 1st installation where the drivers could not be downloaded from 
               Windows Update, the reader is listed under 'Other devices'

      d) The smart card reader should be present here.

   4) Rightclick on the smart card reader and choose 'Update Driver Software'.

   5) The 'Update Driver Software Wizard' will appear, please follow the following steps:
         
      a) In the first screen, choose 'Browse my computer for driver software'.

      b) In the next screen, choose 'Let me pick from a list of device drivers on my computer'.
    
         Note: If a list of device types is shown in the next screen, choose 'Smart card readers' from the 
               list and click 'Next'.
   
      c) In the next screen, press 'Have Disk...'. In the dialog box that opens, enter the location 
         of the driver package saved in step 1, or choose 'Browse' to select the location. 
         Press 'OK' to close the dialog.

      d) Select the displayed model, and click 'Next' to start installation.

      e) The wizard will start to install the driver at this time. 

         Note: A window may appear that the chosen driver is not digitally signed or that the driver
               has been signed by ACS, you will be asked if you want to continue the installation. 
               Click on the 'Install' button to continue.

      f) After the installation has finished, choose 'Finish' to close the wizard.

   6) The driver update is now complete and the smart card reader is ready for use.

   Note: The wizard might ask you to restart the computer in order to use the new driver. 
         Click 'Yes' to do so (make sure all applications are closed though and all documents 
         are saved). 
         
6. Check Installation
---------------------

   In order to check if the drivers have been installed succesfully, please go to our website: 
   http://www.acs.com.hk and download the ACS Quickview tool from the 'Downloads' section as 
   follows:

   1) Go to http://www.acs.com.hk/card-utility-tools.php.
   
   2) Click the 'ACS Diagnostic Tool Quickview' link and download the zip-file to an easy to find 
      location (e.g. the desktop), then unzip the contents.

   3) Start 'QuickView.exe' to start the tool.

   4) In the program, press 'Initialize' to get a list of installed readers, 
      the just installed smart card reader should be listed in the list on the left in the program.
     
7. Driver Removal
-----------------

   1) Make sure the smart card reader is plugged into a USB port.
   
   2) Open the Device Manager (for instructions please follow any of the instructions above). 

   3) In the treeview in the Device Manager, choose 'Smart card readers'.

   4) The installed smart card reader should be present here.

   5) Rightclick on the smart card reader and choose 'Uninstall'.
   
   6) A dialog will open to confirm the device removal, click 'OK' to proceed.
   
      Note: Under Vista or Windows 7, when shown, it is recommended to check the 'Delete the driver software for 
      this device' checkbox as well.  
   
   7) The smart card reader driver has been uninstalled, the reader can be unplugged from the USB
      port.
      

8. Troubleshooting
------------------

   If the reader is not listed in the ACS QuickView tool, please follow the steps below to check 
   the installation of the smart card reader.

   1) Re-plugin the reader and see if the 'New Hardware Found' wizard is starting.
      If it does, please follow the instructions above.

   2) If the reader is still not listed in the QuickView program, please follow the following steps:

      - Open the Device Manager (for instructions please follow any of the instructions above). 

      - In the treeview in the Device Manager, choose 'Smart card readers'.

      - The just installed smart card reader should be present here.

      - If the smart card reader is present in the Device Manager, but it has an exclamation mark,
        please follow the instructions to update the driver as mentioned above.

   3) Check if the Smartcard Service is running as follows:

      - Windows 98: 

        Please note that for Windows 98, the Smart Card Base Components need to be installed in 
        order for a smart card reader to be used. These components can be downloaded from 
        Microsoft.      
           
        To check if the base components are present, and if the smart card service is running, 
        please follow the following steps:
        
        a) Press 'Start', choose 'Programs', 'Accessories',  'System Tools', then 'System
           Information'.
        
        b) In the dialog that opens, choose and expand 'Software Environment', then select 'Running
           Tasks'.
           
        c) Check if Scardsvr.exe is listed.
        
        If scardsvr.exe is not running, please check if the 'scardsvr.exe' is present in the 
        'c:\windows\system' folder. If not, then the smart card base components need to be 
        installed. Please download these components from Microsoft (search for 'Windows 98 Smart 
        Card Base Components'). Alternatively download any driver installer from the ACS website 
        and install the drivers. This will also install the Smart Card Base Components.
        
      - Other Operating Systems:
      
        Press 'Start', then 'Run', type 'services.msc'.
   
        Note: For Vista/Windows 7; if User Account Control is enabled, a dialog will appear asking
              for permission. Press 'Continue' to continue...
 
        In the dialog opens, find the 'Smart Card' entry in the list of services.
   
        The status should be 'Started' and the Startup Type should be 'Automatic'. If this is not 
        the case, please doubleclick on the entry to change the properties and start the service.

   4) Close all applications, reboot the PC with the reader plugged in.
   
   If the smart card reader is still not listed in the QuickView program after any of above steps,
   please save the log and contact ACS using the means mentioned below.


9. Support
----------

   In case of problems, please contact ACS through:

   website:    http://www.acs.com.hk/
   email:      info@acs.com.hk


-----------------------------

Copyright 
---------

   Copyright by Advanced Card Systems Ltd. (ACS) No part of this reference manual may be reproduced 
   or transmitted in any from without the expressed, written permission of ACS. 

Notice 
------
   Due to rapid change in technology, some of specifications mentioned in this publication are 
   subject to change without notice. Information furnished is believed to be accurate and reliable.
   ACS assumes no responsibility for any errors or omissions, which may appear in this document.

-----------------------------
Version      : 1.1 
Last modified: July 2009 