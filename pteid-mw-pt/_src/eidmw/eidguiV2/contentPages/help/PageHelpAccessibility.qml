/*-****************************************************************************

 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageHelpAccessibilityForm {
    Connections {
        target: controler
    }

    Keys.onPressed: function(event) {
        console.log("PageHelpAccessibilityForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Component.onCompleted: {
        if(mainFormID.propertyPageLoader.propertyForceFocus)
            propertyTextTitle.forceActiveFocus()
    }
}

