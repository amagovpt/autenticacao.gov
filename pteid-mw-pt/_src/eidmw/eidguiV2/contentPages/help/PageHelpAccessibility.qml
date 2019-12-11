/*-****************************************************************************

 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageHelpAccessibilityForm {
    Connections {
        target: controler
    }

    Keys.onPressed: {
        console.log("PageHelpAccessibilityForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Component.onCompleted: {
        if(mainFormID.propertyPageLoader.propertyForceFocus)
            propertyTextTitle.forceActiveFocus()
    }
}

