import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants


Item {

    property alias propertyrectAppCache: rectAppCache
    property alias propertyButtonRemoveAppCache: buttonRemoveAppCache
    property alias propertyBusyIndicator : busyIndication
    property alias propertyButtonRemoveSCAPCache: buttonRemoveSCAPCache

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DIFINITIONS_APP_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DIFINITIONS_APP_ROW_TOP_INC_RELATIVE
    }

    Item {
        id: rowMain
        width: parent.width
        height: parent.height
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        anchors.top: rowTop.bottom
        
        BusyIndicator {
            id: busyIndication
            running: false
            anchors.centerIn: parent
            // BusyIndicator should be on top of all other content
            z: 1
        }

        
        Item{
            id: rectAppCache
            width: parent.width
            height: buttonRemoveAppCache.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

            Text {
                id: dateAppCache
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDataApp","STR_CACHE_TITLE") + controler.autoTr
            }
            DropShadow {
                anchors.fill: rowAppCache
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rowAppCache
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rowAppCache
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rowAppCache
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL + Constants.SIZE_TEXT_V_SPACE + 4
                        * Constants.SIZE_TEXT_FIELD
                anchors.top: dateAppCache.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                Rectangle {
                    id: rectAppCacheText
                    width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.50
                    anchors.left:rowAppCache.left
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Text {
                        id: cacheAppTextField
                        text: qsTranslate("PageDataApp","STR_APP_CACHE_TEXT")
                        width: parent.width
                        height: parent.height
                        font.capitalization: Font.MixedCase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        wrapMode: Text.WordWrap
                    }
                }
                Rectangle {
                    id: rectAppCacheButton
                    width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.50
                    anchors.left: rectAppCacheText.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Button {
                        id: buttonRemoveAppCache
                        text: qsTranslate("PageDataApp","STR_CLEAR_CACHE") + controler.autoTr
                        width: parent.width
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }                
                }
            }
        }
        Item{
            id: rectSCAPCache
            width: parent.width
            height: buttonRemoveSCAPCache.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.topMargin: 6*Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP
            anchors.top : rectAppCache.bottom

            Text {
                id: dateSCAPCache
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDataApp","STR_SCAP_CACHE_TITLE") + controler.autoTr
            }
            DropShadow {
                anchors.fill: rowSCAPCache
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rowSCAPCache
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rowSCAPCache
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rowSCAPCache
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL + Constants.SIZE_TEXT_V_SPACE + 4
                        * Constants.SIZE_TEXT_FIELD
                anchors.top: dateSCAPCache.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                Rectangle {
                    id: rectSCAPCacheText
                    width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.50
                    anchors.left:rowSCAPCache.left
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Text {
                        id: cacheSCAPTextField
                        text: qsTranslate("PageDataApp","STR_SCAP_CACHE_TEXT")
                        width: parent.width
                        height: parent.height
                        font.capitalization: Font.MixedCase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        wrapMode: Text.WordWrap
                    }
                }
                Rectangle {
                    id: rectSCAPCacheButton
                    width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.50
                    anchors.left: rectSCAPCacheText.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Button {
                        id: buttonRemoveSCAPCache
                        text: qsTranslate("PageDifiniionsSCAP","STR_SCAP_ATTRIBUTES_BUTTON_REMOVE") + controler.autoTr
                        width: parent.width
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }                
                }
            }
        }
    }
}
