This tool is designed to remove all pteid middleware versions.

Command line
------------
The command line accept the following parameters
  --help or -h              : print help in a dialog
  --log or -l <logfile>     : log file (default value is pteidcleanup.log)
  --timeout or -t <timeout> : set a timeout (in second) for uninstall execution (default value is 5 mins).
  --keep or -k <guid>       : keep the guid version.
	                      To use when unsinstalling msi. It kills processes but it doesn't mix with msi tasks.
  --force or -f             : force remove
		              (kill application that used the middleware to insure uninstall achievment).
  --quiet or -q             : quiet cleanup (does not show dialog if failed)


Return code
-----------
The application may return the following codes:
  -   0 : OK
  -   1 : OK but a reboot is needed.
  -  11 : Bad parameter in command line.
  -  12 : User must be administrator.
  -  13 : Some files are locked.
  -  14 : Could not stop a service.
  -  15 : Could not remove a service.
  -  16 : Uninstall execution failed.
  -  17 : Uninstall execution timeout.
  -  18 : Registry key could not be deleted.
  -  19 : File could not be deleted.
  - 100 : Other internal error (see log file).


Operations
----------
The tools execute the following opertations in that order

  - Stop and remove these services (if found)
      * BELGIUM_ID_CARD_SERVICE
      * eID Privacy Service
      * pteidPrivacyFilter
      * eID CRL Service

  - Kill these processes(if found)
      * pteidgui.exe
      * pteid35gui.exe
      * pteidsystemtray.exe
      * xsign.exe
      * pteid35xsign.exe

  - Check if one of these library is still locked (in used by some processes).
    If the "force parameter" is used, the using processes are killed. 
      * pteid35common.dll
      * pteidcommon.dll
      * pteidwinscard.dll
      * pteidcsp.dll
      * BELGIUM IDENTITY CARD CSP.DLL
      * BELPIC.DLL

  - Uninstall these GUID product
    (If after a successful uninstall, the regstry key remains, 
     this operation remove it - known case with bad unsinstall of the 2.3) 
      * {44CFED0B-BF92-455B-94D3-FA967A81712E} = version 2.3
      * {BA42ACEA-3782-4CAD-AA10-EBC2FA14BB7E} = version 2.4
      * {85D158F2-648C-486A-9ECC-C5D4F4ACC965} = version 2.5
      * {EA248851-A7D5-4906-8C46-A3CA267F6A24} = version 2.6
      * {82493A8F-7125-4EAD-8B6D-E9EA889ECD6A} = version 3.0
      * {824563DE-75AD-4166-9DC0-B6482F2DED5A} = version 3.5 msi
      * {40420E84-2E4C-46B2-942C-F1249E40FDCB} = version 3.5 IS
      * {FBB5D096-1158-4e5e-8EA3-C73B3F30780A} = version 3.5 Pro msi
      * {4C2FBD23-962C-450A-A578-7556BC79B8B2} = version 3.5 Pro IS

  - The following file are remove from System32 folder 
    But only if the force parameter is used and we keep no version is kept (-k parameter not used)
      * Portugal Identity Card CSP.dll
      * Portugal Identity Card PKCS11.dll
      * Belpic PCSC Service.exe
      * belpic.dll
      * eidlib.dll
      * EIDLibCtrl.dll
      * eidlibj.dll
      * eidlibj.dll.manifest
      * eid_libeay32.dll
      * eid_ssleay32.dll
      * pinpad_emulator.dll

      * BelgianEID.cfg
      * belpicgui.dll
      * shbelpicgui.exe
		
      * pteidcsp.conf
      * pteidcsp.dll
      * pteidgui.dll
      * pteidlib.dll
      * pteidlibaxctrl.dll
      * pteidlibeay32.dll
      * pteidlibjni.dll
      * pteidlibjni.dll.manifest
      * pteidlibopensc.dll
      * pteidpkcs11.dll
      * pteidservicecrl.exe
      * pteidservicepcsc.exe
      * pteidssleay32.dll
      * pteidwinscard.dll
      * shpteidgui.exe

      * pteidapplayer.dll
      * pteidcardlayer.dll
      * pteidcommon.dll
      * pteidCSPlib.dll
      * pteidDlgsWin32.dll
      * pteidlibJava_Wrapper.dll

      * pteid35applayer.dll
      * pteid35cardlayer.dll
      * pteid35common.dll
      * pteid35DlgsWin32.dll

  - The following file are remove from Application folder (C:\Program Files\Portugal Identity Card)
    But only if the force parameter is used and we keep no version is kept (-k parameter not used)

      * pteidgui.exe
      * pteidsystemtray.exe

      * pteid35gui.exe
      * pteid35libCpp.dll
      * eid.ico
      * pteidoutlooksnc.exe
      * pteid-pkcs11-register.html
      * pteid-pkcs11-unregister.html
      * eidmw_en.qm
      * eidmw_nl.qm
      * eidmw_fr.qm
      * eidmw_de.qm
      * License_en.rtf
      * License_nl.rtf
      * License_fr.rtf
      * License_de.rtf
      * THIRDPARTY_LICENSES.txt
      * QtCore4.dll
      * QtGui4.dll
      * imageformats\qjpeg4.dll
      * pteidlib.jar
      * eidlib.jar
      * PTEID_old.html
      * XAdESLib.dll
      * pteidlibC.dll
      * x509ac.dll
      * XalanC_1_10.dll
      * XalanMessages_1_10.dll
      * xercesc_2_7.dll
      * xsec_1_4_0.dll
      * libeay32.dll


If one of these operations failed, the program ends and return an error code.
The log file contains information about all these operations and theri result.
