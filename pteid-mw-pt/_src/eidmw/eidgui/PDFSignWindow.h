/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2012-2014 Caixa Mágica Software.
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

enum ErrorCode
{
	SIG_SUCCESS
	,SIG_ERROR
	,TS_WARNING

};

/* FW declaration */
class FreeSelectionDialog;

class PDFSignWindow : public QDialog
{
	Q_OBJECT
	private slots:
	    void on_button_addfile_clicked();
	    void on_button_sign_clicked();
	    void on_button_cancel_clicked();
	    void on_pushButton_freeselection_clicked();
	    void on_pushButton_imgChooser_clicked();
	    void on_pushButton_switchOrientation_clicked();
	    void on_checkBox_location_toggled(bool);
	    void on_checkBox_reason_toggled(bool);
	    void on_visible_checkBox_toggled(bool);
	    void on_radioButton_choosepage_toggled(bool);
	    void on_radioButton_firstpage_toggled(bool);
	    void on_radioButton_lastpage_toggled(bool);
	    void on_spinBox_page_valueChanged(int);
	    void on_tableWidget_currentCellChanged(int row, int column, int prev_row, int prev_column);
	    void on_FreeSelectionDialog_closed();


	public:
	    void customEvent(QEvent *ev);
	    /*Event Handlers for card inserted/removed events
	    that come from Main Window
	    */
	    void enableSignButton();
	    void disableSignButton();
	    PDFSignWindow(QWidget * parent, CardInformation &ci);
	    ~PDFSignWindow();


	private:
	    void update_sector(int row, int column);
	    void update_sector(double x_pos, double y_pos);
	    void clear_sig_position();
	    bool validateSelectedSector();
	    void highlightSectors(QString &csv_sectors);
	    void clearAllSectors();
	    void addFileToListView(QStringList &str);
	    void run_sign(int page, QString& savefile, char *location, char *reason);
	    void ShowSuccessMsgBox();
	    void switchOrientation();
	    void ShowSectorErrorMessage();
	    void ShowErrorMsgBox(QString message);


	    Ui_PDFSignWindow ui;
	    int table_lines;
	    int table_columns;
	    double sig_coord_x;
	    double sig_coord_y;

	    bool card_present;
	    bool horizontal_page_flag;

    	CardInformation const& m_CI_Data;
	    QAbstractItemModel *list_model;
	    QProgressDialog *pdialog;
	    FreeSelectionDialog *m_selection_dialog;

	    QFutureWatcher<void> FutureWatcher;
	    PTEID_PDFSignature *m_pdf_sig;
	
	    //Number of pages of currently loaded document
	    int m_current_page_number;

	    QString current_input_path;
	    QBrush m_default_background;
	    QImage m_scaled_image;
	    QByteArray m_jpeg_scaled_data;

	    int m_selected_sector;

	    //Success flag for the error messages
	    ErrorCode success;

};


#endif
