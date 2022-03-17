/*-****************************************************************************

 * Copyright (C) 2022 Tiago Barroso - <tiago.barroso@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../scripts/Constants.js" as Constants
import "../scripts/Functions.js" as Functions
import "../components" as Components

//Import C++ defined enums
import eidguiV2 1.0

Item {

    property alias title: title
    property alias description: description
    property alias rightButton: rightButton
    property alias link: link
    property string propertyUrl

    height: parent.height
    width: parent.width

    Label {
        id: title
        width: parent.width - Constants.SIZE_IMAGE_BOTTOM_MENU - 40
        text: ""
        lineHeight: 1.2
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        maximumLineCount: 3
    
        anchors.left: parent.left
        anchors.leftMargin: Constants.SIZE_IMAGE_BOTTOM_MENU + 2 * 10 + 4
        anchors.top: parent.top
        anchors.topMargin: 26
        
        font.bold: activeFocus
        font.pixelSize: Constants.SIZE_TEXT_LABEL_FOCUS
        font.family: lato.name
        color: Constants.COLOR_TEXT_BODY
    }

    Label {
        id: description
        width: parent.width - Constants.SIZE_IMAGE_BOTTOM_MENU - 40
        text: ""
        wrapMode: TextEdit.Wrap
        color: Constants.COLOR_TEXT_BODY
        horizontalAlignment: Text.AlignJustify
        elide: Text.ElideRight

        anchors.top: title.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE * 2
        anchors.left: parent.left
        anchors.leftMargin: Constants.SIZE_IMAGE_BOTTOM_MENU + 2 * 10 + 4

        font.pixelSize: Constants.SIZE_TEXT_LABEL_FOCUS
        font.bold: activeFocus
        font.family: lato.name
    }

    Item {
        id: linkContainer
        height: Constants.SIZE_TEXT_LABEL
        opacity: 0.5
        visible: propertyUrl.length > 0

        anchors.left: parent.left
        anchors.leftMargin: Constants.SIZE_IMAGE_BOTTOM_MENU + 2 * 10 + 4  
        anchors.right: rightButton.left
        anchors.rightMargin: Constants.MARGIN_NOTIFICATION_CENTER
        anchors.verticalCenter: rightButton.verticalCenter

        Components.Link {
            id: link
            width: parent.width
            height: parent.height
            visible: true
            propertyLinkUrl: propertyUrl
            propertyAccessibleText: qsTranslate("PageDefinitionsUpdates","STR_AUTOUPDATENEWS_URL")

            anchors.top: parent.top

            propertyText.text: "<a href='" + propertyUrl + "'>"
                                     + qsTranslate("PageDefinitionsUpdates","STR_AUTOUPDATENEWS_KNOW_MORE")
            propertyText.verticalAlignment: Text.AlignVCenter
            propertyText.font.capitalization: Font.MixedCase
            propertyText.font.pixelSize: Constants.SIZE_TEXT_LINK_LABEL
            propertyText.font.bold: true
        }
    }

    Button {
        id: rightButton
        text: ""
        width: Constants.WIDTH_BUTTON
        height: Constants.HEIGHT_BOTTOM_COMPONENT
        visible: text !== ""
        
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Constants.MARGIN_NOTIFICATION_CENTER / 2
        anchors.right: parent.right
        anchors.rightMargin: Constants.MARGIN_NOTIFICATION_CENTER

        font.pixelSize: Constants.SIZE_TEXT_FIELD
        font.family: lato.name
        font.capitalization: Font.MixedCase
        highlighted: activeFocus
    }
}