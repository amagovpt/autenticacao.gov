/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Window 2.2

import "scripts/Functions.js" as Functions
import "scripts/Constants.js" as Constants

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

    FontLoader { id: lato; source: "qrc:/fonts/lato/Lato-Regular.ttf" }

    onWidthChanged: {
        console.log("Resizing app width: " + width + "height" + height)
        mainFormID.propertyMainMenuView.width = Functions.getMainMenuWidth(width)
        mainFormID.propertySubMenuView.width = Functions.getSubMenuWidth(width)
        mainFormID.propertyContentPagesView.width = Functions.getContentPagesMenuWidth(width)
    }

    onHeightChanged: {
        console.log("Resizing app height: " + height)
        console.log("Position: (" + x + "," + y + ")")
    }
    onXChanged: {
    }
    onYChanged: {
    }
    onClosing:{
        // check for unsaved notes when closing application
        close.accepted = false;
        if(!handleUnsavedNotes(-1,"", Constants.QUIT_APPLICATION)){
            gapi.quitApplication();
        }

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
                    "Erro na leitura dos idiomas. Por favor, reinstale a aplicação <br/>
Load language error. Please reinstall the application"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }

        property var autoUpdate: true
        onSignalAutoUpdateAvailable: {
            if(autoUpdate){
                console.log("There is an update available.")
                autoUpdate = false
                autoUpdateDialog.update_release_notes = release_notes
                autoUpdateDialog.update_installed_version = installed_version
                autoUpdateDialog.update_remote_version = remote_version
                autoUpdateDialog.open()
                labelTextTitle.forceActiveFocus();
            }
        }
        onSignalAutoUpdateFail: {
            if(autoUpdate){
                console.log("No updates or startup auto update failed.")
                autoUpdate = false
            }
        }
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            console.log("Signal onSignalGenericError")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("Popup Card","STR_POPUP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("Popup Card","STR_GENERIC_ERROR_MSG") + "<br/><br/>" +
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
                    "Erro na leitura dos idiomas. Por favor, reinstale a aplicação <br/>
Load language error. Please reinstall the application"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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
        property string update_release_notes: ""
        property string update_installed_version: ""
        property string update_remote_version: ""

        id: autoUpdateDialog
        width: 400
        height: 200
        font.family: lato.name
        // Center dialog in the main view
        x: parent.width * 0.5 - width * 0.5
        y: parent.height * 0.5 - height * 0.5
        modal: true

        header: Label {
            id: labelTextTitle
            visible: true
            text: qsTr("STR_AUTOUPDATE_TITLE")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
            KeyNavigation.tab: textAutoupdate
            KeyNavigation.down: textAutoupdate
        }

        Item {
            width: parent.width
            height: parent.height

            Item {
                id: rectMessage
                width: parent.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textAutoupdate
                    text: qsTr("STR_AUTOUPDATE_TEXT") + "\n\n"
                        + qsTr("STR_DISABLE_AUTOUPDATE_INFO")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: activeFocus ? Constants.SIZE_TEXT_LABEL_FOCUS : Constants.SIZE_TEXT_LABEL
                    font.bold: activeFocus
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                    KeyNavigation.tab: buttonCancelUpdate
                    KeyNavigation.down: buttonCancelUpdate
                    KeyNavigation.up: labelTextTitle
                    KeyNavigation.backtab: labelTextTitle
                }
            }
        }
        Button {
            id: buttonCancelUpdate
            width: Constants.WIDTH_BUTTON
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: qsTranslate("Popup File", "STR_POPUP_FILE_CANCEL")
            font.capitalization: Font.MixedCase
            KeyNavigation.tab: buttonInstallUpdate
            KeyNavigation.right: buttonInstallUpdate
            KeyNavigation.up: textAutoupdate
            KeyNavigation.backtab: textAutoupdate
            highlighted: activeFocus
            onClicked: {
                autoUpdateDialog.close()
            }
        }
        Button {
            id: buttonInstallUpdate
            width: Constants.WIDTH_BUTTON
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: qsTr("STR_UPDATE_INSTALL_BUTTON")
            font.capitalization: Font.MixedCase
            KeyNavigation.tab: labelTextTitle
            KeyNavigation.right: labelTextTitle
            KeyNavigation.left: buttonCancelUpdate
            KeyNavigation.backtab: buttonCancelUpdate
            highlighted: activeFocus
            onClicked: {
                autoUpdateDialog.close()

                // go to PageDefinitionsUpdatesForm
                mainMenuBottomPressed(0)
                mainFormID.propertyPageLoader.source =
                        mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(5).url
                mainFormID.propertySubMenuListView.currentIndex = 5
                mainFormID.propertySubMenuListView.forceActiveFocus()

                //re-emit signal for PageDefinitionsUpdatesForm
                controler.signalAutoUpdateAvailable(autoUpdateDialog.update_release_notes,
                                                    autoUpdateDialog.update_installed_version,
                                                    autoUpdateDialog.update_remote_version)
            }
        }
        onRejected: {
            autoUpdateDialog.open()
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

        Accessible.role: Accessible.AlertMessage
        Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS") + labelReaderContext.text + textMessageTop.text


        header: Label {
            id: labelReaderContext
            text: qsTr("STR_WARNING") + controler.autoTr
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
                    text: qsTr("STR_MULTIPLE_CARDS_DETECTED") + controler.autoTr
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
                    text:  qsTr("STR_CHANGE_READER_INFO") + controler.autoTr
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
        id: unsaved_notes_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name

        // to save last menu pressed before dialog opens
        property int index: -1
        property string url: ""
        property int menu: -1

        // Center dialog in the main view
        x: parent.width * 0.5 - unsaved_notes_dialog.width * 0.5
        y: parent.height * 0.5 - unsaved_notes_dialog.height * 0.5

        header: Label {
            id: titleText
            text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_TITLE") + controler.autoTr
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
            Accessible.name: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES") + titleText.text + labelText.text

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
                    text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_DESCRIPTION") + controler.autoTr
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
                    text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_LEAVE") + controler.autoTr
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
                    Keys.onReturnPressed: {
                        unsaved_notes_dialog.reject()
                    }
                }
                Button {
                    id: continueButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageCardNotes","STR_UNSAVED_NOTES_STAY") + controler.autoTr
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
                    Keys.onReturnPressed: {
                        unsaved_notes_dialog.accept()
                    }
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
                from: Constants.MenuState.NORMAL
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

            if(propertyImageLogoBottom.focus){
                mainFormID.propertyMainMenuListView.currentIndex = 0
                mainFormID.propertyMainMenuListView.forceActiveFocus()
            }
        }
        Component.onCompleted: {
            if(controler.getNotShowHelpStartUp()){
                mainFormID.state = Constants.MenuState.HOME;
            }else{
                mainFormID.state = Constants.MenuState.FIRST_RUN
            }

            //Open Advanced Signature on startup
            if (gapi.getShortcutFlag() > 0) {
                //mainFormID.propertyMainMenuView.width = mainWindow.width
                mainFormID.propertyMainMenuListView.currentIndex = 1
                mainFormID.propertySubMenuListView.currentIndex = 1

                mainFormID.state = Constants.MenuState.EXPAND
                // Clean the content page
                mainFormID.propertyPageLoader.source = "contentPages/services/PageServicesSignAdvanced.qml"
                //TODO: we shouldn't need this to make sure the contentPage gets the expanded space
                mainWindow.setWidth(Constants.SCREEN_MINIMUM_WIDTH + 1)
                return;
            }

            if ( mainFormID.state === Constants.MenuState.FIRST_RUN){
                console.log("Runing First time" + mainWindow.width)
                mainFormID.propertyMainMenuView.width = mainWindow.width * 2 * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
                mainFormID.propertySubMenuView.width = 0
                mainFormID.propertyContentPagesView.width = mainWindow.width * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
                mainFormID.propertyPageLoader.source = "contentPages/home/PageHome.qml"
            }else{
                mainFormID.propertyMainMenuView.width = mainWindow.width
            }
            mainFormID.propertyImageLogoBottom.forceActiveFocus()

            // Do not select any option
            mainFormID.propertyMainMenuListView.currentIndex = -1
            mainFormID.propertyMainMenuBottomListView.currentIndex = -1
            console.log("MainForm Completed testmode: "+controler.getTestMode())

            console.log("debugMode: " + controler.getDebugModeValue())
            if (controler.getDebugModeValue()) {
                mainFormID.propertyTitleBarContainer.propertyTitleBar.color = Constants.COLOR_TITLEBAR_DEBUG
            }
        }
        propertyImageLogo {
            onClicked: {
                // if there are no unsaved notes
                if(!handleUnsavedNotes(-1, "", Constants.HOME_ICON_PRESSED)){
                    mainFormID.state = Constants.MenuState.HOME
                    propertySubMenuListView.currentIndex = -1
                    propertyMainMenuListView.currentIndex = -1
                    propertyMainMenuBottomListView.currentIndex = -1
                }
            }
        }
        propertyImageLogoBottom {
            onFocusChanged: {
                if (propertyImageLogoBottom.focus) {
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

            Button {
                id: buttonMain
                text: qsTranslate("MainMenuModel", name) + controler.autoTr
                width: parent.width
                height: parent.height
                focus:  mainFormID.propertyMainMenuListView.currentIndex === index ? true : false
                opacity: 0
                Accessible.role: Accessible.Button
                Accessible.name: text
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
                text: qsTranslate("MainMenuModel", name) + controler.autoTr
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
            width: mainFormID.propertyMainMenuBottomListView.width / 2
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

                if(mainFormID.propertyMainMenuBottomListView.currentIndex ==
                        mainFormID.propertyMainMenuBottomListView.count - 1){
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

                if(mainFormID.propertyMainMenuBottomListView.currentIndex <=
                        0 ){
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
            Button {
                id: buttonMainMenuBottom
                text: qsTranslate("MainMenuBottomModel", name) + controler.autoTr
                width: parent.width
                height: parent.height
                focus:  mainFormID.propertyMainMenuBottomListView.currentIndex === index ? true : false
                opacity: 0
                Accessible.role: Accessible.Button
                Accessible.name: text
                onClicked: mainMenuBottomPressed(index)
                Keys.onTabPressed: {
                    if(mainFormID.propertyMainMenuBottomListView.currentIndex ==
                            mainFormID.propertyMainMenuBottomListView.count - 1){
                        // Move to PageLoader or Main Menu
                        if(mainFormID.propertyPageLoader.source == "qrc:/contentPages/home/PageHome.qml"){
                            var temp = mainFormID.propertyPageLoader.source
                            var url = "contentPages/home/PageHome.qml"
                            mainFormID.propertyPageLoader.source = ""
                            mainFormID.propertyPageLoader.source = url
                        } else {
                            mainFormID.propertyImageLogoBottom.forceActiveFocus()
                        }
                            mainFormID.propertyMainMenuListView.currentIndex = -1
                            mainFormID.propertyMainMenuBottomListView.currentIndex = -1
                    } else {
                        mainFormID.propertyMainMenuBottomListView.currentIndex++
                    }

                }
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
                text: qsTranslate("MainMenuBottomModel", name) + controler.autoTr
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
                onFocusChanged: {
                    if(focus === true){
                        mainFormID.propertySubMenuListView.currentIndex = index
                        mainFormID.propertySubMenuListView.forceActiveFocus()
                    }
                }
            }
            Accessible.role: Accessible.Button
            Accessible.name: Functions.getSubMenuName(index)

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

    Component.onCompleted: {
        console.log("Window mainWindow Completed")
        mainFormID.propertShowAnimation = controler.isAnimationsEnabled()
        gapi.setAppAsDlgParent()
        if(controler.getStartAutoupdateValue()){
            controler.autoUpdates()
        }
    }

    function mainMenuPressed(index){
        // if there are unsaved notes
        if(handleUnsavedNotes(index,"", Constants.MAIN_MENU_PRESSED)){
            return
        }

        mainFormID.propertyMainMenuBottomListView.currentIndex = -1
        mainFormID.propertyMainMenuListView.currentIndex = index

        // Clear list model and then load a new sub menu
        mainFormID.propertySubMenuListView.model.clear()
        for(var i = 0; i < mainFormID.propertyMainMenuListView.model.get(index).subdata.count; ++i) {
            /*console.log("Sub Menu indice " + i + " - "
                        + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).name + " - "
                        + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).expand + " - "
                        + mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).url)*/
            mainFormID.propertySubMenuListView.model
            .append({
                        "subName": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).name,
                        "expand": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).expand,
                        "url": mainFormID.propertyMainMenuListView.model.get(index).subdata.get(i).url
                    })
        }
        // Open the content page of the first item of the new sub menu
        mainFormID.propertyPageLoader.propertyForceFocus = false
        mainFormID.state = Constants.MenuState.NORMAL
        mainFormID.propertyPageLoader.source =
                mainFormID.propertyMainMenuListView.model.get(index).subdata.get(0).url
        mainFormID.propertySubMenuListView.currentIndex = 0

        /*console.log("Main Menu index = " + index);
        console.log("Set focus sub menu")
        console.log("Sub menu count" + mainFormID.propertySubMenuListView.count)*/
        mainFormID.propertySubMenuListView.forceActiveFocus()
    }

    function subMenuPressed(index, url){
        // if there are unsaved notes
        if(handleUnsavedNotes(index,url,Constants.SUB_MENU_PRESSED)){
            return
        } 

        mainFormID.propertySubMenuListView.currentIndex = index
        console.log("Sub Menu index = " + index);
        console.log("Sub Menu Pressed Expand Sub Menu" + mainFormID.propertySubMenuListView.model.get(0).expand)

        if(mainFormID.propertySubMenuListView.model.get(index).expand === true){
            // Clean the content page
            mainFormID.propertyPageLoader.propertyForceFocus = false
            mainFormID.state = Constants.MenuState.EXPAND
            mainFormID.propertyPageLoader.source = url
        }else{
            mainFormID.propertyPageLoader.propertyForceFocus = true
            mainFormID.state = Constants.MenuState.NORMAL
            //var temp = url
            mainFormID.propertyPageLoader.source = ""
            mainFormID.propertyPageLoader.source = url

        }
    }

    function mainMenuBottomPressed(index){
        // if there are unsaved notes
        if(handleUnsavedNotes(index,"",Constants.MAIN_BOTTOM_MENU_PRESSED)){
            return
        }

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
        mainFormID.state = Constants.MenuState.NORMAL
        mainFormID.propertyPageLoader.source =
                mainFormID.propertyMainMenuBottomListView.model.get(index).subdata.get(0).url
        mainFormID.propertySubMenuListView.currentIndex = 0

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
}
