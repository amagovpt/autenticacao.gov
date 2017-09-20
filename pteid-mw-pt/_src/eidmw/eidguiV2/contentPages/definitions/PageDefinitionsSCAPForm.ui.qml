import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {

    property alias propertyButtonLoadCompanyAttributes: buttonLoadCompanyAttributes
    property alias propertyButtonLoadEntityAttributes: buttonLoadEntityAttributes
    property alias propertyBusyIndicator: busyIndicator
    property alias propertyBusyIndicatorAttributes: busyIndicatorAttributes
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
        BusyIndicator {
            id: busyIndicatorAttributes
            running: false
            anchors.centerIn: parent
            // BusyIndicator should be on top of all other content
            z: 1
        }

        Item {
            id: rowTop
            width: parent.width
            height: parent.height * Constants.HEIGHT_DIFINITIONS_ATTRIBUTES_ROW_TOP_V_RELATIVE
                    + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                    * Constants.HEIGHT_DIFINITIONS_ATTRIBUTES_ROW_TOP_INC_RELATIVE
        }

        TabBar {
            id: bar
            anchors.top: rowTop.bottom
            width: parent.width
            currentIndex: 0
            TabButton {
                text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_ENTITIES")
                rightPadding: 2
                leftPadding: 2
            }
            TabButton {
                text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_COMPANY")
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
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_ENTITIES_MSG")
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
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_ENTITIES_BUTTON")
                        width: 2 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: false
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
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_COMPANY_MSG")
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
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_COMPANY_BUTTON")
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
