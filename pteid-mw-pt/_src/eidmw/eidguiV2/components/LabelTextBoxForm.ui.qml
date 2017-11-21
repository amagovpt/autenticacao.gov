import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import "../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent

    property alias propertyDateText: dateText
    property alias propertyDateField: dateField
    property alias propertyRectField: rectField

    Text {
        id: dateText
        x: Constants.SIZE_TEXT_FIELD_H_SPACE
        font.pixelSize: Constants.SIZE_TEXT_LABEL
        font.family: lato.name
        color: Constants.COLOR_TEXT_LABEL
        height: Constants.SIZE_TEXT_LABEL
    }
    DropShadow {
        anchors.fill: rectField
        horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
        verticalOffset: Constants.FORM_SHADOW_V_OFFSET
        radius: Constants.FORM_SHADOW_RADIUS
        samples: Constants.FORM_SHADOW_SAMPLES
        color: Constants.COLOR_FORM_SHADOW
        source: rectField
        spread: Constants.FORM_SHADOW_SPREAD
        opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
    }
    RectangularGlow {
        anchors.fill: rectField
        glowRadius: Constants.FORM_GLOW_RADIUS
        spread: Constants.FORM_GLOW_SPREAD
        color: Constants.COLOR_FORM_GLOW
        cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
        opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
    }
    Rectangle {
        id: rectField
        width: parent.width
        color: "white"
        height: 2 * Constants.SIZE_TEXT_FIELD
        anchors.top :dateText.bottom
        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
        TextEdit {
            id: dateField
            y: Constants.SIZE_TEXT_FIELD_V_SPACE
            width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
            height: parent.height - 2 * Constants.SIZE_TEXT_FIELD_V_SPACE
            anchors.horizontalCenter: rectField.horizontalCenter
            font.capitalization: Font.AllUppercase
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            color: Constants.COLOR_TEXT_BODY
            selectByMouse :true
            readOnly: true
        }
    }
}
