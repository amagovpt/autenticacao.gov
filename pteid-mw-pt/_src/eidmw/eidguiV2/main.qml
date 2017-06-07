import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import Qt.labs.settings 1.0

/* Constants imports */
import "scripts/Constants.js" as Constants

ApplicationWindow {
    id: mainWindow
    visible: true

    width: Constants.SCREEN_MINIMUM_WIDTH
    height: Constants.SCREEN_MINIMUM_HEIGHT
    minimumWidth: Constants.SCREEN_MINIMUM_WIDTH
    minimumHeight: Constants.SCREEN_MINIMUM_HEIGHT

    title: "Cartão de Cidadão"

    FontLoader { id: karma; source: "qrc:/fonts/karma/Karma-Medium.ttf" }
    FontLoader { id: lato; source: "qrc:/fonts/lato/Lato-Regular.ttf" }

    property alias propertyMain: main

    MainForm {
        id: main
        anchors.fill: parent

        Component.onCompleted: {
            main.state = "STATE_FIRST_RUN"
            // Do not select any option
            main.propertyMainMenuListView.currentIndex = -1
            main.propertyMainMenuBottomListView.currentIndex = -1
            console.log("MainForm Completed")
        }
        propertyImageLogo {
            onClicked: {
                propertyMain.state = "STATE_HOME"
                propertyMainMenuListView.currentIndex = -1
                propertyMainMenuBottomListView.currentIndex = -1
            }
        }
        //************************************************************************/
        //**                  states
        //************************************************************************/
        states:[
            State{
                name: "STATE_FIRST_RUN"
                PropertyChanges {
                    target: main.propertyMainMenuView
                    width: 2 * parent.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
                }
                PropertyChanges {
                    target: main.propertySubMenuView
                    width:  0
                }
                PropertyChanges {
                    target: main.propertyPageLoader
                    source:  "contentPages/home/PageHome.qml"
                }
            },
            State{
                name: "STATE_HOME"
                PropertyChanges {
                    target: main.propertyMainMenuView
                    width: parent.width
                }
                PropertyChanges {
                    target: main.propertySubMenuView
                    width:  0
                }
            },
            State{
                name: "STATE_EXPAND"
                PropertyChanges {
                    target: main.propertyMainMenuView
                    width: parent.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
                }
                PropertyChanges {
                    target: main.propertySubMenuView
                    width:  0
                }
                PropertyChanges {
                    target: main.propertyContentPagesView
                    width:  parent.width
                            * (Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE + Constants.SUB_MENU_VIEW_RELATIVE_SIZE)
                }
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
                    target: main.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidthFirstRun
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidthFirstRun
                    target: main.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_HOME"
                to: "STATE_NORMAL"
                NumberAnimation
                {
                    id: animationShowSubMenu
                    target: main.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationShowContent
                    target: main.propertyPageLoader
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidth
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidth
                    target: main.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_NORMAL"
                to: "STATE_HOME"
                NumberAnimation
                {
                    id: animationHideSubMenu
                    target: main.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationHideContent
                    target: main.propertyPageLoader
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationIncreaseMainMenuWidth
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidth
                    target: main.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            },
            Transition {
                from: "STATE_NORMAL"
                to: "STATE_EXPAND"
                NumberAnimation
                {
                    id: animationHideSubMenuExpand
                    target: main.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationIncreaseContentPagesWidthExpand
                    target: main.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width
                        * (Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE + Constants.SUB_MENU_VIEW_RELATIVE_SIZE)
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidthExpand
                    target: main.propertySubMenuView
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
                    target: main.propertySubMenuView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: Constants.ANIMATION_CHANGE_OPACITY
                }
                NumberAnimation
                {
                    id: animationDecreaseContentPagesWidthExpand
                    target: main.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidthExpand
                    target: main.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_MOVE_VIEW
                }
            }
        ]
    }
    Component {
        id: mainMenuDelegate
        Item {
            width: main.propertyMainMenuListView.width
            height: main.propertyMainMenuListView.height
                    / main.propertyMainMenuListView.count
            MouseArea {
                id: mouseAreaMainMenu
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    main.propertyMainMenuBottomListView.currentIndex = -1
                    main.propertyMainMenuListView.currentIndex = index

                    // Clear list model and then load a new sub menu
                    main.propertySubMenuListView.model.clear()
                    for(var i = 0; i < main.propertyMainMenuListView.model.get(index).subdata.count; ++i) {
                        console.log("Sub Menu indice " + i + " - "
                                    + main.propertyMainMenuListView.model.get(index).subdata.get(i).name);
                        main.propertySubMenuListView.model
                        .append({
                                    "subName": main.propertyMainMenuListView.model.get(index).subdata.get(i).name,
                                    "expand": main.propertyMainMenuListView.model.get(index).subdata.get(i).expand,
                                    "url": main.propertyMainMenuListView.model.get(index).subdata.get(i).url
                                })
                    }
                    main.state = "STATE_NORMAL"

                    console.log("Expand Sub Menu = " +  main.propertyMainMenuListView.model.get(index).expand)
                    if( main.propertyMainMenuListView.model.get(index).expand === true){
                        // Clean the content page
                        main.propertyPageLoader.source = ""
                        // Do not select any option in sub menu
                        main.propertySubMenuListView.currentIndex = -1
                    }else{
                        // Open the content page of the first item of the new sub menu
                        main.propertyPageLoader.source =
                                main.propertyMainMenuListView.model.get(index).subdata.get(0).url
                        main.propertySubMenuListView.currentIndex = 0
                    }
                    console.log("Main Menu index = " + index);
                }
            }
            Text {
                text: name
                color:  main.propertyMainMenuListView.currentIndex === index ?
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
                width: Constants.SIZE_TEXT_MAIN_MENU
                height: Constants.SIZE_TEXT_MAIN_MENU
                fillMode: Image.PreserveAspectFit
                x: parent.width * Constants.IMAGE_ARROW_MAIN_MENU_RELATIVE
                anchors.verticalCenter: parent.verticalCenter
                source: getMainMenuArrowSource(index, mouseAreaMainMenu.containsMouse)
            }
            Rectangle {
                id: mainMenuViewHorizontalLine
                width: Constants.MAIN_MENU_LINE_H_SIZE
                height: Constants.MAIN_MENU_LINE_V_SIZE
                color: Constants.COLOR_MAIN_DARK_GRAY
                visible: main.propertyMainMenuListView.count - 1 === index ?
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
            width: main.propertyMainMenuBottomListView.width / 2
            height: main.propertyMainMenuBottomListView.height
            MouseArea {
                id: mouseAreaMainMenuBottom
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    // Do not select any option
                    main.propertyMainMenuListView.currentIndex = -1
                    main.propertyMainMenuBottomListView.currentIndex = index
                    // Clear list model and then load a new sub menu
                    main.propertySubMenuListView.model.clear()
                    for(var i = 0; i < main.propertyMainMenuBottomListView.model.get(index).subdata.count; ++i) {
                        console.log("Sub Menu indice " + i + " - "
                                    + main.propertyMainMenuBottomListView.model.get(index).subdata.get(i).subName);
                        main.propertySubMenuListView.model
                        .append({
                                    "subName": main.propertyMainMenuBottomListView.model.get(index).subdata.get(i)
                                    .subName,
                                    "expand": main.propertyMainMenuBottomListView.model.get(index).subdata.get(i)
                                    .expand,
                                    "url": main.propertyMainMenuBottomListView.model.get(index).subdata.get(i)
                                    .url
                                })
                    }

                    main.state = "STATE_NORMAL"

                    console.log("Expand Bottom Menu = " +  main.propertyMainMenuBottomListView.model.get(index).expand)
                    if( main.propertyMainMenuBottomListView.model.get(index).expand === true){
                        // Clean the content page
                        main.propertyPageLoader.source = ""
                        // Do not select any option in sub menu
                        main.propertySubMenuListView.currentIndex = -1
                    }else{
                        // Open the content page of the first item of the new sub menu
                        main.propertyPageLoader.source =
                                main.propertyMainMenuBottomListView.model.get(index).subdata.get(0).url
                        main.propertySubMenuListView.currentIndex = 0
                    }
                    console.log("Main Menu Bottom index = " + index);
                }
            }
            Image {
                id: imageMainMenuBottom
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                source:  main.propertyMainMenuBottomListView.currentIndex === index ?
                             imageUrl :
                             imageUrlSel
                scale: mouseAreaMainMenuBottom.containsMouse ?
                           1.5 :
                           1
            }
        }
    }
    Component {
        id: subMenuDelegate
        Item {
            width: main.propertySubMenuListView.width
            height: main.propertyMainView.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
            MouseArea {
                id: mouseAreaSubMenu
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    main.propertySubMenuListView.currentIndex = index
                    console.log("Sub Menu index = " + index);
                    console.log("Sub Menu Expand Sub Menu" + main.propertySubMenuListView.model.get(0).expand)

                    if(main.propertySubMenuListView.model.get(0).expand === true){
                        // Clean the content page
                        main.propertyPageLoader.source = url
                        main.state = "STATE_EXPAND"
                    }else{
                        // Open the content page of the first item of the new sub menu
                        main.propertyPageLoader.source = url
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
                width: parent.width
                horizontalAlignment: Text.AlignHCenter

            }
            Image {
                id: imageArrowSubMenu
                width: Constants.SIZE_TEXT_SUB_MENU
                height: Constants.SIZE_TEXT_SUB_MENU
                fillMode: Image.PreserveAspectFit
                x: parent.width * Constants.IMAGE_ARROW_SUB_MENU_RELATIVE
                anchors.verticalCenter: parent.verticalCenter
                source: getSubMenuArrowSource(index, mouseAreaSubMenu.containsMouse)
            }
        }
    }

    Component.onCompleted: {
        console.log("Window mainWindow Completed")
    }

    function getSubNameColor(index, containsMouse)
    {
        var handColor
        if(main.propertySubMenuListView.currentIndex === index)
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
        if(main.propertySubMenuListView.currentIndex === index)
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
        if(main.propertyMainMenuListView.currentIndex === index)
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
}

