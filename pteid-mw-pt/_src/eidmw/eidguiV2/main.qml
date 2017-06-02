import QtQuick 2.6
import QtQuick.Window 2.2

/* Constants imports */
import "scripts/Constants.js" as Constants

Window {
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
            main.state = "STATE_HOME"
            console.log("MainForm Completed")
        }
        //************************************************************************/
        //**                  states
        //************************************************************************/
        states:[
            State{
                name: "STATE_HOME"
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
                name: "STATE_NORMAL"
            }
        ]
        transitions: [
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
                    duration: Constants.ANIMATION_STATE_OPACITY_TO_NORMAL
                    onStarted: console.log("animationShowSubMenu")
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidth
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_STATE_TO_NORMAL
                    onStarted: console.log("animationReduceMainMenuWidth")
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidth
                    target: main.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: main.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_STATE_TO_NORMAL
                    onStarted: console.log("animationShowSubMenuWidth")
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
                    duration: Constants.ANIMATION_STATE_OPACITY_TO_HOME
                    onStarted: console.log("animationHideSubMenu")
                }
                NumberAnimation
                {
                    id: animationIncreaseMainMenuWidth
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 2 * main.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: Constants.ANIMATION_STATE_TO_HOME
                    onStarted: console.log("animationIncreaseMainMenuWidth")
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidth
                    target: main.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: Constants.ANIMATION_STATE_TO_HOME
                    onStarted: console.log("animationHideSubMenuWidth")
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
                    main.propertyMainMenuListView.currentIndex = index


                    // Clear list model and then load a new sub menu
                    main.propertySubMenuListView.model.clear()
                    for(var i = 0; i < main.propertyMainMenuListView.model.get(index).subdata.count; ++i) {
                        console.log("Sub Menu indice " + i + " - "
                                    + main.propertyMainMenuListView.model.get(index).subdata.get(i).name);
                        main.propertySubMenuListView.model.append({
                             "subName": main.propertyMainMenuListView.model.get(index).subdata.get(i).name,
                             "url": main.propertyMainMenuListView.model.get(index).subdata.get(i).url })
                    }

                    // Open the content page of the first item of the new sub menu
                    main.state = "STATE_NORMAL"
                    main.propertyPageLoader.source = main.propertyMainMenuListView.model.get(index).subdata.get(0).url
                    onClicked: console.log("Main Menu index = " + index);
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
                    onClicked: console.log("Sub Menu index = " + index);
                    main.propertyPageLoader.source = url
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
}


