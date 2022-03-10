/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2019 - 2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsSignSettingsForm {

    Keys.onPressed: {
        console.log("PageDefinitionsSignSettingsForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Connections {
        target: gapi
        onSignalInstalledRootCACert: {
            propertyBusyIndicator.running = false
            propertyIsRootCaCertInstalled = successful

            var titlePopup = successful ? 
                            qsTranslate("GAPI","STR_POPUP_SUCESS") + controler.autoTr :
                            qsTranslate("GAPI","STR_POPUP_ERROR") + controler.autoTr
            var bodyPopup = successful ?
                            qsTranslate("PageDefinitionsApp","STR_ROOT_CERT_INSTALLED_SUCCESS") + controler.autoTr :
                            qsTranslate("PageDefinitionsApp","STR_ROOT_CERT_INSTALLED_ERROR") + controler.autoTr
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
    }

    propertyCheckboxRegister{
        onCheckedChanged: {
            propertyCheckboxRegister.checked ? gapi.setRegCertValue(true) :
                                               gapi.setRegCertValue(false)
            if (propertyCheckboxRegister.enabled) {
                restart_dialog.headerTitle = qsTranslate("Popup Card","STR_POPUP_REGISTER_CERTIFICATE") + controler.autoTr
                restart_dialog.open()
            }
        }
    }
    propertyCheckboxRemove{
        onCheckedChanged: {
            propertyCheckboxRemove.checked ? gapi.setRemoveCertValue(true) :
                                             gapi.setRemoveCertValue(false)

            if (propertyCheckboxRemove.enabled) {
                restart_dialog.headerTitle = qsTranslate("Popup Card","STR_POPUP_REMOVE_CERTIFICATE") + controler.autoTr
                restart_dialog.open()
            }
        }
    }
    propertyCheckboxDisable{
        onCheckedChanged: controler.setOutlookSuppressNameChecks(propertyCheckboxDisable.checked);
    }
    Connections {
        target: propertyTextFieldTimeStamp
        onEditingFinished: {
            console.log("Editing TimeStamp finished host: " + propertyTextFieldTimeStamp.text);
            controler.setTimeStampHostValue(propertyTextFieldTimeStamp.text)
        }
    }
    propertyCheckboxTimeStamp{
        onCheckedChanged: if (!propertyCheckboxTimeStamp.checked ){
                              controler.setTimeStampHostValue("http://ts.cartaodecidadao.pt/tsa/server")
                              propertyTextFieldTimeStamp.text = ""
                          }
    }
    propertyCheckboxCMDRegRemember{
        onCheckedChanged: controler.setAskToRegisterCmdCertValue(!propertyCheckboxCMDRegRemember.checked)
    }

    propertyLoadCMDCertsButton {
        onClicked: {
            mainFormID.propertyCmdDialog.enableConnections()
            mainFormID.propertyCmdDialog.open(GAPI.RegisterCert)
        }
    }

    propertyButtonAddCACert {
        onClicked: {
            propertyBusyIndicator.running = true
            gapi.installRootCACert();
        }
    }

    Component.onCompleted: {
        console.log("Page definitionsSignSettings onCompleted")

        propertyCheckboxRegister.enabled = false
        propertyCheckboxRemove.enabled = false
        if (Qt.platform.os === "windows") {
            propertyCheckboxRegister.checked = gapi.getRegCertValue()
            propertyCheckboxRemove.checked = gapi.getRemoveCertValue()
            propertyCheckboxRegister.enabled = true
            propertyCheckboxRemove.enabled = true
            propertyIsRootCaCertInstalled = gapi.areRootCACertsInstalled()
        }else{
            propertyRectAddRootCACert.visible = false
            propertyRectAppCertificates.visible = false
            propertyRectAppTimeStamp.anchors.top = propertyRectAppCertificates.top
        }

        if (controler.getTimeStampHostValue().length > 0
                && controler.getTimeStampHostValue() !== "http://ts.cartaodecidadao.pt/tsa/server"){
            propertyCheckboxTimeStamp.checked = true
            propertyTextFieldTimeStamp.text = controler.getTimeStampHostValue()
        }else{
            propertyCheckboxTimeStamp.checked = false
        }

        if (Qt.platform.os === "windows") {
            propertyCheckboxDisable.enabled = controler.isOutlookInstalled()
            if (propertyCheckboxDisable.enabled)
                propertyCheckboxDisable.checked = controler.getOutlookSuppressNameChecks()
        } else {
            propertyRectOffice.visible = false
        }

        if (Qt.platform.os === "windows") {
            propertyCheckboxCMDRegRemember.checked = !controler.getAskToRegisterCmdCertValue()
        } else {
            propertyRectLoadCMDCerts.visible = false
        }

        if(propertyRectAppCertificates.visible){
            propertyDateAppCertificates.forceActiveFocus()
        }else{
            propertyDateAppTimeStamp.forceActiveFocus()
        }

        console.log("Page definitionsSignSettings onCompleted finished")
    }
    function toggleSwitch(element){
        element.checked = !element.checked
    }
    function handleKeyPressed(key, callingObject){
        var direction = getDirection(key)
        switch(direction){
            case Constants.DIRECTION_UP:
                if(callingObject === propertyDateAppCertificates && !propertyRowMain.atYEnd){
                    propertyRowMain.flick(0, - getMaxFlickVelocity())
                }
                else if(!propertyRowMain.atYBeginning)
                    propertyRowMain.flick(0, Constants.FLICK_Y_VELOCITY)
                break;

            case Constants.DIRECTION_DOWN:
                if(callingObject === propertyButtonAddCACert && !propertyRowMain.atYBeginning){
                    propertyRowMain.flick(0, getMaxFlickVelocity());
                }
                else if(!propertyRowMain.atYEnd)
                    propertyRowMain.flick(0, - Constants.FLICK_Y_VELOCITY)
                break;
        }
    }
    function getDirection(key){
        var direction = Constants.NO_DIRECTION;
        if (key == Qt.Key_Backtab || key == Qt.Key_Up || key == Qt.Key_Left){
            direction = Constants.DIRECTION_UP;
        }
        else if (key == Qt.Key_Tab || key == Qt.Key_Down || key == Qt.Key_Right){
            direction = Constants.DIRECTION_DOWN;
        }
        return direction;
    }
    function getMaxFlickVelocity(){
        // use visible area of flickable object to calculate
        // a smooth flick velocity
        return 200 + Constants.FLICK_Y_VELOCITY_MAX * (1 - propertyRowMain.visibleArea.heightRatio)
    }
}
