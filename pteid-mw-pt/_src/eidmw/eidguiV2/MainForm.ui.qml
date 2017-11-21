import QtQuick 2.6

/* Constants imports */
import "scripts/Constants.js" as Constants
import "components" as Components

Rectangle {
    anchors.fill: parent

    property alias propertyMainView: mainView

    property alias propertyMainMenuView: mainMenuView
    property alias propertyMainMenuListView: mainMenuListView

    property alias propertyMainMenuBottomListView: mainMenuBottomListView
    property alias propertySubMenuView: subMenuView
    property alias propertySubMenuViewMenu: subMenuViewMenu
    property alias propertyMouseAreaSubMenuViewReduced : mouseAreaSubMenuViewReduced
    property alias propertySubMenuListView: subMenuListView

    property alias propertyContentPagesView: contentPagesView

    property alias propertyPageLoader: pageLoaderID

    property alias propertyImageLogo : mouseAreaImageLogo

    property bool propertShowAnimation: true

    border.width: Constants.APP_BORDER
    border.color : Constants.COLOR_MAIN_BLUE

    /* Title bar */
    Components.TitleBar {
        id: titleBar;
        width: parent.width;
        height: Constants.TITLE_BAR_SIZE;
        opacity: 1
    }

    /* Frame Window */
    Components.FrameWindow {
        id: leftFrameBar;
        z: 0
        width: Constants.FRAME_WINDOW_SIZE;
        height: mainView.height;
        anchors.left:  mainView.left
        propertyMouseRegion.cursorShape: Qt.SizeHorCursor
        propertySide: "LEFT"
    }
    /* Frame Window */
    Components.FrameWindow {
        id: rightFrameBar;
        z: 0
        width: Constants.FRAME_WINDOW_SIZE;
        height: mainView.height;
        anchors.right: mainView.right
        propertyMouseRegion.cursorShape: Qt.SizeHorCursor
        propertySide: "RIGHT"
    }
    /* Frame Window */
    Components.FrameWindow {
        id: bottomFrameBar;
        z: 0
        width: parent.width;
        height: Constants.FRAME_WINDOW_SIZE;
        anchors.bottom: mainView.bottom
        propertySide: "BOTTOM"
    }
    /* Main View */
    Item {

        id: mainView
        width: parent.width
        height: parent.height - Constants.TITLE_BAR_SIZE
        y: Constants.TITLE_BAR_SIZE

        /* Main Menu View */
        Rectangle {
            id: mainMenuView
            width: parent.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            height: parent.height
            color: Constants.COLOR_BACKGROUND_MAIN_MENU
            border.width: 0
            z: 1
            Image {
                id: imageLogo
                width: Constants.SIZE_IMAGE_LOGO_MAIN_MENU_WIDTH
                height: Constants.SIZE_IMAGE_LOGO_MAIN_MENU_HEIGHT
                y: parent.height * Constants.IMAGE_LOGO_RELATIVE_V_POS
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                source: "images/logo_autenticacao_gov100.png"
                MouseArea {
                    id: mouseAreaImageLogo
                    anchors.fill: parent
                }
            }
            ListView {
                id: mainMenuListView
                width: parent.width
                height: parent.height * Constants.MAIN_MENU_RELATIVE_V_SIZE
                boundsBehavior: Flickable.StopAtBounds
                highlightFollowsCurrentItem: true
                highlightResizeDuration: propertShowAnimation ? Constants.ANIMATION_LISTVIEW_RESIZE : 0
                highlightMoveDuration : propertShowAnimation ? Constants.ANIMATION_LISTVIEW_MOVE : 0
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                model: MainMenuModel {}
                delegate: mainMenuDelegate
                focus: true
            }
            ListView {
                id: mainMenuBottomListView
                width: Constants.BOTTOM_MENU_WIDTH_SIZE
                height: parent.height * Constants.MAIN_MENU_BOTTOM_RELATIVE_V_SIZE
                boundsBehavior: Flickable.StopAtBounds
                highlightFollowsCurrentItem: true
                highlightResizeDuration: propertShowAnimation ? Constants.ANIMATION_LISTVIEW_RESIZE : 0
                highlightMoveDuration : propertShowAnimation ? Constants.ANIMATION_LISTVIEW_MOVE : 0
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
            height: parent.height - Constants.APP_BORDER
            anchors.left: mainMenuView.right
            color: "white"
            z: 0
            Item {
                id: subMenuViewMenu
                width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE
                       - subMenuViewVerticalLine.width
                height: subMenuListView.count *
                        mainView.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                ListView {
                    id: subMenuListView
                    width: parent.width
                    height: parent.height
                    boundsBehavior: Flickable.StopAtBounds
                    highlightFollowsCurrentItem: true
                    highlightResizeDuration: propertShowAnimation ? Constants.ANIMATION_LISTVIEW_RESIZE : 0
                    highlightMoveDuration : propertShowAnimation ? Constants.ANIMATION_LISTVIEW_MOVE : 0
                    model: SubMenuModel {}
                    delegate: subMenuDelegate
                    highlight: Rectangle {
                        color: Constants.COLOR_BACKGROUND_SUB_MENU
                        radius: 0
                    }
                    focus: true
                }

                Rectangle {
                    id: subMenuViewVerticalLine
                    x: subMenuListView.width
                    width: subMenuView.width * Constants.SUB_MENU_RELATIVE_LINE_H_SIZE
                    height: subMenuListView.height
                    anchors.verticalCenter: parent.verticalCenter
                    color: Constants.COLOR_LINE_SUB_MENU
                    anchors.verticalCenterOffset: 0
                }
            }
            Item {
                id: subMenuViewReduced
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                opacity: 1 - subMenuViewMenu.opacity

                MouseArea {
                    id: mouseAreaSubMenuViewReduced
                    anchors.fill: parent
                     enabled: subMenuViewReduced.opacity == 1 ? true : false
                }
                Image {
                    id: imageReduced
                    width: Constants.SIZE_TEXT_SUB_MENU
                    height: Constants.SIZE_TEXT_SUB_MENU
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: parent.height / 2 - Constants.SIZE_TEXT_SUB_MENU / 2 + Constants.APP_BORDER
                    source: "images/arrow-right_white_AMA.png"
                }
            }
        }

        /* Content Pages View */
        Item {
            id: contentPagesView
            width: parent.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
            height: parent.height
            anchors.left: subMenuView.right
            z: 1
            Item {
                width: parent.width - Constants.SIZE_ROW_H_SPACE
                height: parent.height - 2 * Constants.SIZE_ROW_V_SPACE
                anchors.verticalCenter: parent.verticalCenter
                PageLoader{
                    id: pageLoaderID
                    propertyGeneralPopUp.visible: false
                }
            }
        }
    }
}
