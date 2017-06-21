import QtQuick 2.6

import "../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent

    Item{
        id: rectTopLeft
        width: parent.width - Constants.SIZE_ROW_H_SPACE - Constants.SIZE_IMAGE_LOGO_CC_WIDTH
        height: parent.height
    }
    Item{
        id: rectToRight
        width: Constants.SIZE_IMAGE_LOGO_CC_WIDTH
        height: parent.height
        anchors.left: rectTopLeft.right
        anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
        Image {
            antialiasing: true
            width: Constants.SIZE_IMAGE_LOGO_CC_WIDTH
            height: Constants.SIZE_IMAGE_LOGO_CC_HEIGHT
            fillMode: Image.PreserveAspectFit
            source: "../images/logo_CC.png"
        }
    }
}
