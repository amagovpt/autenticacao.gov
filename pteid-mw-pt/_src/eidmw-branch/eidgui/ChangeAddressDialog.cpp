#include "ChangeAddressDialog.h"
#include <QPushButton>
#include <QString>
#include "eidlib.h"
#include "eidlibdefines.h"


ChangeAddressDialog::ChangeAddressDialog(QWidget* parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{

	ui.setupUi(this);
	//Set icon
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );
    this->setFixedSize(this->width(), this->height());
	//QDialogButtonBox buttonBox = ;	
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	ui.formLayout->setLabelAlignment(Qt::AlignLeft);

	connect(ui.lineEdit_secretCode, SIGNAL(textEdited(QString)), this, SLOT(secretCode_textEdited(QString)));
	connect(ui.lineEdit_process, SIGNAL(textEdited(QString)), this, SLOT(process_textEdited(QString)));

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

void ChangeAddressDialog::secretCode_textEdited(QString text)
{


ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
	ui.lineEdit_process->text().size() > 0 && text.size() > 0);
	
}

void ChangeAddressDialog::process_textEdited(QString text)
{

ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
	ui.lineEdit_secretCode->text().size() > 0 && text.size() > 0);
	
}
