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
#include "qtsingleapplication.h"

#include "Settings.h"
#include "mainwnd.h"

#ifdef MEMORY_LEAKS_CHECK
	#ifdef WIN32
		#include <stdlib.h>
		#include <crtdbg.h>
	#endif
#endif

class MyApplication : public QtSingleApplication
{
public:
	MyApplication( const char* name, int &argc, char ** argv ) :
	  QtSingleApplication( name, argc, argv )
	  {
	  }
#ifdef WIN32
	  //--------------------------------------------
	  // install an event filter and post a message to the application
	  // that it should shut down
	  //--------------------------------------------
	  bool winEventFilter(MSG * msg, long * retVal)
	  {
		  if (msg->message == WM_QUERYENDSESSION )
		  {
			  QuitEvent* quitEvent = new QuitEvent(msg->message);
			  QCoreApplication::postEvent((MainWnd*)activationWindow(),quitEvent);
			  *retVal = true;	//--> indicate that app can be closed
			  return true;		//--> do not let Qt handle the message
		  }
		  return false;
	  }
#elif __MACH__
    bool event(QEvent *event)
    {
		QString openFile = "Open File";
        switch (event->type()) {
			case QEvent::FileOpen:
				//loadFile(static_cast<QFileOpenEvent *>(event)->file());  
				openFile.append(static_cast<QFileOpenEvent *>(event)->file()); 
				sendMessage( (const QString)openFile );
				sendMessage("Restore Windows");
				return true;
			default:
				return QApplication::event(event);
        }
    }
	
#endif
};


int main(int argc, char *argv[])
{
#ifdef MEMORY_LEAKS_CHECK
	#ifdef WIN32
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
#endif

#ifdef WIN32
		SetDllDirectory("");
#endif

	int iRetValue = 0;

	if (argc == 2 && strcmp(argv[1],"/startup")==0)
	{
		PTEID_Config StartupParam(PTEID_PARAM_GUITOOL_STARTWIN);
		if(!StartupParam.getLong())
		{
			PTEID_ReleaseSDK();
			return 0;
		}
	}

	MyApplication instance("eidgui", argc, argv);
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Waking up other instance");
	if (instance.sendMessage("Wake up!"))
	{
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Wake up responding OK");
		instance.sendMessage("Restore Windows");
#ifndef __MACH__
		if ( (argc >= 2) && (strcmp(argv[1],"/startup")!=0) && (argv[1] != NULL) )
		{
			PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "argc = %i argv[1] = %s",argc,argv[1]);
			QString openFile = "Open File";
			openFile.append(argv[1]);
			instance.sendMessage (openFile);
		}
#endif
		return 0;
	}

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Wake up did not respond");
	instance.initialize();

	PTEID_InitSDK();

	GUISettings settings;
	QString     appPath = QCoreApplication::applicationDirPath();
	settings.setExePath(appPath);


	MainWnd widget(settings);

	//-------------------------------------------------
	// connect the messageReceived() of the instance to the 'restoreWindow()' of the
	// main application. In this way, the 'wake up' message emitted by each starting instance
	// will make the running instance to restore.
	//-------------------------------------------------
	QObject::connect(&instance, SIGNAL(messageReceived(const QString&)),
		&widget, SLOT(messageRespond(const QString&)));

	
#ifndef __MACH__	
	if ( (argc >= 2) && (strcmp(argv[1],"/startup")!=0) && (argv[1] != NULL) )
	{
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "argc = %i argv[1] = %s",argc,argv[1]);
		QString openFile = "Open File";
		openFile.append(argv[1]);
		instance.sendMessage( (const QString)openFile );
	}
#endif
	if (!settings.getStartMinimized())
	{
		widget.show();
	}

	instance.setActivationWindow ( &widget );

	iRetValue = instance.exec();

	PTEID_ReleaseSDK();

 	return iRetValue;
}

