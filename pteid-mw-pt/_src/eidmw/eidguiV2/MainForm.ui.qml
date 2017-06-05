import QtQuick 2.6
import QtQuick.Layouts 1.3

/* Constants imports */
import "scripts/Constants.js" as Constants

Item {
    width: Constants.SCREEN_MINIMUM_WIDTH
    height: Constants.SCREEN_MINIMUM_HEIGHT
    Layout.minimumWidth: Constants.SCREEN_MINIMUM_WIDTH
    Layout.minimumHeight: Constants.SCREEN_MINIMUM_HEIGHT

    anchors.fill: parent

    property alias propertyMainView: mainView

    property alias propertyMainMenuView: mainMenuView
    property alias propertyMainMenuListView: mainMenuListView

    property alias propertyMainMenuBottomListView: mainMenuBottomListView
    property alias propertySubMenuView: subMenuView
    property alias propertySubMenuListView: subMenuListView

    property alias propertyPageLoader: pageLoaderID

    property alias propertyImageLogo : mouseAreaImageLogo

    /* Main View */
    Item {

        id: mainView
        anchors.fill: parent

        /* Main Menu View */
        Rectangle {
            id: mainMenuView
            x: 0
            y: 0
            width: parent.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            height: parent.height
            color: Constants.COLOR_BACKGROUND_MAIN_MENU
            border.width: 0
            z: 1
            Image {
                id: imageLogo
                width: parent.width * Constants.IMAGE_LOGO_RELATIVE_H_SIZE
                y: parent.height * Constants.IMAGE_LOGO_RELATIVE_V_POS
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                source: "images/logo_autenticacao_gov.png"
                MouseArea {
                    id: mouseAreaImageLogo
                    anchors.fill: parent
                }
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
            ListView {
                id: mainMenuBottomListView
                width: parent.width / 2
                height: parent.height * Constants.MAIN_MENU_BOTTOM_RELATIVE_V_SIZE
                y: parent.height * Constants.MAIN_MENU_BOTTOM_RELATIVE_V_POS
                orientation: ListView.Horizontal
                anchors.horizontalCenter: parent.horizontalCenter
                model: MainMenuBottomModel {}
                delegate: mainMenuBottomDelegate
                focus: true
            }

        }

        /* Sub Menu View */
        Rectangle {
            id: subMenuView
            width: parent.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
            height: parent.height
            border.width: 0
            anchors.left: mainMenuView.right
            z: 0
            Rectangle {
                width: parent.width * Constants.SUB_MENU_RELATIVE_H_SIZE
                height: subMenuListView.count *
                        mainView.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                border.width: 0
                ListView {
                    id: subMenuListView
                    width: parent.width
                    height: parent.height
                    model: SubMenuModel {}
                    delegate: subMenuDelegate
                    highlight: Rectangle {
                        width: parent.width
                        color: Constants.COLOR_BACKGROUND_SUB_MENU
                        radius: 0
                    }
                    focus: true
                }

                Rectangle {
                    id: subMenuViewVerticalLine
                    x: subMenuListView.width
                    width: subMenuListView.width * Constants.SUB_MENU_RELATIVE_LINE_H_SIZE
                    height: subMenuListView.height
                    anchors.verticalCenter: parent.verticalCenter
                    color: Constants.COLOR_LINE_SUB_MENU
                    anchors.verticalCenterOffset: 0
                }
            }
        }

        /* Content Pages View */
        Rectangle {
            id: contentPagesView
            width: parent.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
            height: parent.height
            anchors.left: subMenuView.right
            z: 1
            Rectangle {
                width: parent.width * Constants.PAGE_RELATIVE_H_SIZE
                height: parent.height * Constants.PAGE_RELATIVE_V_SIZE
                anchors.verticalCenter: parent.verticalCenter
                PageLoader{
                    id: pageLoaderID
                }
            }
        }

    }
}
