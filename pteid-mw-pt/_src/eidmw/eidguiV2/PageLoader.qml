/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

import "scripts/Constants.js" as Constants
import "components" as Components

Loader{
    id: theloader
    opacity:1
    width: parent.width
    height: parent.height

    property alias propertyGeneralPopUp: generalPopUp
    property alias propertyRectPopUp: rectPopUp
    property alias propertyGeneralTitleText: titleText
    property alias propertyGeneralPopUpLabelText: labelText.propertyText
    property bool propertyGeneralPopUpRetSubMenu: false // By default return to the Page Loader

    // Backup data about unfinished advance signature
    property alias propertyBackupfilesModel: backupfilesModel
    property bool propertyAnimationExtendedFinished: false
    property bool propertyBackupFormatPades: true
    property bool propertyBackupTempSign: false
    property bool propertyBackupSignAdd: false
    property variant attributeListBackup: []
    property bool propertyBackupSignShow: true
    property double propertyBackupCoordX: 0
    property double propertyBackupCoordY: 0
    property bool propertyBackupSignReduced: false
    property int propertyBackupPage: 1
    property bool propertyBackupLastPage: false
    property string propertyBackupLocal: ""
    property string propertyBackupReason: ""
    property string propertyBackupMobileNumber: ""
    property int propertyBackupMobileIndicatorIndex: 0
    property bool propertyBackupFromSignaturePage: false

    property bool propertyForceFocus: false

    // Backup data about unsaved notes
    property bool propertyUnsavedNotes: false
    property string propertyBackupText: ""
    property string propertyLoadedText: ""

    ListModel {
        id: backupfilesModel
    }

    Dialog {
        id: generalPopUp
        width: 400
        height: 200
        visible: false

        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - generalPopUp.width * 0.5
        y: parent.height * 0.5 - generalPopUp.height * 0.5

        header: Label {
            id: titleText
            text: "Error"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: rectPopUp.activeFocus ? true : false
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            id: rectPopUp
            width: generalPopUp.availableWidth
            height: generalPopUp.availableHeight - generalPopUp.padding

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                {
                    generalPopUp.close()
                }
            }

            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + titleText.text + labelText.propertyText.text
            KeyNavigation.tab: okButton
            KeyNavigation.down: okButton
            KeyNavigation.right: okButton
            KeyNavigation.backtab: okButton
            KeyNavigation.up: okButton

            Item {
                id: rectLabelText
                width: parent.width
                height: parent.height - Constants.HEIGHT_BOTTOM_COMPONENT - Constants.SIZE_TEXT_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                Components.Link {
                    id: labelText
                    height: parent.height
                    width: parent.width
                    propertyText.verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    propertyText.font.pixelSize: Constants.SIZE_TEXT_LABEL
                    propertyText.color: Constants.COLOR_TEXT_LABEL
                    propertyText.height: parent.height
                    anchors.bottom: parent.bottom
                }
            }
            Button {
                id: okButton
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectLabelText.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: rectPopUp
                KeyNavigation.down: rectPopUp
                KeyNavigation.right: rectPopUp
                KeyNavigation.backtab: rectPopUp
                KeyNavigation.up: rectPopUp
                highlighted: activeFocus ? true : false
                onClicked: {
                    generalPopUp.close()
                }
            }
        }
        onClosed: {
            // return to normal main window opacity
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            setReturnFocus(propertyGeneralPopUpRetSubMenu)
        }
        onOpened: {
            rectPopUp.forceActiveFocus()
        }
    }

    anchors{
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }
    function setReturnFocus(retvalue){
        propertyGeneralPopUpRetSubMenu = false;
        if(retvalue === false){
            mainFormID.propertyPageLoader.forceActiveFocus()
        } else {
            mainFormID.propertySubMenuListView.forceActiveFocus()
        }
    }

    function activateGeneralPopup(titlePopup, bodyPopup, returnToSubMenuWhenClosed){
        titleText.text = titlePopup
        labelText.propertyText.text = bodyPopup
        mainFormID.propertyPageLoader.propertyGeneralPopUpRetSubMenu = returnToSubMenuWhenClosed;

        // reduce main window opacity
        mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
        generalPopUp.visible = true;
        rectPopUp.forceActiveFocus();
    }
}
