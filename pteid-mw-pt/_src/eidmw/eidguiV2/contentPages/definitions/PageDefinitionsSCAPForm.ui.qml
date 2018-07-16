import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {

    property alias propertyButtonLoadCompanyAttributes: buttonLoadCompanyAttributes
    property alias propertyButtonLoadEntityAttributes: buttonLoadEntityAttributes
    property alias propertyButtonRemoveCompanyAttributes: buttonRemoveCompanyAttributes
    property alias propertyButtonRemoveEntityAttributes: buttonRemoveEntityAttributes

    property alias propertyBusyIndicator: busyIndicator
    property alias propertyBusyIndicatorAttributes: busyIndicatorAttributes
    property alias propertyBar: bar
    property alias propertyListViewEntities: listViewEntities
    property alias propertyCompaniesListViewScroll: companiesListViewScroll
    property alias propertyEntitiesListViewScroll: entitiesListViewScroll

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
                        font.pointSize: Constants.SIZE_TEXT_BODY
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
                        - Constants. SIZE_ROW_V_SPACE
                    anchors.top: rowProfessional.bottom

                    ListView {
                        id: listViewEntities
                        width: parent.width
                        height: parent.height
                        clip: true
                        model: entityAttributesModel
                        delegate: attributeListDelegate
                        focus: true
                        spacing: 10
                        boundsBehavior: Flickable.StopAtBounds

                        ScrollBar.vertical: ScrollBar {
                            id: entitiesListViewScroll
                            active: true
                            visible: true
                            width: Constants.SIZE_TEXT_FIELD_H_SPACE
                        }
                    }
                }

                Item {
                    id: rawButtonLoadEntityAttributes
                    anchors.top: rectangleEntities.bottom
                    anchors.margins: Constants. SIZE_ROW_V_SPACE
                    width: parent.width - entitiesListViewScroll.width - listViewEntities.spacing
                    height: Constants.HEIGHT_BOTTOM_COMPONENT

                    Button {
                        id: buttonRemoveEntityAttributes
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_BUTTON_REMOVE")
                        width: 1.4 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pointSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.left: parent.left
                        enabled: true
                    }
                    Button {
                        id: buttonLoadEntityAttributes
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_ENTITIES_BUTTON_LOAD")
                        width: 1.4 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pointSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.right: parent.right
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
                        font.pointSize: Constants.SIZE_TEXT_BODY
                        font.family: lato.name
                        text: qsTranslate("PageDifinitionsSCAP",
                                          "STR_SCAP_ATTRIBUTES_COMPANY_MSG")
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
                        - Constants. SIZE_ROW_V_SPACE
                    anchors.top: rowCompanies.bottom

                    ListView {
                        id: listViewCompanies
                        width: parent.width
                        height: parent.height
                        clip: true
                        model: companyAttributesModel
                        delegate: attributeListDelegateCompanies
                        focus: true
                        spacing: 10
                        boundsBehavior: Flickable.StopAtBounds

                        ScrollBar.vertical: ScrollBar {
                            id: companiesListViewScroll
                            active: true
                            visible: true
                            width: Constants.SIZE_TEXT_FIELD_H_SPACE
                        }
                    }
                }

                Item {
                    id: rawButtonLoadCompanyAttributes
                    anchors.top: rectangleCompanies.bottom
                    anchors.topMargin: Constants. SIZE_ROW_V_SPACE
                    width: parent.width - companiesListViewScroll.width - listViewCompanies.spacing
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    Button {
                        id: buttonRemoveCompanyAttributes
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_BUTTON_REMOVE")
                        width: 1.4 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pointSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.left: parent.left
                        enabled: true
                    }
                    Button {
                        id: buttonLoadCompanyAttributes
                        text: qsTranslate("PageDifinitionsSCAP","STR_SCAP_ATTRIBUTES_COMPANY_BUTTON_LOAD")
                        width: 1.4 * Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pointSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.right: parent.right
                    }
                }
            }
        }
    }
}
