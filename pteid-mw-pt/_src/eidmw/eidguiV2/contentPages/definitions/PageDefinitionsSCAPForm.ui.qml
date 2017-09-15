import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {

    property alias propertyButtonLoadCompanyAttributes: buttonLoadCompanyAttributes
    property alias propertyButtonLoadEntityAttributes: buttonLoadEntityAttributes
    property alias propertyBusyIndicator: busyIndicator

    anchors.fill: parent
    Item {
        width: parent.width
        height: parent.height

        BusyIndicator {
            id: busyIndicator
            running: false
            anchors.centerIn: parent
            // BusyIndicator should be on top of all other content
            z: 1
        }

        Item {
            id: rowTop
            width: parent.width
            height: 20
        }

        Text {
            id: textTitle
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            text: "Atributos profissionais"
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_LABEL
            Layout.fillWidth: true
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        }
        Text {
            id: entitiesDescription
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: "Escolha a entidade fornecedora dos atributos"
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }

        Rectangle {
            id: rectangleEntities
            width: parent.width
            height: 120
            anchors.top: entitiesDescription.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY

            ListView {
                anchors.fill: parent
                model: entityAttributesModel
                delegate: attributeListDelegate
                highlight: Rectangle {
                    color: "lightsteelblue"
                    radius: 5
                }
                focus: true
            }
        }

        Item {
            id: rawButtonLoadEntityAttributes
            anchors.top: rectangleEntities.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonLoadEntityAttributes
                text: "Carregar atributos"
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            id: textCompaniesTitle
            anchors.top: rawButtonLoadEntityAttributes.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            text: "Atributos empresariais"
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_LABEL
            Layout.fillWidth: true
        }

        Rectangle {
            id: rectangleCompanies
            width: parent.width
            height: 120
            anchors.top: textCompaniesTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY

            ListView {
                anchors.fill: parent
                model: companyAttributesModel
                delegate: attributeListDelegate
                highlight: Rectangle {
                    color: "lightsteelblue"
                    radius: 5
                }
                focus: true
            }
        }

        Item {
            id: rawButtonLoadCompanyAttributes
            anchors.top: rectangleCompanies.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonLoadCompanyAttributes
                text: "Carregar atributos"
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
