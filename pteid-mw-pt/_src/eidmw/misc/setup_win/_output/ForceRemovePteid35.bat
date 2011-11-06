:: ****************************************************************************
:: 
::  * eID Middleware Project.
::  * Copyright (C) 2008-2009 FedICT.
::  *
::  * This is free software; you can redistribute it and/or modify it
::  * under the terms of the GNU Lesser General Public License version
::  * 3.0 as published by the Free Software Foundation.
::  *
::  * This software is distributed in the hope that it will be useful,
::  * but WITHOUT ANY WARRANTY; without even the implied warranty of
::  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
::  * Lesser General Public License for more details.
::  *
::  * You should have received a copy of the GNU Lesser General Public
::  * License along with this software; if not, see
::  * http://www.gnu.org/licenses/.
:: 
:: ****************************************************************************
Taskkill /F /IM pteid35gui.exe
Taskkill /F /IM pteid35xsign.exe

rmdir /S /Q "C:\Program Files\InstallShield Installation Information\{40420E84-2E4C-46B2-942C-F1249E40FDCB}"
rmdir /S /Q "C:\Program Files\InstallShield Installation Information\{4C2FBD23-962C-450A-A578-7556BC79B8B2}"

rmdir /S /Q "C:\Program Files\Portugal Identity Card"

regedit /s ForceRemovePteid35.reg

del C:\WINDOWS\system32\EIDLibCtrl.dll
del C:\WINDOWS\system32\qt-mt334.dll
del C:\WINDOWS\system32\eidlib.dll
del C:\WINDOWS\pteidgui.conf
del C:\WINDOWS\system32\eidlibj.dll
del C:\WINDOWS\system32\eidlibj.dll.manifest
del C:\WINDOWS\system32\xerces-c_2_8.dll
del C:\WINDOWS\system32\libeay32_0_9_8g.dll
del C:\WINDOWS\system32\ssleay32_0_9_8g.dll
del C:\WINDOWS\Belgian_eID_PKCS11_java.cfg
del C:\WINDOWS\system32\pteidlib.dll
del C:\WINDOWS\system32\pteidwinscard.dll
del C:\WINDOWS\system32\pteidlibopensc.dll
del C:\WINDOWS\system32\pteidlibaxctrl.dll
del C:\WINDOWS\system32\pteidlibeay32.dll
del "C:\WINDOWS\system32\Portugal Identity Card PKCS11.dll"
del C:\WINDOWS\system32\pteidgui.dll
del C:\WINDOWS\system32\pteidssleay32.dll
del C:\WINDOWS\system32\pteidlibjni.dll
del C:\WINDOWS\system32\pteidlibjni.dll.manifest
del C:\WINDOWS\system32\pteid35DlgsWin32.dll
del C:\WINDOWS\system32\pteid35applayer.dll
del C:\WINDOWS\system32\pteid35cardlayer.dll
del C:\WINDOWS\system32\pteid35common.dll
del C:\WINDOWS\system32\pteidCSP.dll
del C:\WINDOWS\system32\pteidCSPlib.dll
del C:\WINDOWS\system32\pteidpkcs11.dll

