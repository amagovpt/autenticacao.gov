import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

    property string fileName: ""
    property variant filesArray:[]
    property bool fileLoaded: false

    property alias propertyFileDialog: fileDialog
    property alias propertyMouseAreaRectMain: mouseAreaRectMain
    property alias propertyButtonRemove: buttonRemove
    property alias propertyDropArea: dropArea

    Item {
        id: rowMain
        width: parent.width
        height: parent.height - Constants.SIZE_V_BOTTOM_COMPONENT - Constants.SIZE_ROW_V_SPACE

        DropArea {
            id: dropArea;
            anchors.fill: parent;
        }

        FileDialog {
            id: fileDialog
            title: "Escolha o ficheiro para assinar"
            folder: shortcuts.home
            modality : Qt.WindowModal
            selectMultiple: false
            Component.onCompleted: visible = false
        }

        Item{
            id: rectMain
            width: parent.width
            height: parent.height
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            x: Constants.SIZE_ROW_H_SPACE
            GroupBox {
                id: groupPre
                anchors.fill: parent
                title: fileLoaded ?
                           "Selecione o lugar da assinatura" :
                           "Selecione o ficheiro"
                Text {
                    id: textDragMsgImg
                    anchors.fill: parent
                    text: "Arraste para esta zona o ficheiro a assinar \nou\n click para procurar o ficheiro"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    color: Constants.COLOR_TEXT_LABEL
                    visible: !fileLoaded
                    font.family: lato.name
                }
                Image {
                    id: imageTabPreView
                    anchors.fill: parent
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/Pdfdemo.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: fileLoaded
                }
            }
            MouseArea {
                id: mouseAreaRectMain
                anchors.fill: parent
            }
        }
    }

    Item {
        id: rowBottom
        width: parent.width
        height: Constants.SIZE_V_BOTTOM_COMPONENT
        anchors.top: rowMain.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
        x: Constants.SIZE_ROW_H_SPACE

        Item{
            id: rectSignLeft
            width: parent.width  * 0.50 - Constants.SIZE_ROW_H_SPACE
            height: parent.height

            Button {
                id: buttonRemove
                text: "Remover ficheiro"
                width: parent.width
                height: parent.height
                anchors.right: parent.right
                enabled: fileLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
            }
        }
        Item{
            id: rectSignRight
            width: parent.width * 0.50
            height: parent.height
            anchors.left: rectSignLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Button {
                text: "Assinar"
                width: parent.width
                height: parent.height
                anchors.right: parent.right
                enabled: fileLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
            }
        }
    }
}
