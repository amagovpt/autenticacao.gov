import QtQuick 2.6

PageHomeForm {
    Connections {
        target: controler
    }
    propertyTextLinkCC.onLinkActivated: {
        Qt.openUrlExternally(link)
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
