import QtQuick 2.6

import "../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent

        Item{
            id: rectTopLeft
            width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE - Constants.WIDTH_PHOTO_IMAGE
            height: parent.height
        }
        Item{
            id: rectToRight
            width: Constants.WIDTH_PHOTO_IMAGE + Constants.SIZE_ROW_H_SPACE
            height: parent.height
            anchors.left: rectTopLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Image {
                width: parent.width
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                source: "../images/logo_cartao_cidadao.png"
            }
        }
}
