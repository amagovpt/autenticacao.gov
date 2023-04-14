/*-****************************************************************************

 * Copyright (C) 2023 Nuno Romeu Lopes - <nuno.lopes@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import "../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent

    property alias propertyDateText: dateText
    property alias propertyDateField: textField
    property alias propertyRectField: rectField
    property string accessibleText: dateText.text
                                    + (textField.length
                                    ? textField.text
                                    : qsTranslate("GAPI", "STR_EMPTY_FIELD"))
    property string type: "LabelTextBox"

    Text {
        id: dateText
        x: Constants.SIZE_TEXT_FIELD_H_SPACE
        font.pixelSize: parent.activeFocus || parent.parent.activeFocus
                        ? Constants.SIZE_TEXT_LABEL_FOCUS
                        : Constants.SIZE_TEXT_LABEL
        font.family: lato.name
        font.bold: parent.activeFocus
                   || parent.parent.activeFocus ? true : false
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
        anchors.top: dateText.bottom
        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

        Rectangle{
            width: parent.width
            height: parent.height
            clip: true
            
            Flickable {
                id: flick
                y: Qt.platform.os === "windows" ? Constants.SIZE_TEXT_FIELD_V_SPACE : Constants.SIZE_TEXT_FIELD / 2
                width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                height: parent.height - 2 * Constants.SIZE_TEXT_FIELD_V_SPACE
                boundsBehavior: Flickable.StopAtBounds
                flickableDirection: Flickable.HorizontalFlick
                contentWidth: textField.contentWidth
                contentHeight: textField.contentHeight
                anchors.horizontalCenter: parent.horizontalCenter

                function ensureVisible(r)
                {
                    if (contentX >= r.x)
                        contentX = r.x;
                    else if (contentX+width <= r.x+r.width)
                        contentX = r.x+r.width-width;
                    if (contentY >= r.y)
                        contentY = r.y;
                    else if (contentY+height <= r.y+r.height)
                        contentY = r.y+r.height-height;
                }

                TextEdit {
                    id: textField
                    width: flick.width
                    color: Constants.COLOR_TEXT_BODY
                    selectByMouse: true
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                }

                ScrollBar.horizontal: ScrollBar {
                        active: false
                        visible: true
                        height: Constants.SIZE_TEXT_FIELD_V_SPACE
                }
            }
        }   
    }
}
