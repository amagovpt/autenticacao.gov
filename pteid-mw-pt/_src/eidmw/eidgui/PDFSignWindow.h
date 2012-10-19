/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2012 Caixa Mágica Software.
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
 * Author: André Guerreiro <andre.guerreiro@caixamagica.pt>
 **************************************************************************** */

#ifndef PDFSIGNWINDOW_H_

#include <QDialog>
#include <QDropEvent>
#include "ui_PDFSignWindow.h"
#include "CardInformation.h"

class PDFSignWindow : public QDialog
{
	Q_OBJECT
	private slots:
	    void on_button_addfile_clicked();
	    void on_button_sign_clicked();
	    void on_button_cancel_clicked();
	    void on_checkBox_location_toggled(bool);
	    void on_checkBox_reason_toggled(bool);
	    void on_visible_checkBox_toggled(bool);
	    void on_radioButton_choosepage_toggled(bool);
	    void on_spinBox_page_valueChanged(int);
	    void on_tableWidget_currentCellChanged(int row, int column, int prev_row, int prev_column);


	public:
	    PDFSignWindow(QWidget * parent, CardInformation &ci);
	    ~PDFSignWindow();


	private:
	    void update_sector(int row, int column);
	    void highlightSectors(QString &csv_sectors);
	    void clearAllSectors();
	    void addFileToListView(QStringList &str);

	    Ui_PDFSignWindow ui;
    	    CardInformation const& m_CI_Data;
	    QStringList aList;
	    QProgressDialog *pdialog;
	    QFutureWatcher<void> FutureWatcher;
	    PTEID_PDFSignature *m_pdf_sig;

	    QString current_input_path;
	    QBrush m_default_background;

	    int m_selected_sector;


};


#endif
