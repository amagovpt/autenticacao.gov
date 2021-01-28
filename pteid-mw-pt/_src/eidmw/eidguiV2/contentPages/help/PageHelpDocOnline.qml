/*-****************************************************************************

 * Copyright (C) 2017, 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageHelpDocOnlineForm {
    Connections {
        target: controler
    }

    propertyButtonDownload{
        onClicked: {
            Qt.openUrlExternally("https://amagovpt.github.io/docs.autenticacao.gov/Manual_de_Utilizacao_v3.pdf");
        }
    }

    Keys.onPressed: {
        console.log("PageHelpDocOnlineForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
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
        if(mainFormID.propertyPageLoader.propertyForceFocus)
            propertyMainItem.forceActiveFocus()
    }
}

