#ifndef ATTRIBUTESUPPLIERS_H
#define ATTRIBUTESUPPLIERS_H

#include <QDialog>
#include <QThread>
#include <QTreeWidgetItem>

class ns3__AttributeSupplierType;

namespace Ui {
class AttributeSuppliers;
}

class AttributeSuppliers : public QDialog
{
    Q_OBJECT

public:
    explicit AttributeSuppliers(std::vector<ns3__AttributeSupplierType *> suppliersList, QWidget *parent = 0);
    ~AttributeSuppliers();

    std::vector<ns3__AttributeSupplierType *> m_suppliersList;
    std::vector<ns3__AttributeSupplierType *> m_selectedSuppliersList;
private:
    Ui::AttributeSuppliers *ui;

    QTreeWidgetItem * AddRoot(QString);
    void showSuppliers();
    void AddChild(QTreeWidgetItem*, QString, int);
public slots:
    void ShowErrorMsgBox(QString);
private slots:
    void on_okBtn_clicked();
};

#endif // ATTRIBUTESUPPLIERS_H
