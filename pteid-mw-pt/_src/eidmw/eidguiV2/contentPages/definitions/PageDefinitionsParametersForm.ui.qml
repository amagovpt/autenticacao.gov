import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent
    Text {
        text: "Parameters"
        font.pointSize: Constants.SIZE_TEXT_BODY
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.family: lato.name
    }
}
