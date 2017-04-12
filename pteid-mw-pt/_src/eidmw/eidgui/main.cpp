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
#ifdef WIN32
#define _WIN32_WINNT 0x0502
#include <Windows.h>
#include <Winbase.h>
#endif

#include <iostream>
#include <cstring>

#include <QApplication>

#include "Settings.h"
#include "singleapplication.h"
#include "mainwnd.h"

#ifdef MEMORY_LEAKS_CHECK
	#ifdef WIN32
		#include <stdlib.h>
		#include <crtdbg.h>
	#endif
#endif


int main(int argc, char *argv[])
{
	bool test_mode = false;
	const char * default_sam_server = NULL;

#ifdef MEMORY_LEAKS_CHECK
	#ifdef WIN32
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
#endif

#ifdef WIN32
		SetDllDirectory("");
#endif

	int iRetValue = 0;
#ifdef WIN32
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef __APPLE__

	//In MacOS we deploy the QT plugins in a specific location which is common
	// to all the QT applications (pteidgui, ScapSignature, pteiddialogs)
	QCoreApplication::addLibraryPath(QString("/usr/local/Frameworks"));
#endif

	SingleApplication app(argc, argv);

	PTEID_InitSDK();
	PTEID_Config sam_server(PTEID_PARAM_GENERAL_SAM_SERVER);

	if (argc == 2 && strcmp(argv[1], "-test") == 0)
	{
		test_mode = true;
		default_sam_server = strdup(sam_server.getString());
		sam_server.setString("pki.teste.cartaodecidadao.pt:443");
	}
	else
	{
		//Force production mode
		sam_server.setString("pki.cartaodecidadao.pt:443");
	}

	GUISettings settings;
	QString     appPath = QCoreApplication::applicationDirPath();
	settings.setExePath(appPath);
	settings.setTestMode( test_mode );

	MainWnd widget(settings);

	//-------------------------------------------------
	// connect the messageReceived() of the instance to the 'restoreWindow()' of the
	// main application. In this way, the 'wake up' message emitted by each starting instance
	// will make the running instance to restore.
	//-------------------------------------------------
	QObject::connect(
        &app,
        &SingleApplication::instanceStarted,
        &widget,
        &MainWnd::messageRespond);

	if (!settings.getStartMinimized())
	{
		widget.show();
	}

	iRetValue = app.exec();

	if (test_mode)
	{
		sam_server.setString(default_sam_server);
		free((char *)default_sam_server);
	}

	PTEID_ReleaseSDK();

 	return iRetValue;
}

