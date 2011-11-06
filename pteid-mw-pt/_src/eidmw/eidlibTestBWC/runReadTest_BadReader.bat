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
copy /Y bin26\pteidlibTestBWC.exe ..\_Binaries\release\

rmdir /S /Q result
mkdir result

cd bin26
pteidlibTestBWC.exe -r -F ..\result -R test

cd ..\..\_Binaries\release
pteidlibTestBWC.exe -V -r -F ..\..\eidlibTestBWC\result -R test

cd ..\..\eidlibTestBWC