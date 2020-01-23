/*-****************************************************************************

 * Copyright (C) 2017, 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageHomeForm {
    Connections {
        target: controler
    }

    propertyButtonDownload{
        onClicked: {
            Qt.openUrlExternally("https://amagovpt.github.io/autenticacao.gov/Manual_de_Utilizacao_v3.pdf");
        }
    }

    propertyReminderCheckBox{
        onCheckedChanged: if(propertyReminderCheckBox.checked){
                              controler.setNotShowHelpStartUp(true)
                          }else{
                                controler.setNotShowHelpStartUp(false)
                          }
    }

    Component.onCompleted: {
        propertyReminderCheckBox.checked = controler.getNotShowHelpStartUp()
        if(!propertyCmdDialog.visible)
            propertyMainItem.forceActiveFocus()
    }
}
