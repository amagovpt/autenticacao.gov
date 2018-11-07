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

#ifndef _DLGPRINT_H_
#define _DLGPRINT_H_

#include "ui_dlgPrint.h"
#include <QDialog>
#include <QProgressDialog>
#include <QPrinter>
#include "genpur.h"
#include "CardInformation.h"

enum Operation { PRINT, PDF };

class dlgPrint : public QDialog
{
    Q_OBJECT

private slots:
	void on_pbPrint_clicked( void );
	void on_pbGeneratePdf_clicked( void );
	void on_pbCancel_clicked( void );
	void on_chboxID_toggled( bool bChecked );
	void on_chboxAddress_toggled( bool bChecked );
	void on_chboxIDExtra_toggled( bool bChecked );
	void on_chboxPersoData_toggled( bool bChecked );
    void on_chboxSignature_toggled( bool bChecked );
	//void on_btnPDF_clicked( void );


public:
    dlgPrint(QWidget* parent , CardInformation& CI_Data, GenPur::UI_LANGUAGE lng);
    ~dlgPrint();

private:
    Ui_dlgPrint ui;
    CardInformation const& m_CI_Data;
    QString					m_CurrReaderName;		//!< the current reader we're using
    QFutureWatcher<void> FutureWatcher;
    QProgressDialog *pdialog;
    int sections_to_print;
    double pos_x, pos_y;

    bool addressPINRequest_triggered(CardInformation& CI_Data);
    void ShowErrorMsgBox(Operation op);
    
    QString getTranslated(const QString &str);
    void ShowSuccessMsgBox();
    const char * persodata_triggered();
    bool drawpdf(CardInformation& CI_Data, QPrinter &printer, QString filepath);
};

#endif
