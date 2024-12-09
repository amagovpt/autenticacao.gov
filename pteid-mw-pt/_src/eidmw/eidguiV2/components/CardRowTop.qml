/*-****************************************************************************

 * Copyright (C) 2017 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6

import "../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent

    Item{
        id: rectTopLeft
        width: parent.width - Constants.SIZE_ROW_H_SPACE - (gapi.hasOnlyICAO() ? Constants.SIZE_IMAGE_LOGO_ICAO_WIDTH : Constants.SIZE_IMAGE_LOGO_CC_WIDTH)
        height: parent.height
    }
    Connections {
        id: connectionGapi
        target: gapi
        onSignalCardAccessError: updateIcon()

        onSignalCardChanged: updateIcon()

        function updateIcon() {
            iconImage.width = gapi.hasOnlyICAO() ? Constants.SIZE_IMAGE_LOGO_ICAO_WIDTH : Constants.SIZE_IMAGE_LOGO_CC_WIDTH
            iconImage.height = gapi.hasOnlyICAO() ? Constants.SIZE_IMAGE_LOGO_ICAO_HEIGHT : Constants.SIZE_IMAGE_LOGO_CC_HEIGHT
            iconImage.source = gapi.hasOnlyICAO() ? "../images/EPassport_logo.svg" : "../images/logo_CC.png"
        }
    }
    Component.onCompleted: {
        connectionGapi.updateIcon()
    }

    Item{
        id: rectToRight
        width: Constants.SIZE_IMAGE_LOGO_CC_WIDTH
        height: parent.height
        anchors.left: rectTopLeft.right
        anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
        Image {
            id: iconImage
            antialiasing: true
            width: gapi.hasOnlyICAO() ? SIZE_IMAGE_LOGO_ICAO_WIDTH : Constants.SIZE_IMAGE_LOGO_CC_WIDTH
            height: gapi.hasOnlyICAO() ? SIZE_IMAGE_LOGO_ICAO_HEIGHT : Constants.SIZE_IMAGE_LOGO_CC_HEIGHT
            source: gapi.hasOnlyICAO() ? "../images/EPassport_logo.svg" : "../images/logo_CC.png"
            fillMode: gapi.hasOnlyICAO() ? Image.PreserveAspectFit : Image.Stretch
        }
    }
}
