import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

    Item {
        id: rowMain
        width: parent.width
        height: parent.height * Constants.PAGE_SERVICES_MAIN_V_RELATIVE


        Item{
            id: rectMainTop
            width: parent.width
            height: parent.height * 0.5
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            GroupBox {
                id: groupPreTop
                anchors.fill: parent
                title: "Pre-visualização do selo de assinatura"

                Image {
                    id: imageTabPreView
                    anchors.fill: parent
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/CCdemo.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: true
                }
            }
        }
        Item{
            id: rectMainBottom
            width: parent.width
            height: parent.height * 0.5
            anchors.top: rectMainTop.bottom
            anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            GroupBox {
                id: groupPreBottom
                anchors.fill: parent
                title: "Personalização da Assinatura"

                Button {
                    id: buttonImgCustom
                    height: Constants.SIZE_V_COMPONENTS
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    font.bold: false
                    text: "Imagem personalizada"
                }

                Text {
                    id: textImgCustom
                    y: 10
                    anchors.left: buttonImgCustom.right
                    text: "Dimensão máxima da imagem: 185 x 41 px"
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    color: Constants.COLOR_TEXT_LABEL
                }

                Button {
                    id: buttonImgDefault
                    anchors.top: buttonImgCustom.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    height: Constants.SIZE_V_COMPONENTS
                    width: buttonImgCustom.width
                    font.bold: false
                    text: "Imagem por omissão"
                }

                Text {
                    id: textImgDefault
                    anchors.left: buttonImgDefault.right
                    text: ""
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    color: Constants.COLOR_TEXT_LABEL
                }

            }
        }
    }
}
