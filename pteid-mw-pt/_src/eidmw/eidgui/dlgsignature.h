/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2011-2012 Caixa Mágica Software.
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
 * 
 * Author: Luis Medinas <luis.medinas@caixamagica.pt>
 * 	   André Guerreiro <andre.guerreiro@caixamagica.pt>	
 *
 **************************************************************************** */

#ifndef _DLGSIGNATURE_H_
#define _DLGSIGNATURE_H_

#include "ui_dlgSignature.h"
#include <QDialog>
#include <QProgressDialog>
#include "genpur.h"
#include "CardInformation.h"
#include "mylistview.h"


enum XadesLevel
{
    XADES_B,
    XADES_T,
    XADES_A
};


class dlgSignature : public QDialog
{
    Q_OBJECT

private slots:
    void on_pbCancel_clicked ( void );
    void on_pbAddFiles_clicked ( void );
    void on_pbSign_clicked ( void );
    //void ShowContextMenu(const QPoint& pos);

public:
    dlgSignature(QWidget* parent, int selected_reader, CardInformation& CI_Data);
    ~dlgSignature();

private:
    Ui_dlgSignature ui;
    CardInformation const& m_CI_Data;
    QString m_CurrReaderName; //!< the current reader we're using
    QAbstractItemModel *list_model;
    MyListView *view;
    QProgressDialog *pdialog;
    QFutureWatcher<void> FutureWatcher;
    long error_code;
	int m_selected_reader;
    XadesLevel getSelectedXadesLevel();
    void SignListView (QStringList list);
	PTEID_EIDCard& getNewCard();
    void runsign(const char ** paths, unsigned int n_paths, const char *output_path, XadesLevel level);
    void run_multiple_sign(const char**, unsigned int, const char*, XadesLevel);
    void ShowErrorMsgBox();
    void ShowSuccessMsgBox();

};

#endif
