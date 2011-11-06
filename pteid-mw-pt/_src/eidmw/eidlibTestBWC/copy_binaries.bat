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
copy /Y "..\..\ThirdParty\pteid_sdk\2.6\bin\*.dll" bin26\
copy /Y "..\..\ThirdParty\pteid_sdk\2.6\bin\pteidgui.exe" bin26\
copy /Y "..\..\ThirdParty\pteid_sdk\2.6\bin\pteidgui.conf" C:\windows\
copy /Y "..\..\ThirdParty\pteid_sdk\2.6\bin\eidstore\certs\*.der" bin26\eidstore\certs\
copy /Y "..\..\ThirdParty\Qt\3.3.4\bin\qt-mt334.dll" bin26\
