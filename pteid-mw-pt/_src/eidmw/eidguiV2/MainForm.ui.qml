import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3

/* Constants imports */
import "scripts/Constants.js" as Constants

Item {
    width: Constants.SCREEN_MINIMUM_WIDTH
    height: Constants.SCREEN_MINIMUM_HEIGHT
    Layout.minimumWidth: Constants.SCREEN_MINIMUM_WIDTH
    Layout.minimumHeight: Constants.SCREEN_MINIMUM_HEIGHT

    anchors.fill: parent

    property alias propertySplitView: splitView

    property alias propertyMainMenuListView: mainMenuListView

    property alias propertySubMenuListView: subMenuListView

    property alias propertyPageLoader: pageLoaderID

    /* SplitView */
    SplitView {

        id: splitView
        anchors.fill: parent

        /* Main Menu View */
        Rectangle {
            id: mainMenuView
            width: parent.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            height: parent.height
            color: Constants.COLOR_BACKGROUND_MAIN_MENU
            border.width: 0

            Image {
                id: imageLogo
                width: parent.width * Constants.IMAGE_LOGO_RELATIVE_H_SIZE
                y: parent.height * Constants.IMAGE_LOGO_RELATIVE_V_POS
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                source: "images/logo-ag.svg"
            }
            ListView {
                id: mainMenuListView
                width: parent.width
                height: parent.height * Constants.MAIN_MENU_RELATIVE_V_SIZE
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                model: MainMenuModel {}
                delegate: mainMenuDelegate
                focus: true
            }
        }

        /* Sub Menu View */
        Rectangle {
            id: subMenuView
            width: parent.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
            height: parent.height
            border.width: 0

            Rectangle {
                width: parent.width
                height: parent.height
                border.width: 0
                ListView {
                    id: subMenuListView
                    width: parent.width * Constants.SUB_MENU_RELATIVE_H_SIZE
                    height: parent.height * Constants.SUB_MENU_RELATIVE_V_SIZE
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    model: SubMenuModel {}
                    delegate: subMenuDelegate
                    highlight: Rectangle {
                        width: parent.width
                        color: Constants.COLOR_BACKGROUND_SUB_MENU
                        radius: 0
                    }
                    focus: true
                }
            }
        }

        /* Main Menu View */
        Rectangle {
            id: contentPagesView
            width: parent.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE

            Rectangle {
                width: parent.width * Constants.PAGE_RELATIVE_H_SIZE
                height: parent.height * Constants.PAGE_RELATIVE_V_SIZE
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                PageLoader{
                    id: pageLoaderID
                }
            }
        }

    }
}
