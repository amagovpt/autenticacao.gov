/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#ifndef EDTGUI_H
#define EDTGUI_H

#include <QtGui>
#include <QTranslator>
#include <QtGui/QMainWindow>
#include <QString>
#include "ui_edtgui.h"
#include "diagengine.h"
#include "enginethread.h"

class eDTGui : public QMainWindow
{
	Q_OBJECT

public:
	enum
	{
		SCR_PRESENTATION=0
		, SCR_INFORMATION
		, SCR_PROGRESS
		, SCR_SIGNING
		, SCR_SUMMARY
		, SCR_DETAILS
		, SCR_DIAGNOSTICS
	};
	enum
	{
		  COL_TEST_NAME = 0
	    , COL_TEST_RESULT
		, SUMMARY_NRCOLUMNS
	};
#define HEARTBEAT_TIMEOUT 30.0
#define PROGRESS_TIMER    100
#define HEARTBEAT_TIMER   1000

public:
	eDTGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~eDTGui();

private:
			void	updateVerdict(void);
			void	previousVerdict();
			void	nextVerdict();
			void	gettingHelp(void);
			void	solved(void);
			void	send_resolution(void);
			void	not_solved(void);
			void	closeEvent(QCloseEvent *event);
			void	loadReport( void );
			bool	loadTranslationResources(QString const& m_Language);
			bool	allTestsPassed();
			void	fillTestTableGrid();
	static	QString w2qstr(const std::wstring wstr);

private:
	Ui::eDTGuiClass			 ui;
	QString					 m_Language;			// GUI language
	QTranslator				 m_translator;			// translator for GUI
	EngineThread			 m_engineThread;		// engine thread placeholder
	size_t					 m_currTestNr;			// current test nr running
	QTimer*					 m_processingTimer;
	QTimer*					 m_heartbeatTimer;
	size_t					 m_currDiag;
	bool					 m_resolved;

private slots:
	void on_pb_Cancel_clicked();
	void on_pb_SaveReport_clicked();
	void on_pb_SaveReport_2_clicked();
	void on_pb_testAuthentication_clicked();
	void on_stackedWidget_currentChanged(int);
	void on_pb_Diag2_clicked();
	void on_pb_Diag_clicked();
	void on_pb_Quit_clicked();
	void on_pb_SaveSend_clicked();
	void on_pb_solved_clicked();
	void on_pb_needhelp_clicked();
	void on_pb_not_solved_clicked();
	void on_pb_previous_clicked();
	void on_pb_next_clicked();
	void on_pb_Summary_clicked();
	void on_pb_Summary_2_clicked();
	void on_pb_Details_clicked();
	void on_pb_Continue_clicked();
	void on_pb_Continue_2_clicked();
	void on_pb_Continue_3_clicked();
	void on_pb_Analyse_fr_clicked();
	void on_pb_Analyse_nl_clicked();
	void customEvent( QEvent* pEvent );
	void checkProgress();
	void checkHeartbeat();
};

#endif // EDTGUI_H
