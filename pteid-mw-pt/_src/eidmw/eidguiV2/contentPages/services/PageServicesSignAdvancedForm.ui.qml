import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    id: mainItem
    anchors.fill: parent

    property variant filesArray:[]
    property bool fileLoaded: false

    property alias propertyFileDialog: fileDialog
    property alias propertyMouseAreaRectMainRigh: mouseAreaRectMainRigh
    property alias propertyMouseAreaItemOptionsFiles: mouseAreaItemOptionsFiles
    property alias propertyTextDragMsgListView: textDragMsgListView
    property alias propertyListViewFiles: listViewFiles
    property alias propertyFilesListViewScroll: filesListViewScroll
    property alias propertyButtonAdd: buttonAdd
    property alias propertyButtonRemoveAll: buttonRemoveAll

    property alias propertyDropArea: dropArea

    Item {
        id: rowMain
        width: parent.width - Constants.SIZE_ROW_H_SPACE
        height: parent.height

        // Expanded menu need a Horizontal space to Main Menu
        x: Constants.SIZE_ROW_H_SPACE

        DropArea {
            id: dropArea;
            anchors.fill: parent;
        }

        FileDialog {
            id: fileDialog
            title: "Escolha o ficheiro para assinar"
            folder: shortcuts.home
            modality : Qt.WindowModal
            selectMultiple: true
            nameFilters: [ "PDF document (*.pdf)", "All files (*)" ]
            Component.onCompleted: visible = false
        }

        Item{
            id: rectMainLeftFile
            width: parent.width * 0.5 - Constants.SIZE_ROW_H_SPACE
            height: titleSelectFile.height
                    + rectFile.height

            DropShadow {
                anchors.fill: rectFile
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectFile
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectFile
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            Text {
                id: titleSelectFile
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Selecione os ficheiros a assinar"
            }
            Rectangle {
                id: rectFile
                width: parent.width
                height: itemOptionsFiles.height +
                        itemBottonsFiles.height

                color: "white"
                anchors.top: titleSelectFile.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Item{
                    id: itemOptionsFiles
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: 4 * Constants.SIZE_V_URL_FILES
                    anchors.horizontalCenter: parent.horizontalCenter

                    ListView {
                        id: listViewFiles
                        y: Constants.SIZE_TEXT_V_SPACE
                        width: parent.width;
                        height: parent.height
                        clip: true
                        spacing: Constants.SIZE_LISTVIEW_SPACING
                        boundsBehavior: Flickable.StopAtBounds
                        model: filesModel
                        delegate: listViewFilesDelegate

                        ScrollBar.vertical: ScrollBar {
                            id: filesListViewScroll
                            hoverEnabled: true
                            active: hovered || pressed
                        }
                        Text {
                            id: textDragMsgListView
                            anchors.fill: parent
                            text: "Arraste para esta zona o ficheiro a assinar \nou\n clique para procurar o ficheiro"
                            font.bold: true
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: Constants.SIZE_TEXT_BODY
                            font.family: lato.name
                            color: Constants.COLOR_TEXT_LABEL
                            visible: !fileLoaded
                        }
                    }
                    MouseArea {
                        id: mouseAreaItemOptionsFiles
                        width: parent.width
                        height: parent.height
                        enabled: !fileLoaded
                    }

                }
                Item{
                    id: itemBottonsFiles
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: itemOptionsFiles.bottom

                    Button {
                        id: buttonAdd
                        width: Constants.WIDTH_BUTTON
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        text: "Adicionar ficheiro"
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                    Button {
                        id: buttonRemoveAll
                        width: Constants.WIDTH_BUTTON
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        anchors.right: itemBottonsFiles.right
                        text: "Remover todos"
                        enabled: fileLoaded
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                }
            }
        }
        Item{
            id: rectMainLeftOptions
            width: parent.width * 0.5 - Constants.SIZE_ROW_H_SPACE
            height: mainItem.height - rectMainLeftFile.height
                    - 2 * Constants.SIZE_ROW_V_SPACE
            anchors.top: rectMainLeftFile.bottom
            anchors.topMargin: 1.5 * Constants.SIZE_ROW_V_SPACE

            DropShadow {
                anchors.fill: rectOptions
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectOptions
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectOptions
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            Text {
                id: titleConf
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Configurações"
            }

            Rectangle {
                id: rectOptions
                width: parent.width
                height: parent.height - titleConf.height
                enabled: fileLoaded
                color: "white"
                anchors.top: titleConf.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Item{
                    id: itemOptions
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    y: Constants.SIZE_TEXT_FIELD_V_SPACE
                    height: parent.height
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        id: textFormatSign
                        text: "Formato\nda Assinatura"
                        color: Constants.COLOR_TEXT_BODY
                        height: Constants.SIZE_TEXT_LABEL
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE * 0.5
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    RadioButton {
                        id: radioButtonPADES
                        x: 90
                        height: Constants.SIZE_V_URL_FILES
                        anchors.top: textFormatSign.bottom
                        text: "Pdf"
                        anchors.topMargin: -12
                        checked: true
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    RadioButton {
                        id: radioButtonXADES
                        height: Constants.SIZE_V_URL_FILES
                        anchors.top: textFormatSign.bottom
                        anchors.left: radioButtonPADES.right
                        text: "Outros ficheiros"
                        anchors.leftMargin: 10
                        anchors.topMargin: -12
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    TextField {
                        id: textFieldReason
                        width: parent.width
                        font.italic: textFieldReason.text === "" ? true: false
                        anchors.top: radioButtonXADES.bottom
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE * 0.5
                        placeholderText:"Motivo?"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    TextField {
                        id: textFieldLocal
                        width: parent.width
                        font.italic: textFieldLocal.text === "" ? true: false
                        anchors.top: textFieldReason.bottom
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE * 0.5
                        placeholderText:"Localidade?"
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        enabled: fileLoaded
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                    Switch {
                        id: switchSignTemp
                        height: Constants.SIZE_V_URL_FILES
                        anchors.top: textFieldLocal.bottom
                        text: "Adicionar selo temporal"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    Switch {
                        id: switchSignAdd
                        height: Constants.SIZE_V_URL_FILES
                        anchors.top: switchSignTemp.bottom
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                        text: "Adicionar atributos profissionais"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    Column {
                        id: columAttributes
                        anchors.top: switchSignAdd.bottom
                        width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        x: 6 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        visible: switchSignAdd.checked

                        CheckBox {
                            text: "Engenheiro Civil"
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                        }
                        CheckBox {
                            text: "Socio da Empresa Obras Prontas"
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                        }
                        CheckBox {
                            id: checkBox
                            text: "Presidente do Clube Futebol da Terra"
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                        }
                    }
                    Row {
                        id: row
                        anchors.top: columAttributes.bottom
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE * 0.5
                        width: parent.width
                        height: 30
                        spacing: 5
                        x: 40
                        visible: switchSignAdd.checked

                        Switch {
                            id: switchPreserv
                            text: ""
                            spacing: -10
                            anchors.verticalCenter: parent.verticalCenter
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            height: 30
                        }
                        Text {
                            id: textPreserv1
                            x: 35
                            text: "Preservar"
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: false
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            color: Constants.COLOR_TEXT_BODY
                        }
                        Text {
                            id: textPreserv2
                            text: "durante"
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: false
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            color: Constants.COLOR_TEXT_BODY
                            visible: switchPreserv.checked
                        }
                        ComboBox {
                            id: comboBoxPreserve
                            width: 80
                            anchors.verticalCenter: parent.verticalCenter
                            height: 30
                            // textPreservAnos.text is related with model values
                            // if model is changed textPreservAnos.text may be changed
                            model: [ "0", "1", "3", "5", "10" ]
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            visible: switchPreserv.checked

                        }
                        Text {
                            id: textPreservAnos
                            text: comboBoxPreserve.currentIndex === 1 ?
                                      "ano":
                                      "anos"
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: false
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            color: Constants.COLOR_TEXT_BODY
                            visible: switchPreserv.checked
                        }

                    }
                }
            }
        }

        Item{
            id: rectMainRight
            width: parent.width * 0.5
            height: parent.height - 2 * Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.left: rectMainLeftFile.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            DropShadow {
                anchors.fill: rectPre
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPre
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPre
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            Text {
                id: titlePre
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Pré-Visualização"
            }

            Rectangle {
                id: rectPre
                width: parent.width
                height: parent.height - titlePre.height - Constants.SIZE_TEXT_V_SPACE
                enabled: fileLoaded
                color: "white"
                anchors.top: titlePre.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Text {
                    id: textDragMsgImg
                    anchors.fill: parent
                    text: "Arraste para esta zona o ficheiro a assinar \nou\n clique para procurar o ficheiro"
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
                id: mouseAreaRectMainRigh
                anchors.fill: parent
            }
        }
        Item {
            id: rowBottom
            width: rectMainRight.width
            height: 2 * Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.top: rectMainRight.bottom
            anchors.left: rectMainRight.left

            Item{
                id: rectSignOptions
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.left: parent.left
                Item{
                    id: itemCheckSignReduced
                    width: parent.width * 0.5
                    height: parent.height
                    anchors.top: parent.top
                    Switch {
                        id: checkSignReduced
                        text: "Assinatura reduzida"
                        spacing: 0
                        padding: 0
                        rightPadding: 0
                        bottomPadding: 0
                        topPadding: 0
                        leftPadding: 0
                        height: Constants.SIZE_V_URL_FILES
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: fileLoaded
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                Item{
                    id: itemCheckSignShow
                    width: parent.width * 0.5
                    height: parent.height
                    anchors.left: itemCheckSignReduced.right
                    anchors.top: parent.top
                    Switch {
                        id: checkSignShow
                        text: "Assinatura visível"
                        spacing: 0
                        padding: 0
                        rightPadding: 0
                        bottomPadding: 0
                        topPadding: 0
                        leftPadding: 0
                        height: Constants.SIZE_V_URL_FILES
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: fileLoaded
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

            }
            Item{
                id: rectSign
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.left: parent.left
                anchors.top: rectSignOptions.bottom

                Button {
                    text: "Assinar"
                    y: 5
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    enabled: fileLoaded
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }


}
