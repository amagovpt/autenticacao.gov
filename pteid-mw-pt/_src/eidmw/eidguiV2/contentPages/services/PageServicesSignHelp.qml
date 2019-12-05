/*-****************************************************************************

 * Copyright (C) 2017, 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

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
        propertyTextTitle.forceActiveFocus()
    }
}
