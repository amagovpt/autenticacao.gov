import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Window 2.2

import "scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

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

    FontLoader { id: karma; source: "qrc:/fonts/karma/Karma-Medium.ttf" }
    FontLoader { id: lato; source: "qrc:/fonts/lato/Lato-Regular.ttf" }

    onMinimumWidthChanged: {
        console.log("Window Loaded")
        controler.initTranslation()
        gapi.initTranslation()
    }

    onWidthChanged: {
        console.log("Resizing app width: " + width + "height" + height)
        mainFormID.propertyMainMenuView.width = Functions.getMainMenuWidth(width)
        mainFormID.propertySubMenuView.width = Functions.getSubMenuWidth(width)
        mainFormID.propertyContentPagesView.width = Functions.getContentPagesMenuWidth(width)
    }

    Connections {
        target: controler
        onSignalRestoreWindows: {
            console.log("Signal onSignalRestoreWindows")
            mainWindow.raise();
            mainWindow.show();
            mainWindow.activateWindow();
        }
        onSignalLanguageChangedError: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    "Erro / Error"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    "Erro na leitura dos idiomas. Por favor, reinstale a aplicação \n
Load language error. Please reinstall the application"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
        }
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            console.log("Signal onSignalGenericError")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("Popup Card","STR_POPUP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("Popup Card","STR_GENERIC_ERROR_MSG") + "\n\n" +
                    qsTranslate("Popup Card","STR_ERROR_CODE") + error_code
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalImportCertificatesFail: {
            console.log("Signal onSignalImportCertificatesFail")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("Popup Card","STR_POPUP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("Popup Card","STR_CERTIFICATES_IMPORT_ERROR_MSG")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalRemoveCertificatesFail: {
            console.log("Signal onSignalRemoveCertificatesFail")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("Popup Card","STR_POPUP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("Popup Card","STR_CERTIFICATES_REMOVE_ERROR_MSG")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalLanguageChangedError: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    "Erro / Error"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    "Erro na leitura dos idiomas. Por favor, reinstale a aplicação \n
Load language error. Please reinstall the application"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
        }

        onSignalReaderContext: {
            //console.log("Reader List: " + gapi.getRetReaderList())
            //console.log("Reader List Count: " + gapi.getRetReaderList().length)

            for ( var i = 0; i < gapi.getRetReaderList().length; ++i ) {
              //  console.log("Reader List " + "i = " + i +" : "+ gapi.getRetReaderList()[i])
                comboBoxReader.model = gapi.getRetReaderList()
            }
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            readerContext.open()
        }
    }
    Dialog {
        id: readerContext
        width: 400
        height: 250
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: parent.width * 0.5 - readerContext.width * 0.5
        y: parent.height * 0.5 - readerContext.height * 0.5

        header: Label {
            id: labelReaderContext
            text: "Aviso"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            width: parent.width
            height: rectMessageTop.height + rectReaderCombo.height + rectSwitchRemember.height

            Item {
                id: rectMessageTop
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
                    text: "Múltiplos cartões detectados"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
            Rectangle {
                id: rectReaderCombo
                width: parent.width
                color: "white"
                height: 3 * Constants.SIZE_TEXT_FIELD
                anchors.top : rectMessageTop.bottom

                ComboBox {
                    id: comboBoxReader
                    width: parent.width
                    height: 3 * Constants.SIZE_TEXT_FIELD
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    visible: true
                }
            }
            Item {
                id: rectSwitchRemember
                width: parent.width
                height: 50
                anchors.top: rectReaderCombo.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: true
                Text {
                    id: textNote
                    text:  "Para alterar essa opção mais tarde, vá para o menu de configuração"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
        }

        standardButtons: DialogButtonBox.Ok
        onAccepted: {
            console.log("comboBoxReader onActivated index = " + comboBoxReader.currentIndex)
            gapi.setReaderByUser(comboBoxReader.currentIndex)

            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.source = mainFormID.propertyPageLoader.source

            // Force reload page loader
            var temp = mainFormID.propertyPageLoader.source
            mainFormID.propertyPageLoader.source = ""
            mainFormID.propertyPageLoader.source = temp
        }
        onRejected: {
            readerContext.open()
        }
    }

    MainForm {
        id: mainFormID
        property bool isAnimationFinished: mainFormID.propertyPageLoader.propertyAnimationExtendedFinished

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
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidthFirstRun
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidthFirstRun
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
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
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationShowContent
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationReduceMainMenuWidth
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidth
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationIncreaseContentPagesWidthExpand
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
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
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationHideContent
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationIncreaseMainMenuWidth
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidth
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
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
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationHideContent2
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationIncreaseMainMenuWidth2
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidth2
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationReduceContentPagesWidthExpand2
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
            },
            Transition {
                from: "STATE_EXPAND"
                to: "STATE_HOME"
                NumberAnimation
                {
                    target: mainFormID.propertySubMenuViewMenu
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                ColorAnimation
                {
                    id: animationColorSubMenuExpanded
                    target: mainFormID.propertySubMenuView
                    property: "color"
                    easing.type: Easing.Linear
                    to: "white";
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: 0
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                onRunningChanged: {
                    if (( mainFormID.state == "STATE_HOME") && (!running))
                    {
                        mainFormID.propertyPageLoader.propertyAnimationExtendedFinished = false
                    }
                }
            },
            Transition {
                from: "STATE_NORMAL"
                to: "STATE_EXPAND"
                NumberAnimation
                {
                    id: animationHideSubMenuExpand
                    target: mainFormID.propertySubMenuViewMenu
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 0;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                ColorAnimation
                {
                    id: animationColorSubMenuExpand
                    target: mainFormID.propertySubMenuView
                    property: "color"
                    easing.type: Easing.Linear
                    to: Constants.COLOR_MAIN_DARK_GRAY;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationExpandContentPagesWidthExpand
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width
                        * (Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE + Constants.SUB_MENU_VIEW_RELATIVE_SIZE
                           -Constants.SUB_MENU_EXPAND_VIEW_RELATIVE_SIZE)
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationHideSubMenuWidthExpand
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_EXPAND_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                onRunningChanged: {
                    if (( mainFormID.state == "STATE_EXPAND") && (!running))
                    {
                        mainFormID.propertyPageLoader.propertyAnimationExtendedFinished = true
                    }
                }
            },
            Transition {
                from: "STATE_EXPAND"
                to: "STATE_NORMAL"
                NumberAnimation
                {
                    id: animationShowSubMenuExpand
                    target: mainFormID.propertySubMenuViewMenu
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                ColorAnimation
                {
                    id: animationColorSubMenuExpandedNormal
                    target: mainFormID.propertySubMenuView
                    property: "color"
                    easing.type: Easing.Linear
                    to: "white";
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
                }
                NumberAnimation
                {
                    id: animationDecreaseContentPagesWidthExpand
                    target: mainFormID.propertyContentPagesView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    id: animationShowSubMenuWidthExpand
                    target: mainFormID.propertySubMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                onRunningChanged: {
                    if (( mainFormID.state == "STATE_NORMAL") && (!running))
                    {
                        mainFormID.propertyPageLoader.propertyAnimationExtendedFinished = false
                    }
                }
            }
        ]
        Component.onCompleted: {
            if(controler.getNotShowHelpStartUp()){
                mainFormID.state = "STATE_HOME"
            }else{
                mainFormID.state = "STATE_FIRST_RUN"
            }

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
            console.log("MainForm Completed testmode: "+controler.getTestMode())
            if(controler.getTestMode())
                mainWindow.title = mainWindow.title + "   [ " + qsTr("STR_RUN_MODE") + " ]"
        }
        propertyImageLogo {
            onClicked: {
                mainFormID.state = "STATE_HOME"
                propertyMainMenuListView.currentIndex = -1
                propertyMainMenuBottomListView.currentIndex = -1
            }
        }
        propertyMouseAreaSubMenuViewReduced {
            onClicked: {
                mainFormID.state = "STATE_NORMAL"
                mainFormID.propertyPageLoader.source = "contentPages/services/PageServicesSignHelp.qml"
                // Do not select any option in sub menu
                mainFormID.propertySubMenuListView.currentIndex = -1
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
                        mainFormID.propertyPageLoader.source = "contentPages/services/PageServicesSignHelp.qml"
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
                text: qsTranslate("MainMenuModel", name) + controler.autoTr
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
                source: Functions.getMainMenuArrowSource(index, mouseAreaMainMenu.containsMouse)
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
            Component.onDestruction: {
                imageArrowMainMenu.source = ""
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
                                    "subName": qsTranslate("MainMenuBottomModel",
                                                           mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(i).subName),
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
                source: Functions.getBottomMenuImgSource(index,mouseAreaMainMenuBottom.containsMouse)

            }
            Component.onDestruction: {
                imageMainMenuBottom.source = ""
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
                text: qsTranslate("MainMenuModel", subName)
                color: Functions.getSubNameColor(index, mouseAreaSubMenu.containsMouse)
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
                source: Functions.getSubMenuArrowSource(index, mouseAreaSubMenu.containsMouse)
            }
            Rectangle {
                id: subMenuViewHorizontalLine
                width: Constants.MAIN_MENU_LINE_H_SIZE
                height: Constants.MAIN_MENU_LINE_V_SIZE
                color: Constants.COLOR_MAIN_DARK_GRAY
                visible: Boolean(Functions.getIsVisibleSubMenuViewHorizontalLine(index))
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.bottom
            }
            Component.onDestruction: {
                imageArrowSubMenu.source = ""
            }
        }
    }

    Component.onCompleted: {
        console.log("Window mainWindow Completed")
        controler.initTranslation()
        mainFormID.propertShowAnimation = controler.isAnimationsEnabled()
        gapi.initTranslation()
    }
}
