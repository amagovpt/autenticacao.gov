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
"%~dp0IncludeFolder.exe" -u -c DoxygenCppFolder -g {823E95E2-0CFA-4375-87AA-057F18652EB2} -f "%~dp0..\..\..\_DocsExternal\c++_api\html\\" -k SOFTWARE\BEID\SDK -d DOC_EIDLIB_CPP_HTML -o "%~dp0DoxygenCppFolder.wxs" -s index.html SdkProgramMenuFolder "C++ documentation" "Documentation for the C++ API"
echo [INFO] DoxygenCppFolder.wxs created

"%~dp0IncludeFolder.exe" -u -c DoxygenDotNetFolder -g {83D68D2D-4EF6-4FB7-AA7B-D1766F3ED205} -f "%~dp0..\..\..\_DocsExternal\dotNet_api\html\\" -k SOFTWARE\BEID\SDK -d DOC_EIDLIB_DOTNET_HTML -o "%~dp0DoxygenDotNetFolder.wxs" -s index.html SdkProgramMenuFolder ".NET documentation" "Documentation for the .NET API"
echo [INFO] DoxygenDotNetFolder.wxs created

"%~dp0IncludeFolder.exe" -u -c DoxygenJavaFolder -g {AD36E7F2-B142-4B1B-8296-0A11F41CE209} -f "%~dp0..\..\..\_DocsExternal\Java_api\html\\" -k SOFTWARE\BEID\SDK -d DOC_EIDLIB_JAVA_HTML -o "%~dp0DoxygenJavaFolder.wxs" -s index.html SdkProgramMenuFolder "Java documentation" "Documentation for the java API"
echo [INFO] DoxygenJavaFolder.wxs created
