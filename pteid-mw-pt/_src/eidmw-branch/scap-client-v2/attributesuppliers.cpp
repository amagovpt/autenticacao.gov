#include "attributesuppliers.h"
#include "ui_attributesuppliers.h"

#include <QtCore>
#include <QFuture>

#include "ASService/soapH.h"
#include "ASService/soapAttributeSupplierBindingProxy.h"
//#include "ASService/AttributeSupplierBinding.nsmap"

//STD Library
#include <iostream>
#include <sstream>
#include <vector>

AttributeSuppliers::AttributeSuppliers(std::vector<ns3__AttributeSupplierType *> suppliersList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AttributeSuppliers),
    m_suppliersList(suppliersList)
{
    ui->setupUi(this);

    //Set icon
    this->setWindowIcon(QIcon(":/appicon/Images/pteid.ico"));

    std::cout << "Number of suppliers::: " << m_suppliersList.size() << std::endl << std::flush;
    showSuppliers();
}

AttributeSuppliers::~AttributeSuppliers()
{
    delete ui;
}

void AttributeSuppliers::showSuppliers(){
    if(m_suppliersList.size() > 0 ){
        QTreeWidgetItem *publicParent = AddRoot(tr("Public Entities"));
        QTreeWidgetItem *privateParent = AddRoot(tr("Companies"));
        publicParent->setExpanded(true);
        privateParent->setExpanded(true);

        for (uint i = 0; i!=m_suppliersList.size(); i++)
        {
			      QString name = QString::fromUtf8(m_suppliersList[i]->Name.c_str());
            std::cout << "Attr Supplier ID: " << m_suppliersList[i]->Id << std::endl;

            QString url = m_suppliersList[i]->Id.c_str();

            if(url.contains("SCAP", Qt::CaseSensitive)){
                AddChild(publicParent, name, i);
            }else{
                AddChild(privateParent, name, i);
            }
        }
    }else{
        std::cout << "No suppliers available" << std::endl;
    }
}

QTreeWidgetItem * AttributeSuppliers::AddRoot(QString name){
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->suppliersList);
    item->setText(0,name);
    ui->suppliersList->addTopLevelItem(item);
    return item;
}

void AttributeSuppliers::AddChild(QTreeWidgetItem *parent, QString name, int arrayPos){
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0,name);
    item->setData(0, Qt::UserRole, arrayPos);
    item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
    item->setCheckState(0,Qt::Unchecked);
    parent->addChild(item);
}

void AttributeSuppliers::ShowErrorMsgBox(QString){

}

void AttributeSuppliers::on_okBtn_clicked()
{
    int arrayPos = 0;
    //Foreach top level
    for( int parent = 0; parent < ui->suppliersList->topLevelItemCount(); ++parent )
    {
       QTreeWidgetItem *topLevel = ui->suppliersList->topLevelItem( parent );

       //Foreach childreen
       for( int child = 0; child < topLevel->childCount(); ++child )
       {
           QTreeWidgetItem *childItem = topLevel->child(child);

           // See if user checked item
           if(childItem->checkState(0))
           {
               // Add to list when checked
               int test = (int)childItem->data(0, Qt::UserRole).toInt();
               m_selectedSuppliersList.push_back(m_suppliersList[childItem->data(0, Qt::UserRole).toInt()]);
           }
           arrayPos++;
       }
    }
    emit(close());
}
