import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

PageSecurityPinCodesForm {
    Component {
        id: pinCodesMenuDelegate
        Item {
            width: propertyPinCodesListView.width
            height: propertyPinCodesViewLeft.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
            MouseArea {
                id: mouseAreaPinCodes
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    propertyPinCodesListView.currentIndex = index
                    console.log("Pin Codes index = " + index);
                }
            }
            Text {
                text: name
                color: getPinCodesColor(index, mouseAreaPinCodes.containsMouse)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.weight: mouseAreaPinCodes.containsMouse ?
                                 Font.Bold :
                                 Font.Normal
                font.pixelSize: Constants.SIZE_TEXT_SUB_MENU
                wrapMode: Text.Wrap
                width: parent.width - imageArrowPinCodes.width
                horizontalAlignment: Text.AlignHCenter

            }
            Image {
                id: imageArrowPinCodes
                width: Constants.SIZE_IMAGE_ARROW_SUB_MENU
                height: Constants.SIZE_IMAGE_ARROW_SUB_MENU
                fillMode: Image.PreserveAspectFit
                x: parent.width * Constants.IMAGE_ARROW_SUB_MENU_RELATIVE
                anchors.verticalCenter: parent.verticalCenter
                source: getPinCodesArrowSource(index, mouseAreaPinCodes.containsMouse)
            }
        }
    }

    function getPinCodesColor(index, containsMouse)
    {
        var handColor
        if(propertyPinCodesListView.currentIndex === index)
        {
            handColor =  Constants.COLOR_TEXT_SUB_MENU_SELECTED
        }else{
            if(containsMouse === true)
            {
                handColor = Constants.COLOR_TEXT_SUB_MENU_MOUSE_OVER
            }else{
                handColor = Constants.COLOR_TEXT_SUB_MENU_DEFAULT
            }
        }
        return handColor
    }

    function getPinCodesArrowSource(index, containsMouse)
    {
        var handSource
        if(propertyPinCodesListView.currentIndex === index)
        {
            handSource =  "../../images/arrow-right_white_AMA.png"
        }else{
            if(containsMouse === true)
            {
                handSource = "../../images/arrow-right_hover.png"
            }else{
                handSource = ""
            }
        }
        return handSource
    }
}
