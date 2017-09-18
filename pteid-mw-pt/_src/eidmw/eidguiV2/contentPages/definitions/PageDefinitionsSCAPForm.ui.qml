import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {

    property alias propertyButtonLoadCompanyAttributes: buttonLoadCompanyAttributes
    property alias propertyButtonLoadEntityAttributes: buttonLoadEntityAttributes
    property alias propertyBusyIndicator: busyIndicator
    property alias propertyBar : bar
    property alias propertyListViewEntities: listViewEntities

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
            height: 0
        }

        TabBar {
            id: bar
            anchors.top: rowTop.bottom
            width: parent.width
            currentIndex: 0
            TabButton {
                text: "Atributos profissionais"
                rightPadding: 2
                leftPadding: 2
            }
            TabButton {
                text: "Atributos empresariais"
                rightPadding: 2
                leftPadding: 2
            }
        }
        StackLayout {
            id: stackLayout
            width: parent.width
            height: parent.height - rowTop.height - bar.height
            currentIndex: bar.currentIndex
            anchors.top: bar.bottom

            //TODO: Add Scroll bar

            Item {
                id: tabProfessional
                Rectangle {
                    id: rowProfessional
                    width: parent.width
                    height: 5 * Constants.SIZE_TEXT_BODY

                    Text {
                        font.pixelSize: Constants.SIZE_TEXT_BODY
                        font.family: lato.name
                        text: "Escolha a entidade para a qual pretende carregar os atributos profissionais"
                        wrapMode: Text.Wrap
                        width: parent.width
                        color: Constants.COLOR_MAIN_BLUE
                        Layout.fillWidth: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                Item {
                    id: rectangleEntities
                    width: parent.width
                    height: stackLayout.height - rowProfessional.height - rawButtonLoadEntityAttributes.height
                    anchors.top: rowProfessional.bottom

                    ListView {
                        id: listViewEntities
                        anchors.fill: parent
                        model: entityAttributesModel
                        delegate: attributeListDelegate
                        focus: true
                        spacing: 10

                    }
                }

                Item {
                    id: rawButtonLoadEntityAttributes
                    anchors.top: rectangleEntities.bottom
                    width: parent.width
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    Button {
                        id: buttonLoadEntityAttributes
                        text: "Carregar atributos profissionais"
                        width: 2 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
            Item {
                id: tabCompanies

                Item {
                    id: rowCompanies
                    width: parent.width
                    height: 5 * Constants.SIZE_TEXT_BODY
                    Text {
                        font.pixelSize: Constants.SIZE_TEXT_BODY
                        font.family: lato.name
                        text: "Lista das entidades para a qual tem atributos empresariais"
                        wrapMode: Text.Wrap
                        width: parent.width
                        color: Constants.COLOR_MAIN_BLUE
                        Layout.fillWidth: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                Item {
                    id: rectangleCompanies
                    width: parent.width
                    height: stackLayout.height - rowCompanies.height - rawButtonLoadCompanyAttributes.height
                    anchors.top: rowCompanies.bottom

                    ListView {
                        anchors.fill: parent
                        model: companyAttributesModel
                        delegate: attributeListDelegateCompanies
                        focus: true
                        spacing: 10
                    }
                }

                Item {
                    id: rawButtonLoadCompanyAttributes
                    anchors.top: rectangleCompanies.bottom
                    width: parent.width
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    Button {
                        id: buttonLoadCompanyAttributes
                        text: "Carregar atributos empresariais"
                        width: 2 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

            }
        }
    }
}
