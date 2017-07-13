import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import Qt.labs.settings 1.0
import QtQuick.Window 2.2

/* Constants imports */
import "scripts/Constants.js" as Constants

Window {
    id: mainWindow
    visible: true
    flags: Qt.Window | Qt.CustomizeWindowHint | Qt.FramelessWindowHint

    width: Constants.SCREEN_MINIMUM_WIDTH
    height: Constants.SCREEN_MINIMUM_HEIGHT
    minimumWidth: Constants.SCREEN_MINIMUM_WIDTH
    minimumHeight: Constants.SCREEN_MINIMUM_HEIGHT

    title: "Autenticação.Gov"

    property string appVersion: "Versão 0.0.0 - 0000"

    FontLoader { id: karma; source: "qrc:/fonts/karma/Karma-Medium.ttf" }
    FontLoader { id: lato; source: "qrc:/fonts/lato/Lato-Regular.ttf" }

    onWidthChanged: {
        console.log("Resizing app width: " + width + "height" + height)
        mainFormID.propertyMainMenuView.width = getMainMenuWidth(width)
        mainFormID.propertySubMenuView.width = getSubMenuWidth(width)
        mainFormID.propertyContentPagesView.width = getContentPagesMenuWidth(width)
    }

    MainForm {
        id: mainFormID

        //************************************************************************/
        //**                  states
        //************************************************************************/
        states:[
            State{
                name: "STATE_FIRST_RUN"
            },
            State{
                name: "STATE_HOME"
            },
            State{
                name: "STATE_EXPAND"
            },
            State{
                name: "STATE_NORMAL"
            }
        ]
        transitions: [
            Transition {
                from: "STATE_FIRST_RUN"
                to: "STATE_NORMAL"
                NumberAnimation
                {
                    id: animationShowSubMenuFirstRun
                    target: mainFormID.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidthFirstRun
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidthFirstRun
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_HOME"
                to: "STATE_NORMAL"
                NumberAnimation
                {
                    id: animationShowSubMenu
                    target: mainFormID.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationShowContent
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidth
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidth
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationIncreaseContentPagesWidthExpand
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_NORMAL"
                to: "STATE_HOME"
                NumberAnimation
                {
                    id: animationHideSubMenu
                    target: mainFormID.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationHideContent
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationIncreaseMainMenuWidth
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidth
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_FIRST_RUN"
                to: "STATE_HOME"
                NumberAnimation
                {
                    id: animationHideSubMenu2
                    target: mainFormID.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationHideContent2
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationIncreaseMainMenuWidth2
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidth2
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationReduceContentPagesWidthExpand2
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_NORMAL"
                to: "STATE_EXPAND"
                NumberAnimation
                {
                    id: animationHideSubMenuExpand
                    target: mainFormID.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationExpandContentPagesWidthExpand
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width
                        * (Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE + Constants.SUB_MENU_VIEW_RELATIVE_SIZE)
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidthExpand
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_EXPAND"
                to: "STATE_NORMAL"
                NumberAnimation
                {
                    id: animationShowSubMenuExpand
                    target: mainFormID.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationDecreaseContentPagesWidthExpand
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidthExpand
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            }
        ]
        Component.onCompleted: {
            mainFormID.state = "STATE_FIRST_RUN"
            //mainFormID.state = "STATE_HOME"

            //Open Advanced Signature on startup
            if (gapi.getShortcutFlag() > 0) {
                //mainFormID.propertyMainMenuView.width = mainWindow.width
                mainFormID.propertyMainMenuListView.currentIndex = 1
                mainFormID.propertySubMenuListView.currentIndex = 1

                mainFormID.state = "STATE_EXPAND"
                // Clean the content page
                mainFormID.propertyPageLoader.source = "contentPages/services/PageServicesSignAdvanced.qml"
                //TODO: we shouldn't need this to make sure the contentPage gets the expanded space
                mainWindow.setWidth(Constants.SCREEN_MINIMUM_WIDTH + 1)
                return;
            }

            if ( mainFormID.state === "STATE_FIRST_RUN"){
                console.log("Runing First time" + mainWindow.width)
                mainFormID.propertyMainMenuView.width = mainWindow.width * 2 * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
                mainFormID.propertySubMenuView.width = 0
                mainFormID.propertyContentPagesView.width = mainWindow.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                mainFormID.propertyPageLoader.source = "contentPages/home/PageHome.qml"
            }else{
                mainFormID.propertyMainMenuView.width = mainWindow.width
            }

            // Do not select any option
            mainFormID.propertyMainMenuListView.currentIndex = -1
            mainFormID.propertyMainMenuBottomListView.currentIndex = -1
            console.log("MainForm Completed")
        }
        propertyImageLogo {
            onClicked: {
                mainFormID.state = "STATE_HOME"
                propertyMainMenuListView.currentIndex = -1
                propertyMainMenuBottomListView.currentIndex = -1
            }
        }
    }
    Component {
        id: mainMenuDelegate
        Item {
            width: mainFormID.propertyMainMenuListView.width
            height: mainFormID.propertyMainMenuListView.height
                    / mainFormID.propertyMainMenuListView.count
            MouseArea {
                id: mouseAreaMainMenu
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                    mainFormID.propertyMainMenuListView.currentIndex = index

                    // Clear list model and then load a new sub menu
                    mainFormID.propertySubMenuListView.model.clear()
                    for(var i = 0; i < mainFormID.propertyMainMenuListView.model.get(index).subdata.count; ++i) {
                        console.log("Sub Menu indice " + i + " - "
                                    + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).name);
                        mainFormID.propertySubMenuListView.model
                        .append({
                                    "subName": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).name,
                                    "expand": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).expand,
                                    "url": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).url
                                })
                    }
                    mainFormID.state = "STATE_NORMAL"

                    console.log("Expand Sub Menu = " +  mainFormID.propertyMainMenuListView.model.get(index).expand)
                    if( mainFormID.propertyMainMenuListView.model.get(index).expand === true){
                        // Clean the content page
                        mainFormID.propertyPageLoader.source = ""
                        // Do not select any option in sub menu
                        mainFormID.propertySubMenuListView.currentIndex = -1
                    }else{
                        // Open the content page of the first item of the new sub menu
                        mainFormID.propertyPageLoader.source =
                                mainFormID.propertyMainMenuListView.model.get(index).subdata.get(0).url
                        mainFormID.propertySubMenuListView.currentIndex = 0
                    }
                    console.log("Main Menu index = " + index);
                }
            }
            Text {
                text: name
                color:  mainFormID.propertyMainMenuListView.currentIndex === index ?
                            Constants.COLOR_TEXT_MAIN_MENU_SELECTED :
                            Constants.COLOR_TEXT_MAIN_MENU_DEFAULT
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.capitalization: Font.AllUppercase
                font.weight: mouseAreaMainMenu.containsMouse ?
                                 Font.Bold :
                                 Font.Normal
                font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            }
            Image {
                id: imageArrowMainMenu
                width: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                height: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                fillMode: Image.PreserveAspectFit
                x: parent.width * Constants.IMAGE_ARROW_MAIN_MENU_RELATIVE
                anchors.verticalCenter: parent.verticalCenter
                source: getMainMenuArrowSource(index, mouseAreaMainMenu.containsMouse)
                visible: mainFormID.state === "STATE_HOME" ?
                             false:
                             true
            }
            Rectangle {
                id: mainMenuViewHorizontalLine
                width: Constants.MAIN_MENU_LINE_H_SIZE
                height: Constants.MAIN_MENU_LINE_V_SIZE
                color: Constants.COLOR_MAIN_DARK_GRAY
                visible: mainFormID.propertyMainMenuListView.count - 1 === index ?
                             false :
                             true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.bottom
            }
        }
    }
    Component {
        id: mainMenuBottomDelegate
        Item {
            width: mainFormID.propertyMainMenuBottomListView.width / 2
            height: mainFormID.propertyMainMenuBottomListView.height
            MouseArea {
                id: mouseAreaMainMenuBottom
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    // Do not select any option
                    mainFormID.propertyMainMenuListView.currentIndex = -1
                    mainFormID.propertyMainMenuBottomListView.currentIndex = index
                    // Clear list model and then load a new sub menu
                    mainFormID.propertySubMenuListView.model.clear()
                    for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.count; ++i) {
                        console.log("Sub Menu indice " + i + " - "
                                    + mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(i).subName);
                        mainFormID.propertySubMenuListView.model
                        .append({
                                    "subName": mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(i)
                                    .subName,
                                    "expand": mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(i)
                                    .expand,
                                    "url": mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(i)
                                    .url
                                })
                    }

                    mainFormID.state = "STATE_NORMAL"

                    console.log("Expand Bottom Menu = " +  mainFormID.propertyMainMenuBottomListView.model.get(index).expand)
                    if( mainFormID.propertyMainMenuBottomListView.model.get(index).expand === true){
                        // Clean the content page
                        mainFormID.propertyPageLoader.source = ""
                        // Do not select any option in sub menu
                        mainFormID.propertySubMenuListView.currentIndex = -1
                    }else{
                        // Open the content page of the first item of the new sub menu
                        mainFormID.propertyPageLoader.source =
                                mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(0).url
                        mainFormID.propertySubMenuListView.currentIndex = 0
                    }
                    console.log("Main Menu Bottom index = " + index);
                }
            }
            Image {
                id: imageMainMenuBottom
                width: Constants.SIZE_IMAGE_BOTTOM_MENU
                height: Constants.SIZE_IMAGE_BOTTOM_MENU
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                source: getBottomMenuImgSource(index,mouseAreaMainMenuBottom.containsMouse)
            }
        }
    }
    Component {
        id: subMenuDelegate
        Item {
            width: mainFormID.propertySubMenuListView.width
            height: mainFormID.propertyMainView.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
            MouseArea {
                id: mouseAreaSubMenu
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    mainFormID.propertySubMenuListView.currentIndex = index
                    //console.log("Sub Menu index = " + index);
                    //console.log("Sub Menu Expand Sub Menu" + mainFormID.propertySubMenuListView.model.get(0).expand)

                    if(mainFormID.propertySubMenuListView.model.get(0).expand === true){
                        // Clean the content page
                        mainFormID.propertyPageLoader.source = url
                        mainFormID.state = "STATE_EXPAND"
                    }else{
                        // Open the content page of the first item of the new sub menu
                        mainFormID.propertyPageLoader.source = url
                    }
                }
            }
            Text {
                text: subName
                color: getSubNameColor(index, mouseAreaSubMenu.containsMouse)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.weight: mouseAreaSubMenu.containsMouse ?
                                 Font.Bold :
                                 Font.Normal
                font.pixelSize: Constants.SIZE_TEXT_SUB_MENU
                wrapMode: Text.Wrap
                width: parent.width - 2 * imageArrowSubMenu.width
                       - 2 * Constants.IMAGE_ARROW_SUB_MENU_MARGIN
                horizontalAlignment: Text.AlignHCenter
            }
            Image {
                id: imageArrowSubMenu
                width: Constants.SIZE_TEXT_SUB_MENU
                height: Constants.SIZE_TEXT_SUB_MENU
                fillMode: Image.PreserveAspectFit
                anchors.right: parent.right
                anchors.rightMargin: Constants.IMAGE_ARROW_SUB_MENU_MARGIN
                anchors.verticalCenter: parent.verticalCenter
                source: getSubMenuArrowSource(index, mouseAreaSubMenu.containsMouse)
            }
            Rectangle {
                id: subMenuViewHorizontalLine
                width: Constants.MAIN_MENU_LINE_H_SIZE
                height: Constants.MAIN_MENU_LINE_V_SIZE
                color: Constants.COLOR_MAIN_DARK_GRAY
                visible: Boolean(getIsVisibleSubMenuViewHorizontalLine(index))
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.bottom
            }
        }
    }

    Component.onCompleted: {
        console.log("Window mainWindow Completed")

    }

    function getMainMenuWidth(parentWidth){
        var handColor

        switch(mainFormID.state) {
        case "STATE_FIRST_RUN":
            handColor = parentWidth * 2 * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            break;
        case "STATE_HOME":
            handColor = parentWidth
            break;
        case "STATE_EXPAND":
            handColor = parentWidth * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            break;
        default: //STATE_NORMAL
            handColor = parentWidth * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
        }
        return handColor
    }
    function getSubMenuWidth(parentWidth){
        var handColor

        switch(mainFormID.state) {
        case "STATE_FIRST_RUN":
            handColor = 0
            break;
        case "STATE_HOME":
            handColor = 0
            break;
        case "STATE_EXPAND":
            handColor = 0
            break;
        default: //STATE_NORMAL
            handColor = parentWidth * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
        }
        return handColor
    }
    function getContentPagesMenuWidth(parentWidth){
        var handColor

        switch(mainFormID.state) {
        case "STATE_FIRST_RUN":
            handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
            break;
        case "STATE_HOME":
            handColor = 0
            break;
        case "STATE_EXPAND":
            handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE +
                    parentWidth * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
            break;
        default: //STATE_NORMAL
            handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
        }
        return handColor
    }

    function getIsVisibleSubMenuViewHorizontalLine(index)
    {
        var handVisible
        if(mainFormID.propertySubMenuListView.count -1 === index ||
                mainFormID.propertySubMenuListView.currentIndex === index ||
                mainFormID.propertySubMenuListView.currentIndex -1 === index)
        {
            handVisible =  false
        }else{
            handVisible =  true
        }
        return handVisible
    }

    function getSubNameColor(index, containsMouse)
    {
        var handColor
        if(mainFormID.propertySubMenuListView.currentIndex === index)
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

    function getSubMenuArrowSource(index, containsMouse)
    {
        var handSource
        if(mainFormID.propertySubMenuListView.currentIndex === index)
        {
            handSource =  "images/arrow-right_white_AMA.png"
        }else{
            if(containsMouse === true)
            {
                handSource = "images/arrow-right_hover.png"
            }else{
                handSource = ""
            }
        }
        return handSource
    }

    function getMainMenuArrowSource(index, containsMouse)
    {
        var handSource
        if(mainFormID.propertyMainMenuListView.currentIndex === index)
        {
            handSource =  "images/arrow-right_white_AMA.png"
        }else{
            if(containsMouse === true)
            {
                handSource = "images/arrow-right_hover.png"
            }else{
                handSource = ""
            }
        }
        return handSource
    }

    function getBottomMenuImgSource(index, containsMouse)
    {
        var handSource
        if(mainFormID.propertyMainMenuBottomListView.currentIndex === index)
        {
            handSource =  mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrlSel
        }else{
            if(containsMouse === true)
            {
                handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrlSel
            }else{
                handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrl
            }
        }
        return handSource
    }
}

