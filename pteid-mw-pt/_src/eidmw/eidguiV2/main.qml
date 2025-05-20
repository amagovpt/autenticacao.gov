/*-****************************************************************************

 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Window 2.2

import "scripts/Constants.js" as Constants
import "scripts/Functions.js" as Functions

import "components/" as Components

//Import C++ defined enums
import eidguiV2 1.0

Window {
    id: mainWindow
    visible: true
	flags: Qt.Window | Qt.CustomizeWindowHint | Qt.FramelessWindowHint

    width: Constants.SCREEN_MINIMUM_WIDTH
    height: Constants.SCREEN_MINIMUM_HEIGHT
    minimumWidth: Constants.SCREEN_MINIMUM_WIDTH
    minimumHeight: Constants.SCREEN_MINIMUM_HEIGHT

    title: "Autenticação.Gov"

    FontLoader {
        id: lato;
        name: "Lato" // This avoids printing error on application start

        source: {
            // on linux, load installed font by name
            // on windows and macOS use source property to point to distributed font file
            if (Qt.platform.os === "windows" || Qt.platform.os === "osx") {
                controler.getFontFile("lato")
            }
            else ""
        }
    }

    onWidthChanged: {
        //console.log("Resizing app width: " + width + "height" + height)
        mainFormID.propertyMainMenuView.width = Functions.getMainMenuWidth(width)
        mainFormID.propertySubMenuView.width = Functions.getSubMenuWidth(width)
        mainFormID.propertyContentPagesView.width = Functions.getContentPagesMenuWidth(width)
    }

    onHeightChanged: {
        //console.log("Resizing app height: " + height)
        //console.log("Position: (" + x + "," + y + ")")
    }
    onXChanged: {
    }
    onYChanged: {
    }
    onClosing:{
        // check for unsaved notes when closing application
        close.accepted = false;
        if(!handleUnsavedNotes(-1,"", Constants.QUIT_APPLICATION)){
            mainWindow.visible = false;
            gapi.quitApplication();
        }

    }

    Connections {
        target: controler
        onSignalRestoreWindows: {
            console.log("Signal onSignalRestoreWindows")
            // Minimize window in case it was just hidden (it would remain hidden otherwise)
            mainWindow.showMinimized()

            if (Qt.platform.os === "osx")
                mainWindow.flags = Qt.Window | Qt.CustomizeWindowHint | Qt.FramelessWindowHint;
            mainWindow.raise();
            mainWindow.show();
            mainWindow.requestActivate();
        }
        onSignalLanguageChangedError: {
            var titlePopup = "Erro / Error"
            var bodyPopup = "Erro na leitura dos idiomas. Por favor, reinstale a aplicação <br/>
Load language error. Please reinstall the application"
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }

        property var autoUpdateApp: true
        property var autoUpdateCerts: true
        property var isAutoUpdateAlreadyDetected: false

        onSignalAutoUpdateNews: {
            mainFormID.propertyNotificationMenu.addAppNews(news_list)
        }

        onSignalAutoUpdateAvailable: {
            // Do not show dialog when update page is open
            if(mainFormID.propertyMainMenuBottomListView.currentIndex != 0
                    || mainFormID.propertySubMenuListView.currentIndex != 5){
                    mainFormID.propertyNotificationMenu.addUpdate(arg1, arg2, arg3, url_list, updateType)
            }
        }

        onSignalAutoUpdateFail: {

            console.log("No updates or startup auto update failed.")
            if(updateType == GAPI.AutoUpdateApp){
                console.log("No App updates or startup auto update failed.")
                if(autoUpdateApp){
                    autoUpdateApp = false
                }
            }
            else if(updateType == GAPI.AutoUpdateNews){
                console.log("No News updates or startup auto update failed.")
            } else {
                console.log("No Certs updates or startup auto update failed.")
                if(autoUpdateCerts) {
                    autoUpdateCerts = false
                }
                if (error_code == GAPI.InstallFailed){
                    var titlePopup = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTIFICATES_FAIL")
                    var bodyPopup = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_INSTALL_CERTS_FAIL")
                            + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_CONTACT_SUPPORT")
                    mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                }
            }
        }
        onSignalAutoUpdateSuccess: {
            if(updateType == GAPI.AutoUpdateCerts){
                restart_dialog.headerTitle = qsTranslate("PageDefinitionsUpdates","STR_UPDATED_CERTIFICATES")
                restart_dialog.open()
            }
        }
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            console.log("Signal onSignalGenericError")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("Popup Card","STR_GENERIC_ERROR_MSG") + "<br/><br/>" +
                            qsTranslate("Popup Card","STR_ERROR_CODE") + error_code
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalImportCertificatesFail: {
            console.log("Signal onSignalImportCertificatesFail")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("Popup Card","STR_CERTIFICATES_IMPORT_ERROR_MSG")
                + "<br/><br/>" + qsTranslate("Popup Card","STR_GENERIC_CARD_ERROR_MSG") 
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalRemoveCertificatesFail: {
            console.log("Signal onSignalRemoveCertificatesFail")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("Popup Card","STR_CERTIFICATES_REMOVE_ERROR_MSG")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalLanguageChangedError: {
            var titlePopup = "Erro / Error"
            var bodyPopup = "Erro na leitura dos idiomas. Por favor, reinstale a aplicação <br/>
Load language error. Please reinstall the application"
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
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
        onSignalCardChanged: {
            if(mainFormID.propertyMainMenuListView.model.get(mainFormID.propertyMainMenuListView.currentIndex).isCard &&
                    mainFormID.propertyMainMenuListView.model.count > 0)
                mainMenuPressed(mainFormID.propertyMainMenuListView.currentIndex)
        }
    }

    Dialog {
        id: readerContext
        width: 400
        height: 250
        visible: false
        font.family: lato.name
        modal: true
        // Center dialog in the main view
        x: parent.width * 0.5 - readerContext.width * 0.5
        y: parent.height * 0.5 - readerContext.height * 0.5

        Accessible.role: Accessible.AlertMessage
        Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS") + labelReaderContext.text + textMessageTop.text


        header: Label {
            id: labelReaderContext
            text: qsTr("STR_WARNING") 
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            font.family: lato.name
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
                    text: qsTr("STR_MULTIPLE_CARDS_DETECTED") 
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
                    text:  qsTr("STR_CHANGE_READER_INFO") 
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

    Dialog {
        property alias headerTitle: restartDlgTitle.text

        id: restart_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        modal: true
        closePolicy: Popup.CloseOnEscape

        // Center dialog in the main view
        x: parent.width * 0.5 - restart_dialog.width * 0.5
        y: parent.height * 0.5 - restart_dialog.height * 0.5

        header: Label {
            id: restartDlgTitle
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: rectRestartPopUp.activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            id: rectRestartPopUp
            width: parent.width
            height: 50

            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS") 
                    + restartDlgTitle.text + labelRestartText.text

            Keys.enabled: true
            KeyNavigation.tab: cancelButton
            KeyNavigation.down: cancelButton
            KeyNavigation.right: cancelButton
            KeyNavigation.backtab: restartButton
            KeyNavigation.up: restartButton

            Item {
                id: rectRestartLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelRestartText
                    text: qsTranslate("Popup Card", "STR_POPUP_RESTART_APP") 
                    font.bold: activeFocus
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                }
            }
        }

        Item {
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.horizontalCenter: parent.horizontalCenter
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: cancelButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File", "STR_POPUP_RESTART_LATER") 
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: restartButton
                    KeyNavigation.down: restartButton
                    KeyNavigation.right: restartButton
                    KeyNavigation.backtab: rectRestartPopUp
                    KeyNavigation.up: rectRestartPopUp
                    highlighted: activeFocus
                    onClicked: {
                        restart_dialog.close()
                    }
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
                Button {
                    id: restartButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File", "STR_POPUP_RESTART_NOW") 
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectRestartPopUp
                    KeyNavigation.down: rectRestartPopUp
                    KeyNavigation.right: rectRestartPopUp
                    KeyNavigation.backtab: cancelButton
                    KeyNavigation.up: cancelButton
                    highlighted: activeFocus
                    onClicked: {
                        gapi.quitApplication(true)
                    }
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
        }
        onOpened: {
            rectRestartPopUp.forceActiveFocus()
        }
        onClosed: {
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
    }


    Dialog {
        id: unsaved_notes_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        modal: true
        closePolicy: Popup.CloseOnEscape

        // to save last menu pressed before dialog opens
        property int index: -1
        property string url: ""
        property int menu: -1

        // Center dialog in the main view
        x: parent.width * 0.5 - unsaved_notes_dialog.width * 0.5
        y: parent.height * 0.5 - unsaved_notes_dialog.height * 0.5

        header: Label {
            id: titleText
            text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_TITLE") 
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: rectPopUp.activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            id: rectPopUp
            width: unsaved_notes_dialog.availableWidth
            height: 50

            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS") 
                    + qsTranslate("PageCardNotes","STR_UNSAVED_NOTES") + titleText.text + labelText.text

            Keys.enabled: true
            KeyNavigation.tab: rejectButton
            KeyNavigation.down: rejectButton
            KeyNavigation.right: rejectButton
            KeyNavigation.backtab: continueButton
            KeyNavigation.up: continueButton

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    text: unsaved_notes_dialog.menu == Constants.QUIT_APPLICATION ?
                            qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_DESCRIPTION_EXIT")  :
                            qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_DESCRIPTION") 
                    font.bold: activeFocus
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                }
            }
        }

        Item {
            width: unsaved_notes_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: rejectButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_LEAVE") 
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: continueButton
                    KeyNavigation.down: continueButton
                    KeyNavigation.right: continueButton
                    KeyNavigation.backtab: rectPopUp
                    KeyNavigation.up: rectPopUp
                    highlighted: activeFocus ? true : false
                    onClicked: {
                        unsaved_notes_dialog.reject()
                    }
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
                Button {
                    id: continueButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_STAY") 
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectPopUp
                    KeyNavigation.down: rectPopUp
                    KeyNavigation.right: rectPopUp
                    KeyNavigation.backtab: rejectButton
                    KeyNavigation.up: rejectButton
                    highlighted: activeFocus ? true : false
                    onClicked: {
                        unsaved_notes_dialog.accept()
                    }
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
        }
        onRejected:{
            // user rejected unsaved notes
            mainFormID.propertyPageLoader.propertyBackupText = ""
            mainFormID.propertyPageLoader.propertyUnsavedNotes = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS

            // go to where the user pressed before note saving dialog opened
            switch(unsaved_notes_dialog.menu){
                case Constants.MAIN_MENU_PRESSED:
                    mainMenuPressed(unsaved_notes_dialog.index);
                    break;
                case Constants.SUB_MENU_PRESSED:
                    subMenuPressed(unsaved_notes_dialog.index,unsaved_notes_dialog.url);
                    break;
                case Constants.MAIN_BOTTOM_MENU_PRESSED:
                    mainMenuBottomPressed(unsaved_notes_dialog.index);
                    break;
                case Constants.HOME_ICON_PRESSED:
                    Functions.goToHome();
                    break;
                case Constants.KEY_NAVIGATION_EXIT_NOTES:
                    mainFormID.propertyPageLoader.propertyForceFocus = false
                    mainFormID.propertyPageLoader.source = ""
                    mainFormID.propertyPageLoader.source = unsaved_notes_dialog.url
                    mainFormID.propertySubMenuListView.forceActiveFocus()
                    break;
                case Constants.QUIT_APPLICATION:
                    gapi.quitApplication()
            }
        }
        onClosed: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
        onAccepted: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
        onOpened: {
            rectPopUp.forceActiveFocus()
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
                name: Constants.MenuState.FIRST_RUN
            },
            State{
                name: Constants.MenuState.HOME
            },
            State{
                name: Constants.MenuState.EXPAND
            },
            State{
                name: Constants.MenuState.NORMAL
            }
        ]
        transitions: [
            Transition {
                from: Constants.MenuState.FIRST_RUN
                to: Constants.MenuState.NORMAL
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
                from: Constants.MenuState.HOME
                to: Constants.MenuState.NORMAL
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
                from: Constants.MenuState.NORMAL
                to: Constants.MenuState.HOME
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
                from: Constants.MenuState.FIRST_RUN
                to: Constants.MenuState.HOME
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
                from: Constants.MenuState.EXPAND
                to: Constants.MenuState.HOME
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
                    if (( mainFormID.state == Constants.MenuState.HOME) && (!running))
                    {
                        mainFormID.propertyPageLoader.propertyAnimationExtendedFinished = false
                    }
                }
            },
            Transition {
                to: Constants.MenuState.EXPAND
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
                    id: animationExpandMainMenuPagesWidth
                    target: mainFormID.propertyMainMenuView
                    property: "width"
                    easing.type: Easing.OutQuad
                    to: mainFormID.propertyMainView.width * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE;
                    duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_MOVE_VIEW : 0
                }
                NumberAnimation
                {
                    target: mainFormID.propertyContentPagesView
                    property: "opacity"
                    easing.type: Easing.Linear
                    to: 1;
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
                    if (( mainFormID.state == Constants.MenuState.EXPAND) && (!running))
                    {
                        mainFormID.propertyPageLoader.propertyAnimationExtendedFinished = true
                    }
                }
            },
            Transition {
                from: Constants.MenuState.EXPAND
                to: Constants.MenuState.NORMAL
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
                    if (( mainFormID.state == Constants.MenuState.NORMAL) && (!running))
                    {
                        mainFormID.propertyPageLoader.propertyAnimationExtendedFinished = false
                    }
                    if (( mainFormID.state == Constants.MenuState.HOME) && (!running))
                    {
                        /* If user went back to home in the middle of this transition, set values as if transition was completed*/
                        mainFormID.propertySubMenuViewMenu.opacity = 1
                        mainFormID.propertySubMenuView.color = "white"
                        mainFormID.propertyContentPagesView.width = mainFormID.propertyMainView.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                        mainFormID.propertySubMenuView.width = mainFormID.propertyMainView.width * Constants.SUB_MENU_VIEW_RELATIVE_SIZE;
                    }
                }
            }
        ]
        Keys.onPressed: {
            console.log("Main Menu Key Pressed: " + event.key)
            // if backing out of notes, using keys
            if(mainFormID.propertySubMenuListView.focus){
                var CARD_NOTES_URL = "contentPages/card/PageCardNotes.qml"
                mainWindow.handleUnsavedNotes(mainFormID.propertySubMenuListView.currentIndex, CARD_NOTES_URL, Constants.KEY_NAVIGATION_EXIT_NOTES)
            }

            if(propertyLogoContent.focus){
                mainFormID.propertyMainMenuListView.currentIndex = 0
                mainFormID.propertyMainMenuListView.forceActiveFocus()
            }
        }
        Component.onCompleted: {
            mainFormID.propertyLogoContent.forceActiveFocus()
            if(controler.getNotShowHelpStartUp()){
                mainFormID.state = Constants.MenuState.HOME;
            }else{
                mainFormID.state = Constants.MenuState.FIRST_RUN
            }

            if ( mainFormID.state === Constants.MenuState.FIRST_RUN){
                mainFormID.propertyMainMenuView.width = mainWindow.width * 2 * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
                mainFormID.propertySubMenuView.width = 0
                mainFormID.propertyContentPagesView.width = mainWindow.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                mainFormID.propertyPageLoader.source = "contentPages/home/PageHome.qml"
            }else{
                mainFormID.propertyMainMenuView.width = mainWindow.width
            }
            if (!controler.getAskToRegisterCmdCertValue())
                mainFormID.propertyLogoContent.forceActiveFocus()

            // Do not select any option
            mainFormID.propertyMainMenuListView.currentIndex = -1
            mainFormID.propertyMainMenuBottomListView.currentIndex = -1
            console.log("MainForm Completed testmode: "+controler.getTestMode())

            console.log("debugMode: " + controler.getDebugModeValue())
            if (controler.getDebugModeValue()) {
                mainFormID.propertyTitleBarContainer.propertyTitleBar.color = Constants.COLOR_TITLEBAR_DEBUG
            }

            // Take shortcut to signature page
            if (gapi.getShortcutFlag() == GAPI.ShortcutIdSign) {
                mainFormID.propertShowAnimation = false
                mainMenuPressed(Constants.MAIN_MENU_SIGN_PAGE_INDEX)
                //TODO: we shouldn't need this to make sure the contentPage gets the expanded space
                mainWindow.setWidth(Constants.SCREEN_MINIMUM_WIDTH + 1)
                mainFormID.propertShowAnimation = controler.isAnimationsEnabled()
                return
            }
        }
        propertyImageLogo {
            onClicked: {
                // if there are no unsaved notes
                if(!handleUnsavedNotes(-1, "", Constants.HOME_ICON_PRESSED)){
                    mainFormID.propertyPageLoader.source = ""
                    mainFormID.state = Constants.MenuState.HOME
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    propertySubMenuListView.currentIndex = -1
                    propertyMainMenuListView.currentIndex = -1
                    propertyMainMenuBottomListView.currentIndex = -1
                }
            }
        }
        propertyLogoContent {
            onFocusChanged: {
                if (propertyLogoContent.focus) {
                    propertyMainMenuListView.currentIndex = -1
                    propertyMainMenuBottomListView.currentIndex = -1
                }
            }
        }
        propertyMouseAreaSubMenuViewReduced {
            onClicked: {
                Functions.goToSubMenu()
            }
        }
    }

    Connections {
        target: mainFormID.propertyNotificationMenu
        onPopupClosed: {
            if (mainFormID.propertyMainMenuListView.currentIndex !== -1) {
                mainFormID.propertyMainMenuListView.forceActiveFocus()
            } else if (mainFormID.propertyMainMenuBottomListView.currentIndex !== -1) {
                mainFormID.propertyMainMenuBottomListView.forceActiveFocus()
            }
        }
    }

    Component {
        id: mainMenuDelegate
        Item {
            width: mainFormID.propertyMainMenuListView.width
            height: mainFormID.propertyMainMenuListView.height
                    / mainFormID.propertyMainMenuListView.count

            Keys.onPressed: {
                console.log("mainMenu onPressed:" + event.key)
                Functions.detectBackKeys(event.key, Constants.MenuState.HOME)
            }

            Keys.onDownPressed: {
                console.log("onDownPressed currentIndex:"
                            + mainFormID.propertyMainMenuListView.currentIndex)

                if(mainFormID.propertyMainMenuListView.currentIndex ==
                        mainFormID.propertyMainMenuListView.count - 1){
                    console.log("Move to botton Menu - onDownPressed")
                    mainFormID.propertyMainMenuListView.currentIndex = -1
                    mainFormID.propertyMainMenuBottomListView.forceActiveFocus()
                    mainFormID.propertyMainMenuBottomListView.currentIndex = 0

                } else {
                    mainFormID.propertyMainMenuListView.currentIndex++
                }
            }

            Keys.onUpPressed: {
                console.log("onUpPressed currentIndex:"
                            + mainFormID.propertyMainMenuListView.currentIndex)

                if(mainFormID.propertyMainMenuListView.currentIndex <=
                        0 ){
                    console.log("Move to botton Menu - onUpPressed")
                    mainFormID.propertyMainMenuListView.currentIndex = -1
                    mainFormID.propertyMainMenuBottomListView.forceActiveFocus()
                    mainFormID.propertyMainMenuBottomListView.currentIndex =
                            mainFormID.propertyMainMenuBottomListView.count - 1
                } else {
                    mainFormID.propertyMainMenuListView.currentIndex--
                }
            }

            Keys.onRightPressed: {
                mainMenuPressed(index)
            }

            Keys.onSpacePressed: {
                mainMenuPressed(index)
            }

            Keys.onReturnPressed: {
                mainMenuPressed(index)
            }


            MouseArea {
                id: mouseAreaMainMenu
                anchors.fill: parent
                hoverEnabled: true
                onClicked: mainMenuPressed(index)
                z:1 // MouseArea above buttons
            }

            Accessible.role: Accessible.Button
            Accessible.name: buttonMain.text

            Button {
                id: buttonMain
                text: qsTranslate("MainMenuModel", name) 
                width: parent.width
                height: parent.height
                focus:  mainFormID.propertyMainMenuListView.currentIndex === index ? true : false
                opacity: 0
                Keys.onTabPressed: {
                    if(mainFormID.propertyMainMenuListView.currentIndex ==
                            mainFormID.propertyMainMenuListView.count - 1){
                        mainFormID.propertyMainMenuListView.currentIndex = -1
                        mainFormID.propertyMainMenuBottomListView.forceActiveFocus()
                        mainFormID.propertyMainMenuBottomListView.currentIndex = 0
                    } else {
                        mainFormID.propertyMainMenuListView.currentIndex++
                    }
                }

                Keys.onBacktabPressed: {
                    if(mainFormID.propertyMainMenuListView.currentIndex <= 0){
                        mainFormID.propertyMainMenuListView.currentIndex = -1
                        mainFormID.propertyLogoContent.forceActiveFocus()
                    } else {
                        mainFormID.propertyMainMenuListView.currentIndex--
                    }
                }

                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
                onClicked: {
                    console.log(index)
                    mainMenuPressed(index)
                }
                onFocusChanged: {
                    if(focus === true){
                        mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                        mainFormID.propertyMainMenuListView.currentIndex = index
                        mainFormID.propertyMainMenuListView.forceActiveFocus()
                    }
                }
            }









            Text {
                id: textMain
                text: qsTranslate("MainMenuModel", name) 
                color:  mainFormID.propertyMainMenuListView.currentIndex === index ?
                            Constants.COLOR_TEXT_MAIN_MENU_SELECTED :
                            Constants.COLOR_TEXT_MAIN_MENU_DEFAULT
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.family: lato.name
                font.capitalization: Font.AllUppercase
                font.weight: mouseAreaMainMenu.containsMouse
                             || mainFormID.propertyMainMenuListView.currentIndex === index
                             ? Font.Bold
                             : Font.Normal
                font.pixelSize: mainFormID.propertyMainMenuListView.currentIndex === index
                    ? Constants.SIZE_TEXT_SUB_MENU
                    : Constants.SIZE_TEXT_SUB_MENU
                focus:  mainFormID.propertyMainMenuListView.currentIndex === index ?
                            true :
                            false
            }
            Image {
                id: imageArrowMainMenu
                width: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                height: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                fillMode: Image.PreserveAspectFit
                x: parent.width * Constants.IMAGE_ARROW_MAIN_MENU_RELATIVE
                anchors.verticalCenter: parent.verticalCenter
                source: Functions.getMainMenuArrowSource(index, mouseAreaMainMenu.containsMouse)
                visible: mainFormID.state === Constants.MenuState.HOME ?
                             false:
                             true
            }
            Rectangle {
                id: mainMenuViewHorizontalLine
                width: Constants.MAIN_MENU_LINE_H_SIZE
                height: Constants.MAIN_MENU_LINE_V_SIZE
                color: Constants.COLOR_MAIN_MIDDLE_GRAY
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
            width: mainFormID.propertyMainMenuBottomListView.width / 3
            height: mainFormID.propertyMainMenuBottomListView.height


            Keys.onPressed: {
                console.log("mainMenuBottom onPressed:" + event.key)
                Functions.detectBackKeys(event.key, Constants.MenuState.HOME)
                    }

            Keys.onReturnPressed: {
                console.log("MainMenuBottom onReturnPressed")
                mainMenuBottomPressed(index)
            }
            Keys.onSpacePressed: {
                mainMenuBottomPressed(index)
            }
            Keys.onRightPressed: {
                mainMenuBottomPressed(index)
            }

            Keys.onBackPressed: {
                console.log("MainMenuBottom onBackPressed")
                mainFormID.propertyMainMenuListView.forceActiveFocus();
                mainFormID.propertyMainMenuListView.currentIndex = 0;
                mainFormID.propertyMainMenuBottomListView.currentIndex = -1;
            }

            Keys.onDownPressed: {
                console.log("MainMenuBottom onDownPressed currentIndex:"
                     + mainFormID.propertyMainMenuBottomListView.currentIndex)

                if (mainFormID.propertyMainMenuBottomListView.currentIndex ==
                        mainFormID.propertyMainMenuBottomListView.count - 1) {
                    console.log("Move to Main Menu")
                    mainFormID.propertyMainMenuListView.currentIndex = 0
                    mainFormID.propertyMainMenuListView.forceActiveFocus()
                    mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                } else {
                    mainFormID.propertyMainMenuBottomListView.currentIndex++
                }
            }

            Keys.onTabPressed: {
                console.log("MainMenuBottom onTabPressed currentIndex:"
                    + mainFormID.propertyMainMenuBottomListView.currentIndex)

                if (mainFormID.propertyMainMenuBottomListView.currentIndex ==
                        mainFormID.propertyMainMenuBottomListView.count - 1) {
                    console.log("Move to Main Menu")
                    mainFormID.propertyMainMenuListView.currentIndex = 0
                    mainFormID.propertyMainMenuListView.forceActiveFocus()
                    mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                } else {
                    mainFormID.propertyMainMenuBottomListView.currentIndex++
                }
            }

            Keys.onUpPressed: {
                console.log("MainMenuBottom onUpPressed currentIndex:"
                    + mainFormID.propertyMainMenuBottomListView.currentIndex)

                if (mainFormID.propertyMainMenuBottomListView.currentIndex <= 0 ) {
                    console.log("Move to Main Menu")
                    mainFormID.propertyMainMenuListView.currentIndex =
                        mainFormID.propertyMainMenuListView.count -1
                    mainFormID.propertyMainMenuListView.forceActiveFocus()
                    mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                } else {
                    mainFormID.propertyMainMenuBottomListView.currentIndex--
                }
            }

            Keys.onBacktabPressed: {
                console.log("MainMenuBottom onBackTabPressed currentIndex:"
                            + mainFormID.propertyMainMenuBottomListView.currentIndex)

                if (mainFormID.propertyMainMenuBottomListView.currentIndex <= 0 ) {
                    console.log("Move to Main Menu")
                    mainFormID.propertyMainMenuListView.currentIndex =
                        mainFormID.propertyMainMenuListView.count -1
                    mainFormID.propertyMainMenuListView.forceActiveFocus()
                    mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                } else {
                    mainFormID.propertyMainMenuBottomListView.currentIndex--
                }
            }

            MouseArea {
                id: mouseAreaMainMenuBottom
                anchors.fill: parent
                hoverEnabled: true
                onClicked: mainMenuBottomPressed(index)
                z:1 // MouseArea above buttons
            }
            Image {
                id: imageMainMenuBottom
                width: Constants.SIZE_IMAGE_BOTTOM_MENU
                height: Constants.SIZE_IMAGE_BOTTOM_MENU
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                source: Functions.getBottomMenuImgSource(index,mouseAreaMainMenuBottom.containsMouse)
            }

            Accessible.role: Accessible.Button
            Accessible.name: buttonMainMenuBottom.text

            Button {
                id: buttonMainMenuBottom
                text: qsTranslate("MainMenuBottomModel", name) 
                width: parent.width
                height: parent.height
                focus:  mainFormID.propertyMainMenuBottomListView.currentIndex === index ? true : false
                opacity: 0
                onClicked: mainMenuBottomPressed(index)
                Keys.onTabPressed: {
                    if(mainFormID.propertyMainMenuBottomListView.currentIndex ==
                            mainFormID.propertyMainMenuBottomListView.count - 1){
                        // Move to PageLoader or Main Menu
                        if(mainFormID.propertyPageLoader.source == "qrc:/contentPages/home/PageHome.qml"){
                            mainFormID.propertyPageLoader.item.propertyMainItem.forceActiveFocus()
                        } else {
                            mainFormID.propertyLogoContent.forceActiveFocus()
                        }
                            mainFormID.propertyMainMenuListView.currentIndex = -1
                            mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                    } else {
                        mainFormID.propertyMainMenuBottomListView.currentIndex++
                    }

                }
                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
                onFocusChanged: {
                    if(focus === true){
                        mainFormID.propertyMainMenuListView.currentIndex = -1
                        mainFormID.propertyMainMenuBottomListView.currentIndex = index
                        mainFormID.propertyMainMenuBottomListView.forceActiveFocus()
                    }
                }
            }
            Text {
                id: textMainMenuBottom
                visible: false
                text: qsTranslate("MainMenuBottomModel", name) 
                focus:  mainFormID.propertyMainMenuBottomListView.currentIndex === index ?
                            true :
                            false
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

            Keys.onPressed: {
                console.log("subMenu onPressed:" + event.key)
                Functions.detectBackKeys(event.key, Constants.MenuState.HOME)
            }

            function moveSelectionDown() {
                console.log(" subMenu moveSelectionDown currentIndex:"
                            + mainFormID.propertySubMenuListView.currentIndex)

                if(mainFormID.propertySubMenuListView.currentIndex ==
                        mainFormID.propertySubMenuListView.count - 1){
                    console.log("Move to top")
                    mainFormID.propertySubMenuListView.currentIndex = 0

                } else {
                    mainFormID.propertySubMenuListView.currentIndex++
                }
            }
            function moveSelectionUp() {
                console.log("subMenu moveSelectionUp currentIndex:"
                            + mainFormID.propertySubMenuListView.currentIndex)

                if(mainFormID.propertySubMenuListView.currentIndex <=
                        0 ){
                    console.log("Move to Main Menu")
                    mainFormID.propertySubMenuListView.currentIndex =
                            mainFormID.propertySubMenuListView.count - 1
                } else {
                    mainFormID.propertySubMenuListView.currentIndex--
                }
            }
            Keys.onDownPressed: {
                moveSelectionDown()
            }
            Keys.onTabPressed: {
                moveSelectionDown()
            }
            Keys.onUpPressed: {
                moveSelectionUp()
            }
            Keys.onBacktabPressed: {
                moveSelectionUp()
            }

            Keys.onReturnPressed: {
                console.log("subMenu onReturnPressed")
                subMenuPressed(index, url)
            }
            Keys.onSpacePressed: {
                subMenuPressed(index, url)
            }
            Keys.onRightPressed: {
                subMenuPressed(index, url)
            }

            MouseArea {
                id: mouseAreaSubMenu
                anchors.fill: parent
                hoverEnabled: true
                onClicked: subMenuPressed(index, url)
                z:1 // MouseArea above buttons
            }

            Accessible.role: Accessible.MenuItem
            Accessible.name: buttonSubMenuBottom.text

            Button {
                id: buttonSubMenuBottom
                text: qsTranslate("MainMenuModel", subName)
                width: parent.width
                height: parent.height
                focus:  mainFormID.propertySubMenuListView.currentIndex === index ? true : false
                opacity: 0
                onClicked: subMenuPressed(index, url)
                Keys.onTabPressed: {
                    moveSelectionDown()
                }
                Keys.onBacktabPressed: {
                    moveSelectionUp()
                }
                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
                onFocusChanged: {
                    if(focus === true){
                        mainFormID.propertySubMenuListView.currentIndex = index
                        mainFormID.propertySubMenuListView.forceActiveFocus()
                    }
                }
            }

            Text {
                text: qsTranslate("MainMenuModel", subName)
                color: Functions.getSubNameColor(index, mouseAreaSubMenu.containsMouse)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.family: lato.name
                font.weight: mouseAreaSubMenu.containsMouse ?
                                 Font.Bold :
                                 Font.Normal
                font.pixelSize: mainFormID.propertySubMenuListView.currentIndex === index && mainFormID.propertySubMenuListView.activeFocus
                    ? Constants.SIZE_TEXT_SUB_MENU + 2
                    : Constants.SIZE_TEXT_SUB_MENU
                font.bold: mainFormID.propertySubMenuListView.currentIndex === index && mainFormID.propertySubMenuListView.activeFocus
                    ? true
                    : false

                wrapMode: Text.Wrap
                width: parent.width - 2 * imageArrowSubMenu.width
                       - 2 * Constants.IMAGE_ARROW_SUB_MENU_MARGIN
                horizontalAlignment: Text.AlignHCenter
                focus:  mainFormID.propertySubMenuListView.currentIndex === index ?
                            true :
                            false
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
                color: Constants.COLOR_MAIN_MIDDLE_GRAY
                visible: Boolean(Functions.getIsVisibleSubMenuViewHorizontalLine(index))
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.bottom
            }
            Component.onDestruction: {
                imageArrowSubMenu.source = ""
            }
        }
    }
    Item {
        width:  mainFormID.propertyMainMenuView.width
        height: Constants.SIZE_TEXT_LABEL
        anchors {
            bottom: parent.bottom
            left: parent.left
            bottomMargin: Constants.SIZE_ROW_V_SPACE
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.bold: mouseAreaVersionLabel.containsMouse
            font.family: lato.name
            color:  mainFormID.opacity == Constants.OPACITY_MAIN_FOCUS 
                ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY

            text: qsTranslate("PageHelpAbout","STR_HELP_APP_VERSION") 
                    + " " + controler.getAppVersion().split("-")[0]
            MouseArea {
                id: mouseAreaVersionLabel
                anchors.fill: parent
                hoverEnabled: true
                onClicked:{
                    mainMenuBottomPressed(1)
                    subMenuPressed(2, mainFormID.propertyMainMenuBottomListView.model.get(1).subdata.get(2).url)
                }
                z:1 // MouseArea above buttons
            }
        }
    }

    Component.onCompleted: {
        console.log("Window mainWindow Completed")
        mainFormID.propertShowAnimation = controler.isAnimationsEnabled()

        if(mainWindow.screen.desktopAvailableHeight < mainWindow.height || mainWindow.screen.desktopAvailableWidth < mainWindow.width) {
            console.log("Resize window to fit the screen")
            mainWindow.visibility = Window.Maximized
        }
        
        if(Qt.platform.os === "osx") {
            if(!gapi.isMacOSApplicationFolder()) {
                mainFormID.propertyPageLoader.activateGeneralPopup(qsTranslate("main", "STR_WARNING_NOT_CORRECT_FOLDER_MAC"),
                                                                   qsTranslate("main", "STR_WARNING_NOT_CORRECT_FOLDER_MAC_BODY"), false)
            }
        }

        // Only update certificates on Windows and MacOS
        if (Qt.platform.os !== "linux")
            controler.autoUpdatesCerts()
        
        controler.autoUpdatesNews()
        if (controler.getStartAutoupdateValue()) {
            controler.autoUpdateApp()
        }
        if (controler.getAskToSetCacheValue()) {
            mainFormID.propertyNotificationMenu.addCacheSettings()   
        }
        if (controler.getAskToSetTelemetryValue()) {
            mainFormID.propertyNotificationMenu.addTelemetrySettings()
        }
        if(Qt.platform.os === "windows"){
            mainFormID.propertyNotificationMenu.addCmdSettings(!controler.getAskToRegisterCmdCertValue())
        }

        // Should we retry any of the telemetry status requests
        if(gapi.getTelemetryStatus() === GAPI.RetryEnable)
            gapi.enableTelemetry()
        else if(gapi.getTelemetryStatus() === GAPI.RetryDisable)
            gapi.disableTelemetry()

        gapi.updateTelemetry(GAPI.Startup)
    }

    function mainMenuPressed(index){
        // if there are unsaved notes
        if(handleUnsavedNotes(index,"", Constants.MAIN_MENU_PRESSED)){
            return
        }

        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        mainFormID.propertyMainMenuBottomListView.currentIndex = -1
        mainFormID.propertyMainMenuListView.currentIndex = index

        // Clear the sub menu
        mainFormID.propertySubMenuListView.model.clear()

        if(mainFormID.propertyMainMenuListView.model.get(index).expand === true){
            mainFormID.propertyPageLoader.propertyForceFocus = true
            mainFormID.state = Constants.MenuState.EXPAND
            mainFormID.propertyPageLoader.source =
                    mainFormID.propertyMainMenuListView.model.get(index).subdata.get(0).url
        }else{
            // Load a new sub menu
            for(var i = 0; i < mainFormID.propertyMainMenuListView.model.get(index).subdata.count; ++i) {
                var hasIcao = gapi.hasICAO();
                var hasOnlyIcao = gapi.hasOnlyICAO()
                if(mainFormID.propertyMainMenuListView.model.get(index).isCard &&
                        (mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).isIcao && !hasIcao) ||
                        (hasIcao && hasOnlyIcao && !mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).isIcao))
                {
                    continue
                }

                /*console.log("Sub Menu indice " + i + " - "
                            + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).name + " - "
                            + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).expand + " - "
                            + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).url)*/
                mainFormID.propertySubMenuListView.model
                .append({
                            "subName": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).name,
                            "expand": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).expand,
                            "url": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).url,
                            "isIcao": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).isIcao,
                        })
            }

            // Open the content page of the first item of the new sub menu
            mainFormID.propertyPageLoader.propertyForceFocus = false
            mainFormID.propertyPageLoader.source =
                    mainFormID.propertySubMenuListView.model.get(0).url
            mainFormID.propertySubMenuListView.currentIndex = 0
            /* Setting the state should be done after setting the source: changing the state causes the PDFPreview to call
               another (unnecessary) requestPixmap if the signature pages are loaded. */
            mainFormID.state = Constants.MenuState.NORMAL

            /*console.log("Main Menu index = " + index);
            console.log("Set focus sub menu")
            console.log("Sub menu count" + mainFormID.propertySubMenuListView.count)*/
            mainFormID.propertySubMenuListView.forceActiveFocus()

        }
    }

    function subMenuPressed(index, url){
        // if there are unsaved notes
        if(handleUnsavedNotes(index,url,Constants.SUB_MENU_PRESSED)){
            return
        }

        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        mainFormID.propertySubMenuListView.currentIndex = index
        console.log("Sub Menu index = " + index);

        // Clean the content page
        mainFormID.propertyPageLoader.propertyForceFocus = true
        //var temp = url
        mainFormID.propertyPageLoader.source = ""
        mainFormID.propertyPageLoader.source = url
        /* Setting the state should be done after setting the source: changing the state causes the PDFPreview to call
                        another (unnecessary) requestPixmap if the signature pages are loaded. */
        mainFormID.state = Constants.MenuState.NORMAL

    }

    function mainMenuBottomPressed(index){
        // if there are unsaved notes
        if(handleUnsavedNotes(index,"",Constants.MAIN_BOTTOM_MENU_PRESSED)){
            return
        }

        if (index == 2) {
            mainFormID.propertyNotificationMenu.open()
            return
        }

        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        // Do not select any option
        mainFormID.propertyMainMenuListView.currentIndex = -1
        mainFormID.propertyMainMenuBottomListView.currentIndex = index
        // Clear list model and then load a new sub menu
        mainFormID.propertySubMenuListView.model.clear()
        for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.count; ++i) {
            /*console.log("Sub Menu indice " + i + " - "
                        + mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(i).subName);*/
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
        // Open the content page of the first item of the new sub menu
        mainFormID.propertyPageLoader.propertyForceFocus = false
        mainFormID.propertyPageLoader.source =
                mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(0).url
        mainFormID.propertySubMenuListView.currentIndex = 0
        /* Setting the state should be done after setting the source: changing the state causes the PDFPreview to call 
           another (unnecessary) requestPixmap if the signature pages are loaded. */
        mainFormID.state = Constants.MenuState.NORMAL

        console.log("Main Menu Bottom index = " + index);
        mainFormID.propertySubMenuListView.forceActiveFocus()
    }

    function handleUnsavedNotes(index, url, menu){
        if ( mainFormID.propertyPageLoader.propertyUnsavedNotes ){
            // Unsaved notes dialog
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            unsaved_notes_dialog.open()
            unsaved_notes_dialog.visible = true
            unsaved_notes_dialog.index = index
            unsaved_notes_dialog.url = url
            unsaved_notes_dialog.menu = menu
        }

        return mainFormID.propertyPageLoader.propertyUnsavedNotes
    }

    function reloadNotificationCenter() {
        mainFormID.propertyNotificationMenu.clearModels()

        // Only update certificates on Windows and MacOS
        if (Qt.platform.os !== "linux")
            controler.autoUpdatesCerts()
        
        if (controler.getStartAutoupdateValue()) {
            controler.autoUpdateApp()
        }
        if (controler.getAskToSetCacheValue()) {
            mainFormID.propertyNotificationMenu.addCacheSettings()   
        }
        if (controler.getAskToSetTelemetryValue()) {
            mainFormID.propertyNotificationMenu.addTelemetrySettings()
        }
        if(Qt.platform.os === "windows"){
            mainFormID.propertyNotificationMenu.addCmdSettings(!controler.getAskToRegisterCmdCertValue())
        }
    }
}
