import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
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
        width: parent.width
        height: parent.height * Constants.PAGE_SERVICES_MAIN_V_RELATIVE

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
            Component.onCompleted: visible = false
        }

        Item{
            id: rectMainLeftFile
            width: parent.width * 0.5 - Constants.SIZE_ROW_H_SPACE
            height: listViewFiles.height
                    + itemBottonsFiles.height
                    + 40
            x: Constants.SIZE_ROW_H_SPACE

            GroupBox {
                id: groupBoxFile
                width: parent.width
                height: parent.height
                title: "Selecione os ficheiros a assinar"

                Item{
                    id: itemOptionsFiles
                    width: parent.width
                    height: 4 * Constants.SIZE_V_COMPONENTS
                    anchors.horizontalCenter: parent.horizontalCenter

                    ListView {
                        id: listViewFiles
                        width: parent.width;
                        height: 4 * Constants.SIZE_V_COMPONENTS
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
                            text: "Arraste para esta zona o ficheiro a assinar \nou\n click para procurar o ficheiro"
                            font.bold: true
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: Constants.SIZE_TEXT_BODY
                            color: Constants.COLOR_TEXT_LABEL
                            visible: !fileLoaded
                        }
                    }
                    MouseArea {
                        id: mouseAreaItemOptionsFiles
                        width: parent.width
                        height: 4 * Constants.SIZE_V_COMPONENTS
                        enabled: !fileLoaded
                    }

                }
                Item{
                    id: itemBottonsFiles
                    width: parent.width
                    height: Constants.SIZE_V_COMPONENTS
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: itemOptionsFiles.bottom

                    Button {
                        id: buttonAdd
                        height: Constants.SIZE_V_COMPONENTS
                        text: "Adicionar ficheiro"
                        font.bold: false
                    }
                    Button {
                        id: buttonRemoveAll
                        height: Constants.SIZE_V_COMPONENTS
                        anchors.right: itemBottonsFiles.right
                        text: "Remover todos"
                        font.bold: false
                        enabled: fileLoaded
                    }
                }
            }
        }
        Item{
            id: rectMainLeftOptions
            width: parent.width * 0.5 - Constants.SIZE_ROW_H_SPACE
            height: parent.height - rectMainLeftFile.height
            anchors.top: rectMainLeftFile.bottom
            anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
            x: Constants.SIZE_ROW_H_SPACE

            GroupBox {
                id: groupBoxOptions
                anchors.fill: parent
                title: "Configurações"

                Item{
                    id: itemOptions
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE

                    Text {
                        id: textFormatSign
                        text: "Formato da Assinatura"
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        color: Constants.COLOR_TEXT_LABEL
                        height: Constants.SIZE_TEXT_LABEL
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    }
                    RadioButton {
                        id: radioButtonPADES
                        height: Constants.SIZE_V_COMPONENTS
                        anchors.top: textFormatSign.bottom
                        text: "PADES"
                        checked: true
                    }
                    RadioButton {
                        id: radioButtonXADES
                        height: Constants.SIZE_V_COMPONENTS
                        anchors.top: textFormatSign.bottom
                        anchors.left: radioButtonPADES.right
                        text: "XADES"
                    }
                    TextField {
                        id: textFieldReason
                        width: parent.width
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        anchors.top: radioButtonXADES.bottom
                        placeholderText:"Escreva a razão"
                    }
                    TextField {
                        id: textFieldLocal
                        width: parent.width
                        anchors.top: textFieldReason.bottom
                        placeholderText:"Escreva o local"
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                    }
                    Switch {
                        id: switchSignTemp
                        height: Constants.SIZE_V_COMPONENTS
                        anchors.top: textFieldLocal.bottom

                        text: "Adicionar selo temporal"
                    }
                    Switch {
                        id: switchSignAdd
                        height: Constants.SIZE_V_COMPONENTS
                        anchors.top: switchSignTemp.bottom
                        text: "Adicionar atribtutos profissionais"
                    }
                    Column {
                        anchors.top: switchSignAdd.bottom
                        width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        x: 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        visible: switchSignAdd.checked

                        CheckBox {
                            text: "Engenheiro Civil"
                            height: Constants.SIZE_V_COMPONENTS
                        }
                        CheckBox {
                            text: "Socio da Empresa Obras Prontas"
                            height: Constants.SIZE_V_COMPONENTS
                        }
                        CheckBox {
                            id: checkBox
                            text: "Presidente do Clube Futebol da Terra"
                            height: Constants.SIZE_V_COMPONENTS
                        }

                        Item{
                            anchors.top: checkBox.bottom
                            height: Constants.SIZE_V_COMPONENTS

                            Text {
                                id: textPreserv
                                text: "Preservar durante"
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: Constants.SIZE_TEXT_LABEL
                                color: Constants.COLOR_TEXT_LABEL
                                height: parent.height
                                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            ComboBox {
                                id: comboBoxPreserve
                                anchors.left: textPreserv.right
                                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                                height: parent.height
                                width: 60
                                // textPreservAnos.text is related with model values
                                // if model is changed textPreservAnos.text may be changed
                                model: [ "0", "1", "3", "5", "10" ]
                            }
                            Text {
                                id: textPreservAnos
                                text: comboBoxPreserve.currentIndex === 1 ?
                                          "ano":
                                          "anos"
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: Constants.SIZE_TEXT_LABEL
                                color: Constants.COLOR_TEXT_LABEL
                                height: parent.height
                                anchors.left: comboBoxPreserve.right
                                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }
            }
        }

        Item{
            id: rectMainRight
            width: parent.width * 0.5
            height: parent.height
            anchors.left: rectMainLeftFile.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            GroupBox {
                id: groupPre
                anchors.fill: parent
                title: qsTr("Pré-Visualização")

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
                }
                Image {
                    id: imageTabPreView
                    anchors.fill: parent
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/Pdfdemo.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                }
            }
            MouseArea {
                id: mouseAreaRectMainRigh
                anchors.fill: parent
            }
        }
    }

    Item {
        id: rowBottom
        width: parent.width
        height: Constants.SIZE_V_COMPONENTS
        anchors.top: rowMain.bottom
        anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

        Item{
            id: rectSignLeft
            width: parent.width  * 0.50
            height: parent.height
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
            }
        }
    }
}
