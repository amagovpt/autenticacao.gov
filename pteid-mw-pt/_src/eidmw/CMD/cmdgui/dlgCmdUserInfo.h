#ifndef DLGCMDUSERINFO_H
#define DLGCMDUSERINFO_H

#include <QDialog>

namespace Ui {
class dlgCmdUserInfo;
}

class dlgCmdUserInfo : public QDialog
{
    Q_OBJECT

public:
    explicit dlgCmdUserInfo(QWidget *parent = 0);
    ~dlgCmdUserInfo();
    QString getUserId();
    QString getUserPin();

private slots:
    void on_button_Ok_clicked();

private:
    Ui::dlgCmdUserInfo *ui;
    QString m_userId;
    QString m_Pin;
};

#endif // DLGCMDUSERINFO_H
