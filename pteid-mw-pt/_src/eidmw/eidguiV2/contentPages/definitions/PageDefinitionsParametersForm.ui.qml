import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.0
import QtGraphicalEffects 1.0


/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

// Work around to QTCREATORBUG-18321
import QtQuick.Templates 2.0 as T

Item {
    anchors.fill: parent

    property variant filesArray:[]
    property bool fileLoaded: false
    property alias propertyFileDialog: fileDialog
    property alias propertyMouseAreaPreCustom: mouseAreaPreCustom
    property alias propertyButtonAdd: buttonAdd
    property alias propertyButtonRemove: buttonRemove
    property alias propertyDropArea: dropArea
    property alias propertyRadioButtonDefault: radioButtonDefault
    property alias propertyRadioButtonCustom: radioButtonCustom

    Item {
        id: rowMain
        width: parent.width
        height: parent.height * Constants.PAGE_SERVICES_MAIN_V_RELATIVE

        T.ButtonGroup {
            id: radioGroup
        }

        Item{
            id: rectTop
            width: parent.width
            height: parent.height * 0.5 - Constants.SIZE_ROW_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            DropShadow {
                anchors.fill: rectPreDefault
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPreDefault
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPreDefault
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            RadioButton {
                id: radioButtonDefault
                width: parent.width * 0.5
                height: Constants.SIZE_V_COMPONENTS
                text: "Usar assinatura padrão"
                checked: true
                T.ButtonGroup.group: radioGroup
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
            }

            Rectangle {
                id: rectPreDefault
                width: parent.width
                height: parent.height - radioButtonDefault.height
                color: "white"
                anchors.top: radioButtonDefault.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Image {
                    id: imagePreDefault
                    width: parent.width
                    height: parent.height
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/CCdemo.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: true
                    opacity: radioButtonDefault.checked ? 1 : 0.3
                }

            }
        }

        Item{
            id: rectBottom
            width: parent.width
            height: parent.height * 0.5 - Constants.SIZE_ROW_V_SPACE
            anchors.top: rectTop.bottom
            anchors.topMargin: 3 * Constants.SIZE_ROW_V_SPACE

            DropArea {
                id: dropArea;
                anchors.fill: parent;
            }
            FileDialog {
                id: fileDialog
                title: "Escolha o ficheiro da assinatura personalizada"
                folder: shortcuts.home
                modality : Qt.WindowModal
                selectMultiple: false
                nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
                Component.onCompleted: visible = false
            }
            DropShadow {
                anchors.fill: rectPreCustom
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPreCustom
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPreCustom
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            RadioButton {
                id: radioButtonCustom
                width: parent.width * 0.5
                height: Constants.SIZE_V_COMPONENTS
                text: "Usar assinatura personalizada"
                enabled: fileLoaded
                T.ButtonGroup.group: radioGroup
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
            }
            Rectangle {
                id: rectPreCustom
                width: parent.width
                height: parent.height - radioButtonCustom.height
                color: "white"
                anchors.top: radioButtonCustom.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Image {
                    id: imagePreCustom
                    width: parent.width
                    height: parent.height
                            - Constants.SIZE_V_BOTTOM_COMPONENT
                            - 2 * Constants.SIZE_TEXT_V_SPACE
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/CCdemo_custom.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: fileLoaded
                    opacity: radioButtonCustom.checked ? 1 : 0.3
                }
                Text {
                    id: textDragMsgImg
                    width: parent.width
                    height: parent.height
                            - Constants.SIZE_V_BOTTOM_COMPONENT
                            - 2 * Constants.SIZE_TEXT_V_SPACE
                    text:
                        "Arraste para esta zona o ficheiro da assinatura personalizada
ou
clique para procurar o ficheiro
( Dimensão máxima da imagem: 185 x 41 px)"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    color: Constants.COLOR_TEXT_LABEL
                    visible: !fileLoaded
                    font.family: lato.name
                }
                MouseArea {
                    id: mouseAreaPreCustom
                    anchors.fill: parent
                }
                Item{
                    id: rectSignLeft
                    width: parent.width  * 0.50 - Constants.SIZE_ROW_H_SPACE
                    height: Constants.SIZE_V_BOTTOM_COMPONENT
                    anchors.top: imagePreCustom.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    x: Constants.SIZE_ROW_H_SPACE / 2

                    Button {
                        id: buttonRemove
                        text: "Remover assinatura"
                        width: parent.width
                        height: Constants.SIZE_V_BOTTOM_COMPONENT
                        anchors.right: parent.right
                        enabled: fileLoaded
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                }
                Item{
                    id: rectSignRight
                    width: parent.width * 0.50 - Constants.SIZE_ROW_H_SPACE
                    height: Constants.SIZE_V_BOTTOM_COMPONENT
                    anchors.top: imagePreCustom.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    anchors.left: rectSignLeft.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

                    Button {
                        id: buttonAdd
                        text: "Adicionar assinatura"
                        width: parent.width
                        height:Constants.SIZE_V_BOTTOM_COMPONENT
                        anchors.right: parent.right
                        enabled: !fileLoaded
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                }
            }
        }
    }
}
