#include "dlgCmdCode.h"
#include "ui_dlgCmdCode.h"
#include <QMessageBox>

dlgCmdCode::dlgCmdCode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgCmdCode){
    ui->setupUi(this);

    QRegExp regexp;
    regexp.setPattern( "^[0-9]{,6}$" );
    QRegExpValidator *validator = new QRegExpValidator( regexp, this );
    ui->textbox_RcvdCode->setValidator( validator );

    connect( ui->button_Cancel, SIGNAL( clicked() ), this, SLOT( close() ) );
    ui->textbox_RcvdCode->clear();
    m_receivedCode = QString("");
}/* dlgCmdCode::dlgCmdCode(QWidget *parent) */

dlgCmdCode::~dlgCmdCode(){
    delete ui;
}/* dlgCmdCode::~dlgCmdCode() */

QString dlgCmdCode::getReceivedCode(){
    return m_receivedCode;
}/* dlgCmdCode::getReceivedCode() */

void dlgCmdCode::on_button_Ok_clicked(){
    if ( ui->textbox_RcvdCode->text().isEmpty() ){
        QMessageBox m_msgBox( QMessageBox::Critical
                              , tr( "Critical" )
                              , tr("Empty field: Received Code")
                              , 0
                              , this);
        m_msgBox.exec();
        return;
    }/* if ( ui->textbox_RcvdCode->text().isEmpty() ) */

    m_receivedCode = ui->textbox_RcvdCode->text();
}/* dlgCmdCode::on_button_Ok_clicked() */

