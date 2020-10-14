/*-****************************************************************************

 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Layouts 1.3
import "../scripts/Constants.js" as Constants

RowLayout {
    property alias propertyRowText: rowText
    anchors.topMargin: 0.5 * Constants.SIZE_TEXT_BODY
    width: parent.width
    TextBullet {
        Layout.fillHeight: true
    }
    Text {
        id: rowText
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        font.bold: parent.parent.activeFocus
        wrapMode: Text.Wrap
        horizontalAlignment: Text.left
        color: Constants.COLOR_TEXT_BODY
        Layout.fillWidth: true
    }
}

