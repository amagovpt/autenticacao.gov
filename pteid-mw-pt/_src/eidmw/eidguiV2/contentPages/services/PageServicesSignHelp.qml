import QtQuick 2.6
import QtQuick.Controls 2.1

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageServicesSignHelpForm {

    Keys.enabled: true
    
    Keys.onPressed: {
        console.log("PageServicesSignHelp onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Component.onCompleted: {
        console.log("Page Services Sign Help Form Completed")
        propertyMainItem.forceActiveFocus()
    }
}
