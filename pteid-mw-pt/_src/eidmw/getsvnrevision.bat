@echo on

svn info | FindStr /L Revision > svn_info.txt

for /f "tokens=2" %%T in (svn_info.txt) do set SVNRevision=%%T

Set /A SVNRevision+=6000

echo %SVNRevision%> svn_revision
echo.>> svn_revision

echo /* ****************************************************************************> svn_revision.h
echo  *>> svn_revision.h
echo  * eID Middleware Project.>> svn_revision.h
echo  * Copyright (C) 2008-2009 FedICT.>> svn_revision.h
echo  *>> svn_revision.h
echo  * This is free software; you can redistribute it and/or modify it>> svn_revision.h
echo  * under the terms of the GNU Lesser General Public License version>> svn_revision.h
echo  * 3.0 as published by the Free Software Foundation.>> svn_revision.h
echo  *>> svn_revision.h
echo  * This software is distributed in the hope that it will be useful,>> svn_revision.h
echo  * but WITHOUT ANY WARRANTY; without even the implied warranty of>> svn_revision.h
echo  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU>> svn_revision.h
echo  * Lesser General Public License for more details.>> svn_revision.h
echo  *>> svn_revision.h
echo  * You should have received a copy of the GNU Lesser General Public>> svn_revision.h
echo  * License along with this software; if not, see>> svn_revision.h
echo  * http://www.gnu.org/licenses/.>> svn_revision.h
echo  *>> svn_revision.h
echo **************************************************************************** */>> svn_revision.h
echo #ifndef __SVN_REVISION_H__>> svn_revision.h
echo #define __SVN_REVISION_H__>> svn_revision.h
echo.>> svn_revision.h
echo #define SVN_REVISION %SVNRevision%>> svn_revision.h
echo #define SVN_REVISION_STR "%SVNRevision%">> svn_revision.h
echo.>> svn_revision.h
echo #endif //__SVN_REVISION_H__>> svn_revision.h

echo ^<Include^>>svn_revision.wxs
echo ^<?define RevisionNumber=%SVNRevision%?^>>>svn_revision.wxs
echo ^</Include^>>>svn_revision.wxs

@echo off