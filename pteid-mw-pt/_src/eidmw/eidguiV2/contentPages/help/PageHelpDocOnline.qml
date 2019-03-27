import QtQuick 2.6

PageHelpDocOnlineForm {
    Connections {
        target: controler
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
    }
}

