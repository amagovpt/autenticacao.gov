import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {
    property alias propertyStackLayout: stackLayout

    property alias propertyButtonLoadEntityAttributes: buttonLoadEntityAttributes
    property alias propertyButtonLoadCompanyAttributes: buttonLoadCompanyAttributes

    property alias propertyButtonRemoveEntityAttributes: buttonRemoveEntityAttributes
    property alias propertyButtonLoadCompanyAttributesOAuth: buttonLoadCompanyAttributesOAuth
    property alias propertyButtonLoadEntityAttributesOAuth: buttonLoadEntityAttributesOAuth
    property alias propertyButtonRemoveCompanyAttributes: buttonRemoveCompanyAttributes

    property alias propertyBusyIndicator: busyIndicator
    property alias propertyBusyIndicatorAttributes: busyIndicatorAttributes
    property alias propertyBar: bar
    property alias propertyListViewEntities: listViewEntities
    property alias propertyListViewCompanies: listViewCompanies
    property alias propertyCompaniesListViewScroll: companiesListViewScroll
    property alias propertyEntitiesListViewScroll: entitiesListViewScroll
    property alias propertyMouseArealinkScapCompanies: mouseArealinkScapCompanies
    property alias propertyMouseArealinkScapEntities: mouseArealinkScapEntities
    property alias propertylinkScapEntities: linkScapEntities
    property alias propertylinkScapCompanies: linkScapCompanies

    property alias propertyTabButtonEntities: tabButtonEntities
    property alias propertyEntitiesText: entitiesText

    property alias propertyTabButtonCompanies: tabButtonCompanies
    property alias propertyCompaniesText: companiesText

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
            height: parent.height * Constants.HEIGHT_DEFINITIONS_ATTRIBUTES_ROW_TOP_V_RELATIVE
                    + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                    * Constants.HEIGHT_DEFINITIONS_ATTRIBUTES_ROW_TOP_INC_RELATIVE
        }

        TabBar {
            id: bar
            anchors.top: rowTop.bottom
            width: parent.width
            currentIndex: 0
            TabButton {
                id: tabButtonEntities
                text: qsTranslate("PageDefinitionsSCAP",
                                  "STR_SCAP_ATTRIBUTES_ENTITIES")
                font.bold: activeFocus ? true : false
                rightPadding: 2
                leftPadding: 2
                Accessible.role: Accessible.MenuItem
                Accessible.name: text
                KeyNavigation.tab: tabButtonCompanies
                // @disable-check M222
                Keys.onReturnPressed: tabDetectKeys(event.key)
                // @disable-check M222
                Keys.onSpacePressed: tabDetectKeys(event.key)
            }
            TabButton {
                id: tabButtonCompanies
                text: qsTranslate("PageDefinitionsSCAP",
                                  "STR_SCAP_ATTRIBUTES_COMPANY")
                font.bold: activeFocus? true : false
                rightPadding: 2
                leftPadding: 2
                Accessible.role: Accessible.MenuItem
                Accessible.name: text
                KeyNavigation.tab: tabButtonEntities
                // @disable-check M222
                Keys.onReturnPressed: tabDetectKeys(event.key)
                // @disable-check M222
                Keys.onSpacePressed: tabDetectKeys(event.key)
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
                id: tabEntities
                Rectangle {
                    id: rowEntities
                    width: parent.width
                    height: 5 * Constants.SIZE_TEXT_BODY

                    Text {
                        id: entitiesText
                        font.pixelSize: Constants.SIZE_TEXT_BODY
                        font.family: lato.name
                        font.bold: entitiesText.focus ? true : false
                        text: qsTranslate("PageDefinitionsSCAP",
                                          "STR_SCAP_ATTRIBUTES_ENTITIES_MSG")
                        wrapMode: Text.Wrap
                        width: parent.width
                        color: Constants.COLOR_MAIN_BLUE
                        Layout.fillWidth: true
                        anchors.verticalCenter: parent.verticalCenter
                        Accessible.role: Accessible.StaticText
                        Accessible.name: text
                        KeyNavigation.tab: listViewEntities.count
                                           > 0 ? listViewEntities : linkScapEntities
                    }
                }
                Item {
                    id: rectangleEntities
                    width: parent.width
                    height: stackLayout.height - rowEntities.height
                            - rawButtonLoadEntityAttributes.height - Constants.SIZE_ROW_V_SPACE
                            - (propertyPageLoader.propertyBackupFromSignaturePage ? Constants.SIZE_TEXT_FIELD + Constants.SIZE_ROW_V_SPACE : 0)
                    anchors.top: rowEntities.bottom

                    ListView {
                        id: listViewEntities
                        width: parent.width
                        height: parent.height
                        clip: true
                        model: entityAttributesModel
                        delegate: attributeListDelegateEntities
                        focus: false
                        spacing: 10
                        boundsBehavior: Flickable.StopAtBounds
                        highlightMoveDuration: 1000
                        highlightMoveVelocity: 1000

                        ScrollBar.vertical: ScrollBar {
                            id: entitiesListViewScroll
                            active: true
                            visible: true
                            width: Constants.SIZE_TEXT_FIELD_H_SPACE
                        }
                        Accessible.role: Accessible.List
                        Accessible.name: qsTranslate(
                                             "PageDefinitionsSCAP",
                                             "STR_SCAP_ATTRIBUTES_ENTITIES_MSG")
                        KeyNavigation.tab: linkScapEntities
                    }
                }

                Item {
                    id: rectSignaturePageLinkEntities
                    width: parent.width
                    height: (propertyPageLoader.propertyBackupFromSignaturePage ? Constants.SIZE_TEXT_FIELD : 0)
                    anchors.top: rectangleEntities.bottom
                    anchors.margins: (propertyPageLoader.propertyBackupFromSignaturePage ? Constants.SIZE_ROW_V_SPACE : 0)
                    visible: propertyPageLoader.propertyBackupFromSignaturePage
                    MouseArea {
                        id: mouseArealinkScapEntities
                        anchors.fill: parent
                        enabled: propertyPageLoader.propertyBackupFromSignaturePage
                        hoverEnabled: true
                    }
                    Text {
                        id: linkScapEntities
                        text: qsTranslate("PageDefinitionsSCAP",
                                          "STR_BACK_TO_SIGNATURE_PAGE")
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        color: Constants.COLOR_MAIN_BLUE
                        visible: propertyPageLoader.propertyBackupFromSignaturePage
                        font.underline: mouseArealinkScapEntities.containsMouse
                        font.bold: activeFocus
                        KeyNavigation.tab: buttonRemoveEntityAttributes
                    }
                }
                Item {
                    id: rawButtonLoadEntityAttributes
                    anchors.top: rectSignaturePageLinkEntities.bottom
                    anchors.margins: Constants.SIZE_ROW_V_SPACE
                    width: parent.width - entitiesListViewScroll.width - listViewEntities.spacing
                    height: Constants.HEIGHT_SIGN_BOTTOM_COMPONENT

                    Button {
                        id: buttonRemoveEntityAttributes
                        text: qsTranslate("PageDefinitionsSCAP",
                                          "STR_SCAP_ATTRIBUTES_BUTTON_REMOVE")
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        anchors.left: parent.left
                        enabled: true
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: buttonLoadEntityAttributes.enabled ? buttonLoadEntityAttributes : buttonLoadEntityAttributesOAuth
                    }
                    Button {
                        id: buttonLoadEntityAttributes
                        text: qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD") + "\n" +
                              qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD_CC")
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: isAnyEntitySelected()
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                    }
                    Button {
                        id: buttonLoadEntityAttributesOAuth
                        text: qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD") + "\n" +
                              qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD_CMD")
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        anchors.right: parent.right
                        enabled: isAnyEntitySelected()
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: entitiesText
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
                        id: companiesText
                        font.pixelSize: Constants.SIZE_TEXT_BODY
                        font.family: lato.name
                        font.bold: focus
                        text: qsTranslate("PageDefinitionsSCAP",
                                          "STR_SCAP_ATTRIBUTES_COMPANY_MSG")
                        wrapMode: Text.Wrap
                        width: parent.width
                        color: Constants.COLOR_MAIN_BLUE
                        Layout.fillWidth: true
                        anchors.verticalCenter: parent.verticalCenter
                        Accessible.role: Accessible.StaticText
                        Accessible.name: text
                        KeyNavigation.tab: listViewCompanies.count
                                           > 0 ? listViewCompanies : linkScapCompanies
                    }
                }
                Item {
                    id: rectangleCompanies
                    width: parent.width
                    height: stackLayout.height - rowCompanies.height
                            - rawButtonLoadCompanyAttributes.height - Constants.SIZE_ROW_V_SPACE
                            - (propertyPageLoader.propertyBackupFromSignaturePage ? Constants.SIZE_TEXT_FIELD + Constants.SIZE_ROW_V_SPACE : 0)
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
                        highlightMoveDuration: 1000
                        highlightMoveVelocity: 1000

                        ScrollBar.vertical: ScrollBar {
                            id: companiesListViewScroll
                            active: true
                            visible: true
                            width: Constants.SIZE_TEXT_FIELD_H_SPACE
                        }
                        Accessible.role: Accessible.List
                        Accessible.name: qsTranslate(
                                             "PageDefinitionsSCAP",
                                             "STR_SCAP_ATTRIBUTES_COMPANY_MSG")
                        KeyNavigation.tab: linkScapCompanies
                    }
                }

                Item {
                    id: rectSignaturePageLinkCompanies
                    width: parent.width
                    height: (propertyPageLoader.propertyBackupFromSignaturePage ? Constants.SIZE_TEXT_FIELD : 0)
                    anchors.top: rectangleCompanies.bottom
                    anchors.margins: (propertyPageLoader.propertyBackupFromSignaturePage ? Constants.SIZE_ROW_V_SPACE : 0)
                    visible: propertyPageLoader.propertyBackupFromSignaturePage
                    MouseArea {
                        id: mouseArealinkScapCompanies
                        anchors.fill: parent
                        enabled: propertyPageLoader.propertyBackupFromSignaturePage
                        hoverEnabled: true
                    }
                    Text {
                        id: linkScapCompanies
                        text: qsTranslate("PageDefinitionsSCAP",
                                          "STR_BACK_TO_SIGNATURE_PAGE")
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        font.underline: mouseArealinkScapCompanies.containsMouse
                        font.bold: activeFocus
                        color: Constants.COLOR_MAIN_BLUE
                        visible: propertyPageLoader.propertyBackupFromSignaturePage
                        KeyNavigation.tab: buttonRemoveCompanyAttributes
                    }
                }
                Item {
                    id: rawButtonLoadCompanyAttributes
                    anchors.top: rectSignaturePageLinkCompanies.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    width: parent.width - companiesListViewScroll.width - listViewCompanies.spacing
                    height: Constants.HEIGHT_SIGN_BOTTOM_COMPONENT
                    Button {
                        id: buttonRemoveCompanyAttributes
                        text: qsTranslate("PageDefinitionsSCAP",
                                          "STR_SCAP_ATTRIBUTES_BUTTON_REMOVE")
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        anchors.left: parent.left
                        enabled: true
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: buttonLoadCompanyAttributes
                    }
                    Button {
                        id: buttonLoadCompanyAttributes
                        text: qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD") + "\n" +
                              qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD_CC")
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: isCardPresent
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                    }
                    Button {
                        id: buttonLoadCompanyAttributesOAuth
                        text: qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD") + "\n" +
                              qsTranslate("PageDefinitionsSCAP","STR_SCAP_BUTTON_LOAD_CMD")
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        anchors.right: parent.right
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: companiesText
                    }
                }
            }
        }
    }
}
