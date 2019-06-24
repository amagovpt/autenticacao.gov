import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageHelpAboutForm {
    Connections {
        target: controler
    }

    Keys.onPressed: {
        console.log("PageHelpAboutForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Component.onCompleted: {
        propertyTextVersion.text = qsTranslate("PageHelpAbout","STR_HELP_APP_VERSION") + " : "
                + controler.getAppVersion()
        propertyMainItem.forceActiveFocus()
    }
}
