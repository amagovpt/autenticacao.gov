/*-****************************************************************************

 * Copyright (C) 2026 Christophe Soares - <soareschristophe@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick
import QtQuick.Controls

import "../scripts/Constants.js" as Constants

/* Switch indicator with the look of the Qt 5 Material style used by the application.
   The Qt 6 Material style follows Material 3, which does not fit the layout sizes. */
Item {
    id: indicator

    property Item control // the Switch; AbstractButton resolves to the style type and rejects it
    // control is assigned after the bindings are first evaluated
    readonly property bool checked: control ? control.checked : false

    implicitWidth: 40
    implicitHeight: 24
    x: !control ? 0 : control.text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding)
                                   : control.leftPadding + (control.availableWidth - width) / 2
    y: control ? control.topPadding + (control.availableHeight - height) / 2 : 0
    opacity: !control || control.enabled ? 1.0 : 0.38

    Rectangle {
        id: track
        width: 34
        height: 14
        radius: height / 2
        anchors.centerIn: parent
        color: indicator.checked ? Qt.alpha(Constants.COLOR_MAIN_BLUE, 0.5) : "#BDBDBD"
    }

    Rectangle {
        id: knob
        width: 20
        height: 20
        radius: width / 2
        anchors.verticalCenter: parent.verticalCenter
        x: indicator.checked ? indicator.width - width : 0
        color: indicator.checked ? Constants.COLOR_MAIN_BLUE : "#FAFAFA"
        border.width: indicator.checked ? 0 : 1
        border.color: "#D0D0D0"

        Behavior on x {
            NumberAnimation { duration: 120; easing.type: Easing.OutCubic }
        }
    }
}
