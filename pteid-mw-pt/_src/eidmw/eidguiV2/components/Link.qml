import QtQuick 2.6

import "../scripts/Constants.js" as Constants

Item {
    id: linkRect
    property alias propertyText : linkText 

    height: childrenRect.height
	Text {
		id: linkText
		text: parent.text
		font.pixelSize: Constants.SIZE_TEXT_BODY
        font.italic: false
        font.family: lato.name
		font.capitalization: Font.MixedCase
		color: Constants.COLOR_TEXT_BODY
		visible: parent.visible
		font.underline: mouseArea.containsMouse
        onLinkActivated: Qt.openUrlExternally(link)
        wrapMode: Text.WordWrap
	}
    MouseArea {
        id: mouseArea
        anchors.fill: linkText
        enabled: linkRect.visible
        acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
        cursorShape: linkText.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
