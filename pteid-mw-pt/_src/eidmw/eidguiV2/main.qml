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

    MainForm {
        id: main
        anchors.fill: parent

        Component.onCompleted: {
            console.log("MainForm Completed")
        }
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
                font.pointSize: Constants.SIZE_TEXT_MAIN_MENU
            }
        }
    }
    Component {
        id: subMenuDelegate
        Item {
            width: main.propertySplitView.width
                   * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
                   * Constants.SUB_MENU_RELATIVE_H_SIZE
            height: main.propertySubMenuListView.height
                    / main.propertySubMenuListView.count
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
                text: name
                color:  main.propertySubMenuListView.currentIndex === index ?
                            Constants.COLOR_TEXT_SUB_MENU_SELECTED :
                            Constants.COLOR_TEXT_SUB_MENU_DEFAULT
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.weight: mouseAreaSubMenu.containsMouse ?
                                 Font.Bold :
                                 Font.Normal
                font.pointSize: Constants.SIZE_TEXT_SUB_MENU
            }
        }
    }

    Component.onCompleted: {
        console.log("Window mainWindow Completed")
    }
}
