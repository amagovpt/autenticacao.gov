#include "dlgCmdUserInfo.h"
#include "ui_dlgCmdUserInfo.h"
#include <QMessageBox>

dlgCmdUserInfo::dlgCmdUserInfo(QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::dlgCmdUserInfo){

    ui->setupUi(this);

    QRegExp regexp;
    regexp.setPattern( "^([\+][0-9]{0,3}[ ])?[0-9]{,9}$" );
    QRegExpValidator *validatorUserId = new QRegExpValidator( regexp, this );
    ui->textbox_UserId->setValidator( validatorUserId );

    regexp.setPattern( "^[0-9]{,4}$" );
    QRegExpValidator *validatorPin = new QRegExpValidator( regexp, this );
    ui->textbox_Pin->setValidator( validatorPin );

    ui->textbox_UserId->clear();
    ui->textbox_Pin->clear();

    m_userId = QString("");
    m_Pin = QString("");

    connect( ui->button_Cancel, SIGNAL( clicked() ), this, SLOT( close() ) );
}/* dlgCmdUserInfo::dlgCmdUserInfo( */

dlgCmdUserInfo::~dlgCmdUserInfo(){
    delete ui;
}/* dlgCmdUserInfo::~dlgCmdUserInfo() */

QString dlgCmdUserInfo::getUserId(){
    return m_userId;
}/* dlgCmdUserInfo::getUserId() */

QString dlgCmdUserInfo::getUserPin(){
    return m_Pin;
}/* dlgCmdUserInfo::getUserPin() */

void dlgCmdUserInfo::on_button_Ok_clicked(){
    if ( ui->textbox_UserId->text().isEmpty() ){
        QMessageBox m_msgBox( QMessageBox::Critical
                              , tr( "Critical" )
                              , tr("Empty field: UserId")
                              , 0, this);
        m_msgBox.exec();

        ui->textbox_UserId->setFocus();
        return;
    }/* if ( ui->textbox_UserId->text().isEmpty() ) */

    if ( ui->textbox_Pin->text().isEmpty() ){
        QMessageBox m_msgBox( QMessageBox::Critical
                              , tr( "Critical" )
                              , tr("Empty field: PIN")
                              , 0, this);
        m_msgBox.exec();

        ui->textbox_Pin->setFocus();
        return;
    }/* if ( ui->textbox_Pin->text().isEmpty() ) */

    m_userId = ui->textbox_UserId->text();
    m_Pin = ui->textbox_Pin->text();

    this->hide();
}/* dlgCmdUserInfo::on_button_Ok_clicked() */

