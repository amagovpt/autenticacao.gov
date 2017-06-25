import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    id: mainWindow
    property alias propertyPinCodesListView: pinCodesListView
    property alias propertyPinCodesViewLeft: pinCodesViewLeft

    property alias propertyButtonModify : buttonModify
    property alias propertyButtonTest: buttonTest

    anchors.fill: parent

    /* Pin Codes List View */
    Item {
        id: pinCodesViewLeft
        width: parent.width * 0.5
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter

        Item {
            id: rectListView
            width: parent.width
            height: pinCodesListView.count *
                    mainView.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
            anchors.verticalCenter: parent.verticalCenter
            ListView {
                id: pinCodesListView
                width: parent.width * 0.9
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                model: PinCodesModel {}
                delegate: pinCodesMenuDelegate
                highlight: Rectangle {
                    color: Constants.COLOR_BACKGROUND_SUB_MENU
                    radius: 0
                }
                focus: true
            }
        }
    }

    /* Pin Codes Right */
    Item {
        id: pinCodesViewRight
        width: parent.width * 0.5
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: pinCodesViewLeft.right

        Item{
            width: parent.width
            height:rectType.height + rectPinID.height + rectState.height
                   + buttonTest.height + buttonModify.height
                   + 4 * Constants.SIZE_ROW_V_SPACE
            anchors.verticalCenter: parent.verticalCenter

            Item{
                id: rectType
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD

                Components.LabelTextBoxForm{
                    propertyDateText.text: "Tipo de Código PIN"
                    propertyDateField.text: "PIN Autenticação"
                }
            }
            Item{
                id: rectPinID
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD

                anchors.top: rectType.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    propertyDateText.text: "Identificação do código PIN"
                    propertyDateField.text: "1"
                }
            }
            Item {
                id: rectState
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD

                anchors.top: rectPinID.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm {
                    propertyDateText.text: "Estado do Código PIN"
                    propertyDateField.text: "Restam 3 tentativas"
                }
            }
            Button {
                id: buttonTest
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.top: rectState.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Testar PIN"
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
            }

            Button {
                id: buttonModify
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.top: buttonTest.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Modificar PIN"
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
            }
        }
    }
}
