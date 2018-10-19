import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants


Item {

    property alias propertyRectCache: rectCache
    property alias propertyButtonFlushCache: buttonFlushCache
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
            id: rectCache
            width: parent.width
            height: buttonFlushCache.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

            Text {
                id: dateCache
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDataApp","STR_CACHE_TITLE") + controler.autoTr
            }
            
            Rectangle {
                id: rectCacheButton
                width: parent.width
                color: "white"
                height: Constants.HEIGHT_BOTTOM_COMPONENT + Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateCache.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Button {
                    id: buttonFlushCache
                    text: qsTranslate("PageDataApp","STR_CLEAR_CACHE") + controler.autoTr
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }                
            }
        }
        Item{
            id: rectSCAPCache
            width: parent.width
            height: buttonRemoveSCAPCache.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.topMargin: 4*Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP
            anchors.top : rectCache.bottom

            Text {
                id: dateSCAPCache
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDataApp","STR_SCAP_CACHE_TITLE") + controler.autoTr
            }
        
            Rectangle {
                id: rectSCAPCacheButton
                width: parent.width
                color: "white"
                height: Constants.HEIGHT_BOTTOM_COMPONENT + Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateSCAPCache.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Button {
                    id: buttonRemoveSCAPCache
                    text: qsTranslate("PageDifiniionsSCAP","STR_SCAP_ATTRIBUTES_BUTTON_REMOVE") + controler.autoTr
                    width: Constants.WIDTH_BUTTON 
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }                
            }
        }
    }
}
