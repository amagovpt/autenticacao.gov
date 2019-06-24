import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageHelpDocOnlineForm {
    Connections {
        target: controler
    }

    propertyButtonDownload{
        onClicked: {
            Qt.openUrlExternally("https://www.autenticacao.gov.pt/documents/10179/11465/Manual+de+Utiliza%C3%A7%C3%A3o+da+Aplica%C3%A7%C3%A3o+do+Cart%C3%A3o+de+Cidad%C3%A3o+v3/");
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
        propertyMainItem.forceActiveFocus()
    }
}

