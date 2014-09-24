#include "ChangeAddressDialog.h"


ChangeAddressDialog::ChangeAddressDialog(QWidget* parent)
    : QDialog(parent)
{

	ui.setupUi(this);
	//Set icon
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );
    this->setFixedSize(this->width(), this->height());
	//QDialogButtonBox buttonBox = ;	
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

ChangeAddressDialog::~ChangeAddressDialog()
{

}

QString ChangeAddressDialog::getSecretCode()
{
	return ui.lineEdit_secretCode->text();
}

QString ChangeAddressDialog::getProcess()
{
	return ui.lineEdit_process->text();
	
}

void ChangeAddressDialog::on_lineEdit_secretCode_textEdited(QString &text)
{

ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
	ui.lineEdit_process->text().size() > 0 && text.size() > 0);
	

}

void ChangeAddressDialog::on_lineEdit_process_textEdited(QString &text)
{

ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
	ui.lineEdit_secretCode->text().size() > 0 && text.size() > 0);
	

}
