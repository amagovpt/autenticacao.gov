import QtQuick 2.6

import "../scripts/Constants.js" as Constants

Item {
    id: linkRect
    property alias propertyText : linkText
    property string propertyLinkUrl : ""
    property string propertyAccessibleText : ""
    property string propertyAccessibleDescription : ""

    height: childrenRect.height
    TextEdit {
        id: linkText
        width: parent.width
        readOnly: true
        selectByMouse: true
        selectByKeyboard: true
        textFormat: Text.RichText
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.italic: false
        font.family: lato.name
        font.capitalization: Font.MixedCase
        font.bold: activeFocus? true : false
        color: Constants.COLOR_TEXT_BODY
        visible: parent.visible
        font.underline: mouseArea.containsMouse
        onLinkActivated: Qt.openUrlExternally(link)
        wrapMode: Text.WordWrap

        Accessible.role: Accessible.Link
        Accessible.name: propertyAccessibleText
        Accessible.description: propertyAccessibleDescription

        Keys.onSpacePressed: Qt.openUrlExternally(propertyLinkUrl)
        Keys.onReturnPressed: Qt.openUrlExternally(propertyLinkUrl)

    }
    MouseArea {
        id: mouseArea
        anchors.fill: linkText
        enabled: linkRect.visible
        acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
        cursorShape: linkText.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
