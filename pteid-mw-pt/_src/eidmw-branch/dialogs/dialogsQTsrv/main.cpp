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
#include <QtGui>
#include <QtCore>
#include <QPushButton>
#include <QMessageBox>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <signal.h>

#include "../dialogs.h"
#include "../langUtil.h"
#include "dlgWndAskPIN.h"
#include "dlgWndAskPINs.h"
#include "dlgWndBadPIN.h"
#include "dlgWndPinpadInfo.h"
#include "SharedMem.h"
#include "errno.h"

#include "../../common/Thread.h"
#include "../../common/MWException.h"
#include "../../common/eidErrors.h"
#include "../../common/Log.h"
#include "../../common/Util.h"
#include "../../common/Config.h"

std::string readableFilePath = "/usr/local/etc/pteidgui.conf";

DlgDisplayPinpadInfoArguments *oInfoData = NULL;
dlgWndPinpadInfo *dlgInfo = NULL;
QDialog *dlg = NULL;

pid_t getPidFromParentid(pid_t parentid, const char *CommandLineToFind);

using namespace eIDMW;

void sigint_handler(int sig)
{
  // this is needed because after the first call
  // to this function, the handler is reset to the
  // unix default
  signal(SIGINT,sigint_handler);

  // respond to the signal : clean up the dialogs,
  // the shared memory and the random file
  if (dlgInfo)
    delete dlgInfo;
  dlgInfo = NULL;
  if (dlg)
    delete dlg;
  dlg = NULL;
}

int g_UseKeyPad = -1;

bool DlgGetKeyPad()
{
	if(g_UseKeyPad==-1)
	{
		g_UseKeyPad = (CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD)?1:0);
	}
	return g_UseKeyPad;
}

/*
 * Translate Pin names to english from their native labels
 * present on the in-card objects
 */

QString getPinName( DlgPinUsage usage, wchar_t *inPinName ){
    QString PinName;

    switch( usage ){
        case DLG_PIN_AUTH:
            PinName = GETQSTRING_DLG(AuthenticationPin);
            break;

        case DLG_PIN_SIGN:
            PinName = GETQSTRING_DLG(SignaturePin);
            break;

        case DLG_PIN_ADDRESS:
            PinName = GETQSTRING_DLG(AddressPin);
            break;

        DLG_PIN_UNKNOWN:
            if ( inPinName == NULL ){
                PinName = GETQSTRING_DLG(UnknownPin);
            }else{
                if( wcscmp( inPinName , L"" ) == 0 ){
                    PinName = GETQSTRING_DLG(UnknownPin);
                } else{
                    PinName = QString::fromWCharArray( inPinName );
                }/* if( wcscmp( inPinName , L"" ) == 0 ) */
            }/* if ( inPinName == NULL ) */
            break;

        default:
            if ( inPinName == NULL ){
                PinName = GETQSTRING_DLG(UnknownPin);
            }else{
                if ( wcscmp( inPinName, L"") == 0 ){
                    PinName = GETQSTRING_DLG(Pin);
                } else {
                    PinName = QString::fromWCharArray( inPinName );
                }/* if ( wcscmp( inPinName, L"") == 0 ) */
            }/* if ( inPinName == NULL ) */
            break;
    }/* switch( usage ) */

    return PinName;
}/* getPinName() */

int main(int argc, char *argv[])
{

	void sigint_handler(int sig); /* prototype */

	if(signal(SIGINT,sigint_handler) == SIG_ERR)
	{
		MWLOG(LEV_ERROR, MOD_DLG, L"  %s setup of signal handler : %s ", argv[0], strerror(errno) );
		exit(DLG_ERR);
	}

	int iRet = DLG_CANCEL;

	// parse the arguments according to the operation requested
	MWLOG(LEV_INFO, MOD_DLG, L"  Running %s ...",argv[0]);

	if(argc > 2 )
	{
		int iFunctionIndex = atoi(argv[1]);
		std::string readableFilePath = argv[2];

        Type_WndGeometry parentWndGeometry;
	if (argc > 5 ) {
            parentWndGeometry.x = atoi(argv[3]);
            parentWndGeometry.y = atoi(argv[4]);
            parentWndGeometry.width = atoi(argv[5]);
            parentWndGeometry.height = atoi(argv[6]);
	}

		#ifdef __APPLE__
		// In MacOS we deploy the QT plugins in a specific location which is common
		// to all the QT applications (pteidgui, ScapSignature, pteiddialogs)
		QCoreApplication::addLibraryPath(QString("/usr/local/Frameworks"));
		#endif

		if(iFunctionIndex == DLG_ASK_PIN)
		{
			QApplication a(argc, argv);

			// attach to the segment and get a pointer
			DlgAskPINArguments *oData = NULL;

			SharedMem oShMemory;
			oShMemory.Attach( sizeof(DlgAskPINArguments), readableFilePath.c_str(),(void **) &oData);
			MWLOG(LEV_DEBUG, MOD_DLG, L"Running DLG_ASK_PIN with args: operation=> %d usage=> %d\n", oData->operation, oData->usage);

			// do something
			dlgWndAskPIN *dlg = NULL;
			try
			{
				QString PINName = getPinName( oData->usage, oData->pinName );
				QString Header;
				switch( oData->operation )
				{
					case DLG_PIN_OP_VERIFY:
						switch( oData->usage )
						{
							case DLG_PIN_AUTH:
                                Header = GETQSTRING_DLG(PleaseEnterYourPin);
								Header += ", ";
								Header += GETQSTRING_DLG(InOrderToAuthenticateYourself);
								Header += "\n";
								break;
							case DLG_PIN_SIGN:
								Header = GETQSTRING_DLG(Caution);
								Header += " ";
								Header += GETQSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);
								Header += "\n";
								break;
							default:
								Header = GETQSTRING_DLG(PleaseEnterYourPin);
								Header += "\n";
								break;
						}
						break;
					case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
						Header = GETQSTRING_DLG(PleaseEnterYourPuk);
						Header += ", ";
						Header = GETQSTRING_DLG(ToUnblock);
						Header += " ";
						Header = GETQSTRING_DLG(Your);
						Header += " \"";
						if( wcslen(oData->pinName)==0 )
						{
							Header += QString::fromWCharArray(oData->pinName);
						}
						else
						{
							Header += GETQSTRING_DLG(Pin);
						}
						Header += "\"\n";
						break;
					default:
						oData->returnValue = DLG_BAD_PARAM;
						oShMemory.Detach((void *)oData);
						return 0;
						break;
				}

				dlg = new dlgWndAskPIN(
									   oData->pinInfo,
									   oData->usage,
									   Header,
									   PINName,
									   DlgGetKeyPad(),
                                       0, &parentWndGeometry );
				int retVal = dlg->exec();
				if( retVal == QDialog::Accepted )
				{
					wcscpy_s(oData->pin, sizeof(oData->pin)/sizeof(wchar_t), dlg->getPIN().c_str());
					oData->returnValue = DLG_OK;
				}
				else	// we'll consider as cancel
				{
					oData->returnValue = DLG_CANCEL;
				}
				delete dlg;
				dlg = NULL;
				oShMemory.Detach((void *)oData);
				return 0;
			}
			catch( ... )
			{
				if( dlg )  delete dlg;
				oData->returnValue = DLG_ERR;
				oShMemory.Detach((void *)oData);
				return 0;
			}

		}
		else if (iFunctionIndex == DLG_ASK_PINS )
		{
			QApplication a(argc, argv);
			// attach to the segment and get a pointer
			DlgAskPINsArguments *oData = NULL;
			SharedMem oShMemory;
			oShMemory.Attach( sizeof(DlgAskPINsArguments), readableFilePath.c_str(),(void **) &oData);
			MWLOG(LEV_ERROR, MOD_DLG, L"Running DLG_ASK_PINS with args: operation=> %d usage=> %d\n", oData->operation, oData->usage);

			dlgWndAskPINs *dlg = NULL;
			try
			{
				QString PINName;
				QString Header;
				Header = GETQSTRING_DLG(EnterYour);
				Header += " ";

				switch( oData->operation )
				{
					case DLG_PIN_OP_CHANGE:
						if( oData->usage == DLG_PIN_UNKNOWN )
						{
							PINName = QString::fromWCharArray(oData->pinName);
						}
						else
						{
							PINName = GETQSTRING_DLG(Pin);
						}
						break;
					case DLG_PIN_OP_UNBLOCK_CHANGE:
						Header = GETQSTRING_DLG(PleaseEnterYourPuk);
						Header += ", ";
						Header = GETQSTRING_DLG(ToUnblock);
						Header += " ";
						Header = GETQSTRING_DLG(Your);
						Header += " \"";

					//	if( oData->usage == DLG_PIN_UNKNOWN )
					//	{
				    //			PINName = QString::fromWCharArray(oData->pinName);
					//	}
					//	else
					//	{
							PINName = GETQSTRING_DLG(Puk);
					//	}
						break;
					default:
						oData->returnValue = DLG_BAD_PARAM;
						oShMemory.Detach((void *)oData);
						return 0;
						break;
				}

                PINName = getPinName( oData->usage, oData->pinName );
                Header += PINName;

				Header += " ";
				QString tr_pin;

				if (oData->operation == DLG_PIN_OP_UNBLOCK_CHANGE)
				{
					Header = GETQSTRING_DLG(UnlockDialogHeader);
					tr_pin = QString("PUK");
				}
				else
				{
					tr_pin = PINName;
				}

				dlg = new dlgWndAskPINs(oData->pin1Info,
						oData->pin2Info,
						Header,
						tr_pin,
						DlgGetKeyPad(),
						0, &parentWndGeometry );
				if( dlg->exec() )
				{
				        wcscpy_s(oData->pin1, sizeof(oData->pin1)/sizeof(wchar_t), dlg->getPIN1().c_str());
					wcscpy_s(oData->pin2, sizeof(oData->pin2)/sizeof(wchar_t), dlg->getPIN2().c_str());
					delete dlg;
					dlg = NULL;
					oData->returnValue = DLG_OK;
					oShMemory.Detach((void *)oData);
					return 0;
				}
				delete dlg;
				dlg = NULL;
			}
			catch( ... )
			{
				if( dlg ) delete dlg;
				oData->returnValue = DLG_ERR;
				oShMemory.Detach((void *)oData);
				return 0;
			}
			oData->returnValue = DLG_CANCEL;
			oShMemory.Detach((void *)oData);
			return 0;

		}
		else if (iFunctionIndex == DLG_BAD_PIN)
		{
			QApplication a(argc, argv);

			// attach to the segment and get a pointer
			DlgBadPinArguments *oData = NULL;
			SharedMem oShMemory;
			oShMemory.Attach( sizeof(DlgBadPinArguments), readableFilePath.c_str(),(void **) &oData);

			dlgWndBadPIN *dlg = NULL;
			try
			{
				QString PINName;
				PINName = getPinName( oData->usage, oData->pinName );
				dlg = new dlgWndBadPIN( PINName
                                        , oData->ulRemainingTries
                                        , 0
                                        , &parentWndGeometry );
				if( dlg->exec() )
				{
					delete dlg;
					dlg = NULL;

					eIDMW::DlgRet dlgResult = DLG_RETRY;
					if( oData->ulRemainingTries == 0 )
					{
						dlgResult = DLG_OK;
					}

					oData->returnValue = dlgResult;
					oShMemory.Detach((void *)oData);

					return 0;
				}
				delete dlg;
				dlg = NULL;
			}
			catch( ... )
			{
				if( dlg )  delete dlg;

				oData->returnValue = DLG_ERR;
				oShMemory.Detach((void *)oData);

				return 0;
			}

			oData->returnValue = DLG_CANCEL;
			oShMemory.Detach((void *)oData);
			return 0;

		}
		else if (iFunctionIndex == DLG_DISPLAY_PINPAD_INFO)
		{
			//To avoid problem on Mac Leopard, we follow these steps
			//1. The server is call with the 2 usual param
			//2. A fork is made
			//2a. The fork process recall the QtServer with a 3rd parameter
			//2b. The main process get the child pid and write it into the share memory and then quit
			//3. The Child process show the dialog

			SharedMem oShMemory;

			if ( ( argc == 3 ) || ( argc == 7 ) )
			{
				MWLOG(LEV_DEBUG, MOD_DLG,L"  %s called with DLG_DISPLAY_PINPAD_INFO",argv[0]);

				char csCommand[100];
				sprintf(csCommand,"%s %s %s",argv[0], argv[1], argv[2]);
                int len;
				if ( argc == 7 ){
                    len = strlen( csCommand );
                    sprintf(  &csCommand[len]
                            , " %s %s %s %s"
                            , argv[3], argv[4], argv[5], argv[6] );
                }
                len = strlen( csCommand );
                sprintf(  &csCommand[len], " child" );

				// spawn a child process
				signal(SIGCHLD,SIG_IGN);
				pid_t pid = fork();

				if(pid == -1)
				{
					MWLOG(LEV_ERROR, MOD_DLG, L"  %s fork : %s ", argv[0], strerror(errno) );
					exit(DLG_ERR);
				}

				if(pid == 0)
				{
					//
					// fork process
					//
					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s fork process started", argv[0]);

					//Due to Mac Leopard constraint, we start another QtServer
					//See __THE_PROCESS_HAS_FORKED_AND_YOU_CANNOT_USE_THIS_COREFOUNDATION_FUNCTIONALITY___YOU_MUST_EXEC__
					int code = system(csCommand);
					if(code != 0)
					{
						MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::CallQTServer %s %s child : %s ",argv[1], argv[2], strerror(errno) );
						exit(DLG_ERR);
					}

					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s fork system() return", argv[0]);

					return 0;
				}
				else
				{
					//
					// parent process
					//
					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s started fork process with ID %d", argv[0], pid);

					pid_t subpid=0;

					for(int i=0; i<10; i++)
					{
						CThread::SleepMillisecs(100); //Wait for the child process to start
						if(0 != (subpid = getPidFromParentid(pid, csCommand)))
						{
							break;
						}
					}

					oShMemory.Attach( sizeof(DlgDisplayPinpadInfoArguments), readableFilePath.c_str(),(void **) &oInfoData);

					if(subpid == 0)
					{
						MWLOG(LEV_ERROR, MOD_DLG, L"  %s failed to find child process ID", argv[0]);
						oInfoData->returnValue = DLG_ERR;
					}
					else
					{
						MWLOG(LEV_DEBUG, MOD_DLG, L"  %s find child process whit ID %ld", argv[0], subpid);
						oInfoData->tRunningProcess = subpid;
						oInfoData->returnValue = DLG_OK;
					}

					oShMemory.Detach((void *)oInfoData);

					return 0;
				}
			}
			else
			{
				//
				// child process
				//
				oShMemory.Attach( sizeof(DlgDisplayPinpadInfoArguments), readableFilePath.c_str(),(void **) &oInfoData);
				MWLOG(LEV_DEBUG, MOD_DLG, L"  %s child process started (pin=%ls, usage=%ld, operation=%ld)", argv[0],oInfoData->pinName,  oInfoData->usage, oInfoData->operation);

				QApplication a(argc, argv);
				MWLOG(LEV_DEBUG, MOD_DLG, L"  %s child process : QApplication created", argv[0]);

				// attach to the segment and get a pointer

				try
				{
					QString	qsReader=QString::fromWCharArray(oInfoData->reader);
					QString qsPinName=QString::fromWCharArray(oInfoData->pinName);
					QString qsMessage=QString::fromWCharArray(oInfoData->message);

					QString PINName;
                    PINName = getPinName( oInfoData->usage, oInfoData->pinName );

					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s child process : pin name set", argv[0]);

					if(qsMessage.isEmpty())
					{
						switch( oInfoData->operation )
						{
							case DLG_PIN_OP_VERIFY:
								switch( oInfoData->usage )
								{
									case DLG_PIN_AUTH:
										qsMessage = GETQSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
										if(!qsReader.isEmpty())
										{
											qsMessage += " \"";
											qsMessage += qsReader;
											qsMessage += "\"";
										}
										qsMessage += ", ";
										qsMessage += GETQSTRING_DLG(InOrderToAuthenticateYourself);
										qsMessage += "\n";
										break;
									case DLG_PIN_SIGN:
										qsMessage += GETQSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
										if(!qsReader.isEmpty())
										{
											qsMessage += " \"";
											qsMessage += qsReader;
											qsMessage += "\"";
										}
										break;
									default:
										qsMessage = GETQSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
										if(!qsReader.isEmpty())
										{
											qsMessage += " \"";
											qsMessage += qsReader;
											qsMessage += "\"";
										}
										qsMessage += "\n";
										break;
								}
								break;
							case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
								qsMessage = GETQSTRING_DLG(PleaseEnterYourPukOnThePinpadReader);
								if(!qsReader.isEmpty())
								{
									qsMessage += " \"";
									qsMessage += qsReader;
									qsMessage += "\"";
								}
								qsMessage += ", ";
								qsMessage = GETQSTRING_DLG(ToUnblock);
								qsMessage += " ";
								qsMessage += GETQSTRING_DLG(Your);
								qsMessage += " \"";
								if( !qsPinName.isEmpty() )
								{
									qsMessage += qsPinName;
								}
								else
								{
									qsMessage += GETQSTRING_DLG(Pin);
								}
								qsMessage += "\"\n";
								break;
							case DLG_PIN_OP_CHANGE:
								qsMessage = GETQSTRING_DLG(ChangeYourPin);
								qsMessage += " \"";
								if( !qsPinName.isEmpty() )
								{
									qsMessage += qsPinName;
								}
								else
								{
									qsMessage += GETQSTRING_DLG(Pin);
								}
								qsMessage += "\" ";
								qsMessage += GETQSTRING_DLG(OnTheReader);
								if(!qsReader.isEmpty())
								{
									qsMessage += " \"";
									qsMessage += qsReader;
									qsMessage += "\"";
								}
								qsMessage += "\n";
								qsMessage += GETQSTRING_DLG(EnterOldNewVerify);
								qsMessage += "\n";
								break;
							case DLG_PIN_OP_UNBLOCK_CHANGE:
								qsMessage = GETQSTRING_DLG(PleaseEnterYourPuk);
								qsMessage += ", ";
								qsMessage = GETQSTRING_DLG(ToUnblock);
								qsMessage += " ";
								qsMessage = GETQSTRING_DLG(Your);
								qsMessage += " \"";
								if( !qsPinName.isEmpty() )
								{
									qsMessage += qsPinName;
								}
								else
								{
									qsMessage += GETQSTRING_DLG(Pin);
								}
								qsMessage += "\" ";
								qsMessage += GETQSTRING_DLG(OnTheReader);
								if(!qsReader.isEmpty())
								{
									qsMessage += " \"";
									qsMessage += qsReader;
									qsMessage += "\"";
								}
								qsMessage += "\n";
								break;
							default:
								oInfoData->returnValue = DLG_BAD_PARAM;
								oShMemory.Detach((void *)oInfoData);
								MWLOG(LEV_ERROR, MOD_DLG, L"  %s child process : Bad param", argv[0]);
								return 0;
								break;
						}
					}

					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s child process : qsMessage set", argv[0]);

					dlg = new QDialog();
					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s child process : QDialog created", argv[0]);

					DlgPinOperation operation = oInfoData->operation;
					unsigned long infoCollectorIndex = oInfoData->infoCollectorIndex;

					// we do not need the shared memory area anymore
					oShMemory.Detach((void *)oInfoData);
					SharedMem::Delete(oShMemory.getID());

					dlgInfo = new dlgWndPinpadInfo(
												   infoCollectorIndex,
												   operation,
												   qsReader,
												   PINName,
												   qsMessage,
												   dlg,
												   &parentWndGeometry );

					MWLOG(LEV_DEBUG, MOD_DLG, L"  %s child process : dlgWndPinpadInfo created", argv[0]);
					dlg->show();
                   			dlg->raise();
					dlg->exec();

					if (dlgInfo)
					{
						delete dlgInfo;
						dlgInfo = NULL;
					}

					if (dlg)
					{
						delete dlg;
						dlg = NULL;
					}

					return 0;
				}
				catch(...)
				{
					MWLOG(LEV_ERROR, MOD_DLG, L"  %s child process failed", argv[0]);

					if (dlgInfo)
					{
						delete dlgInfo;
						dlgInfo = NULL;
					}

					if (dlg)
					{
						delete dlg;
						dlg = NULL;
					}

					oInfoData->returnValue = DLG_ERR;
					oShMemory.Detach((void *)oInfoData);
					SharedMem::Delete(oShMemory.getID());

					return 0;
				}
			}
			return 0;
		}
	}
	else
	{
		// wrong number of arguments
	}
	return iRet;
}

pid_t getPidFromParentid(pid_t parentid, const char *CommandLineToFind)
{
	pid_t pid = 0;

	FILE *			pF;
	pid_t pPid;
	pid_t pPpid;
	char pCommand[8192];

	// popen will fork and invoke the ps command and return a stream reference with its result data
	char buffer[8192];
#ifdef __APPLE__
	const char *psCommand="ps -A -o pid,ppid,command";
#else
	const char *psCommand="ps -A -o pid,ppid,command --sort=-etime";
#endif

	pF = popen(psCommand, "r");

	if (pF == NULL )
	{
		MWLOG(LEV_ERROR, MOD_DLG, L"  getPidFromParentid : popen %s failed", psCommand);
		return 0;
	}
	while(fgets(buffer,sizeof(buffer),pF) != NULL){
		sscanf(buffer,"%d %d %[^\n]\n",&pPid,&pPpid,pCommand);
		if(0 == strcmp(pCommand,CommandLineToFind)){
			pid=pPid;
			MWLOG(LEV_DEBUG, MOD_DLG, L"  getPidFromParentid :found pid=%ld", pid);
		}
	}

	// Close the stream
	pclose (pF);
	return pid;
}
