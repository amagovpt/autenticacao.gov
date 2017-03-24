#ifndef dlgCmdCode_H
#define dlgCmdCode_H

#include <QDialog>

namespace Ui {
class dlgCmdCode;
}

class dlgCmdCode : public QDialog
{
    Q_OBJECT

public:
    explicit dlgCmdCode(QWidget *parent = 0);
    ~dlgCmdCode();
    QString getReceivedCode();

private slots:
    void on_button_Ok_clicked();

private:
    Ui::dlgCmdCode *ui;
    QString m_receivedCode;
};

#endif // dlgCmdCode_H
