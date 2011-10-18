/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include <QtGui/QApplication>
#include <QtPlugin>
#include <QMessageBox>
#include "ezinstaller.h"
#include "confirmdialogbox.h"
#include "accessibleDialogbox.h"

#include "Platform.h"
#ifdef WIN32
Q_IMPORT_PLUGIN(qjpeg)
#else
// Q_IMPORT_PLUGIN(QJpegPlugin) // correct name for mac *static* link
#endif

int main(int argc, char *argv[])
{
	
#ifndef WIN32
	OSStatus status;
	AuthorizationFlags flags;
	flags = kAuthorizationFlagDefaults;
	status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, 
								 flags, &ezInstaller::authRef);
	
	if (status != errAuthorizationSuccess) {
		ezInstaller::authRef = NULL;
		return false;
	}
	
	AuthorizationItem authItems = {kAuthorizationRightExecute, 0, NULL, 0};
	AuthorizationRights rights = {1, &authItems};
	flags = kAuthorizationFlagDefaults | 
	kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize | 
	kAuthorizationFlagExtendRights;
#endif
	
	QApplication a(argc, argv);

#ifndef WIN32
    a.addLibraryPath(a.applicationDirPath() + "/../Plugins");
#endif

/*     QTranslator translator;
	 //translator.load(QString(":/TranslationFiles/transFrans"));
	 translator.load(QString("eid-ezinstaller_fr"));
	 qApp->installTranslator(&translator);
*/
	ezInstaller w;
	w.show();
	w.raise();
	w.activateWindow();
	
#ifdef WIN32
	systemClass scl;

	if(!scl.IsUserAdmin())
	{
		QString _title = "Waarschuwing / Avertissement";
		QString _body = "U moet Administrator zijn om deze applicatie uit te voeren.\n\nVous devez être administrateur du sysème pour exécuter cette application.";
		QString _closeButton = "Afsluiten / Quitter";

		//if(QAccessible::isActive()){//doesn't work before a.exec is called
			accessibleDialogbox::showDialogBox(_title,_body,_closeButton,NULL,0);
		//} else {
		//	QMessageBox::question(NULL,_title,_body,_closeButton,QString::null,QString::null,0,0);	
		//}
		return 0;
	}
#else
	status = AuthorizationCopyRights (ezInstaller::authRef, &rights, kAuthorizationEmptyEnvironment, flags, NULL);
	if (status != errAuthorizationSuccess) {
		AuthorizationFree(ezInstaller::authRef,kAuthorizationFlagDefaults);
		ezInstaller::authRef = NULL;
		return 0;
	}
#endif
	
	int exitcode = a.exec();
	
#ifdef WIN32
#else
	AuthorizationFree(ezInstaller::authRef,kAuthorizationFlagDefaults);
	ezInstaller::authRef = NULL;
#endif

	return exitcode;
}
