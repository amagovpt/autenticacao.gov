/*-****************************************************************************

 * Copyright (C) 2017-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsAppForm {

    Keys.onPressed: {
        console.log("PageDefinitionsAppForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Connections {
        target: gapi
        onSignalCardAccessError:{
            propertyComboBoxReader.model = gapi.getRetReaderList()
        }
        onSignalCardDataChanged:{
            propertyComboBoxReader.model = gapi.getRetReaderList()
        }
        onSignalCardChanged:{
            propertyComboBoxReader.model = gapi.getRetReaderList()
        }
    }
    Connections {
        target: controler
    }

    propertyComboBoxReader.onActivated:  {
        console.log("propertyComboBoxReader onActivated index = "
                    + propertyComboBoxReader.currentIndex)
        gapi.setReaderByUser(propertyComboBoxReader.currentIndex)
    }
    propertyComboBoxReader.onModelChanged: {
        console.log("propertyComboBoxReader onModelChanged index = "
                    + propertyComboBoxReader.currentIndex)
        propertyComboBoxReader.currentIndex = gapi.getReaderIndex()
        console.log("propertyComboBoxReader onModelChanged index = "
                    + propertyComboBoxReader.currentIndex)
    }

    propertyCheckboxEnablePinpad {
        onCheckedChanged: { 
            controler.setPinpadEnabledValue(propertyCheckboxEnablePinpad.checked) 

            if (propertyCheckboxEnablePinpad.enabled) {
                restart_dialog.headerTitle = qsTranslate("PageDefinitionsApp","STR_USE_PINPAD") + controler.autoTr
                restart_dialog.open()
            }
        }
    }

    propertyCheckboxAutoStart{
        onCheckedChanged: propertyCheckboxAutoStart.checked ? controler.setStartAutoValue(true) :
                                                              controler.setStartAutoValue(false)
    }
    propertyCheckboxStartAutoupdate{
        onCheckedChanged: controler.setStartAutoupdateValue(propertyCheckboxStartAutoupdate.checked)
    }
    propertyRadioButtonUK{
        onCheckedChanged: {
            if (propertyRadioButtonUK.checked){
                controler.setGuiLanguageString("en")
                propertyRadioButtonPT.checked = false
            }
            updatePages()
        }
    }
    propertyRadioButtonPT{
        onCheckedChanged: {
            if (propertyRadioButtonPT.checked){
                controler.setGuiLanguageString("nl")
                propertyRadioButtonUK.checked = false
            }

            updatePages()

        }
    }

    propertyCheckboxShowAnime{
        onCheckedChanged: if(propertyCheckboxShowAnime.checked){
                              controler.setShowAnimationsValue(true)
                              mainFormID.propertShowAnimation = true
                          }else{
                              controler.setShowAnimationsValue(false)
                              mainFormID.propertShowAnimation = false
                          }
    }

    propertyCheckboxUseSystemScale {
        onCheckedChanged: {
            controler.setUseSystemScaleValue(propertyCheckboxUseSystemScale.checked)
            if (propertyCheckboxUseSystemScale.enabled) {
                restart_dialog.headerTitle = qsTranslate("PageDefinitionsApp","STR_SCALE_APPLICATION_TITLE") + controler.autoTr
                restart_dialog.open()
            }
        }
    }
    propertyComboBoxScaling.onActivated:  {
        console.log("propertyComboBoxScaling onActivated index = "
                    + propertyComboBoxScaling.currentIndex)
        controler.setApplicationScaleValue(propertyComboBoxScaling.currentIndex)
        restart_dialog.headerTitle = qsTranslate("PageDefinitionsApp","STR_SCALE_APPLICATION_TITLE") + controler.autoTr
        restart_dialog.open()
    }
    propertyComboBoxScaling.onModelChanged: {
        propertyComboBoxScaling.currentIndex = controler.getApplicationScaleValue()
        console.log("propertyComboBoxScaling onModelChanged index = "
                    + propertyComboBoxScaling.currentIndex)
    }

    propertyCheckBoxDebugMode {
        onCheckedChanged: {
            var debugFilename = controler.setDebugModeValue(propertyCheckBoxDebugMode.checked)

            if (propertyCheckBoxDebugMode.enabled) {
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_DEBUG_MODE") + controler.autoTr
                var bodyPopup = ""
                if (debugFilename === "") {
                    restart_dialog.headerTitle = titlePopup
                    restart_dialog.open()
                    return
                } else {
                    bodyPopup = qsTranslate("Popup Card", "STR_POPUP_WIN_DEBUG_FILE_ERROR").arg(debugFilename) + controler.autoTr
                }
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
        }
    }

    propertyComboboxAccelGraphics{
        onActivated:  {
            console.log("propertyComboboxAccelGraphics onActivated index = "
                        + propertyComboboxAccelGraphics.currentIndex)
            controler.setGraphicsAccelValue(propertyComboboxAccelGraphics.currentIndex)
            restart_dialog.headerTitle = qsTranslate("Popup Card","STR_POPUP_ACCEL_GRAPHICS") + controler.autoTr
            restart_dialog.open()
        }
        onModelChanged: {
            console.log("propertyComboboxAccelGraphics onModelChanged index = "
                        + propertyComboboxAccelGraphics.currentIndex)
            propertyComboboxAccelGraphics.currentIndex = controler.getGraphicsAccelValue();
            console.log("propertyComboboxAccelGraphics onModelChanged index = "
                        + propertyComboboxAccelGraphics.currentIndex)
        }
    }
    propertyCheckboxSystemProxy{
        onCheckedChanged: if (propertyCheckboxSystemProxy.checked ){
                              propertyCheckboxProxy.checked = false
                              controler.setProxySystemValue(true)
                          }else{
                              controler.setProxySystemValue(false)
                          }
    }
    propertyCheckboxProxy{
        onCheckedChanged: if (propertyCheckboxProxy.checked ){
                              propertyCheckboxSystemProxy.checked = false
                          }else{
                              controler.setProxyHostValue("")
                              controler.setProxyPortValue(0)
                              propertyTextFieldAdress.text = ""
                              propertyTextFieldPort.text = ""
                          }
    }

    Connections {
        target: propertyTextFieldAdress
        onEditingFinished: {
            console.log("Editing proxy host finished - host: " + propertyTextFieldAdress.text);
            controler.setProxyHostValue(propertyTextFieldAdress.text)
        }
    }
    Connections {
        target: propertyTextFieldPort
        onEditingFinished: {
            console.log("Editing proxy host port finished - port: " + propertyTextFieldPort.text);
            controler.setProxyPortValue(parseInt(propertyTextFieldPort.text))
        }
    }

    propertyCheckboxAutProxy{
        onCheckedChanged: if (!propertyCheckboxAutProxy.checked ){
                              controler.setProxyUsernameValue("")
                              controler.setProxyPwdValue("")
                              propertyTextFieldAutUser.text = ""
                              propertyTextFieldAutPass.text = ""
                          }
    }
    Connections {
        target: propertyTextFieldAutUser
        onEditingFinished: {
            //console.log("Editing proxy username finished - username: " + propertyTextFieldAutUser.text);
            controler.setProxyUsernameValue(propertyTextFieldAutUser.text)
        }
    }
    Connections {
        target: propertyTextFieldAutPass
        onEditingFinished: {
            //console.log("Editing proxy password finished - password: " + propertyTextFieldAutPass.text);
            controler.setProxyPwdValue(propertyTextFieldAutPass.text) 
        }
    }

    Component.onCompleted: {
        console.log("Page definitions onCompleted")

        propertyComboBoxReader.model = gapi.getRetReaderList()

        propertyCheckboxEnablePinpad.checked = controler.getPinpadEnabledValue()
        propertyCheckboxEnablePinpad.enabled = true

        if (Qt.platform.os === "windows") {
            propertyCheckboxAutoStart.checked = controler.getStartAutoValue()

            propertyCheckboxUseSystemScale.checked = controler.getUseSystemScaleValue()
            propertyCheckboxUseSystemScale.enabled = true
        }else{
            propertyRectAppStart.visible = false
            propertyRectStartAutoupdate.anchors.top = propertyRectReader.bottom

            propertyCheckboxUseSystemScale.visible = false
            propertyComboBoxScaling.width = propertyComboBoxScaling.parent.width - propertyTextManualScaling.width - 2*10- Constants.SIZE_TEXT_V_SPACE

            // Only Windows supports ANGLE option
            propertyComboboxAccelGraphics.model = propertyComboboxAccelGraphics.model.slice(0,-1)
        }

        propertyCheckboxStartAutoupdate.checked = controler.getStartAutoupdateValue()

        if(controler.getGuiLanguageString() === "nl"){
            propertyRadioButtonUK.checked = false
            propertyRadioButtonPT.checked = true
        }else{
            propertyRadioButtonUK.checked = true
            propertyRadioButtonPT.checked = false
        }

        propertyCheckboxShowAnime.checked = controler.getShowAnimationsValue()

        if (controler.getDebugModeValue()) {
            propertyCheckBoxDebugMode.checked = true
        }
        else {
            propertyCheckBoxDebugMode.checked = false
        }
        propertyCheckBoxDebugMode.enabled = true

        updatePages()

        if (Qt.platform.os === "linux") {
            propertyCheckboxSystemProxy.visible = false
            propertyCheckboxProxy.anchors.top = propertyRectAppNetworkCheckBox.top
            propertyCheckboxAutProxy.anchors.top = propertyRectAppNetworkCheckBox.top
            propertyRectAppNetworkCheckBox.height = propertyCheckboxProxy.height
                                                    + propertyTextFieldAdress.height
                                                    + Constants.SIZE_TEXT_V_SPACE
        }else{
            propertyCheckboxSystemProxy.checked = controler.getProxySystemValue()
        }

        if(!propertyCheckboxSystemProxy.checked || Qt.platform.os === "linux"){
            propertyCheckboxProxy.checked = controler.getProxyHostValue().length > 0 ? true : false
        }

        propertyTextFieldAdress.text = controler.getProxyHostValue()
        propertyTextFieldPort.text = controler.getProxyPortValue() > 0 ?
                    controler.getProxyPortValue().toString() :
                    ""

        propertyCheckboxAutProxy.checked = controler.getProxyUsernameValue().length > 0 ? true : false
        propertyTextFieldAutUser.text = controler.getProxyUsernameValue()
        propertyTextFieldAutPass.text = controler.getProxyPwdValue()

        console.log("Page definitions onCompleted finished")
        if(mainFormID.propertyPageLoader.propertyForceFocus)
            propertyTextReader.forceActiveFocus()
    }
    function updateGraphicsTextField(){
            propertyGraphicsTextField.text = qsTranslate("PageDefinitionsApp","STR_ACCEL_GRAPHICS_TEXT")  + " : "
                    + OpenGLInfo.majorVersion + "." + OpenGLInfo.minorVersion
                    + "\n" + qsTranslate("PageDefinitionsApp","STR_ACCEL_GRAPHICS_TEXT_DESCRIPTION")
        }
    function updatePages(){
        // Update submenu
        mainFormID.state = Constants.MenuState.NORMAL
        mainFormID.propertySubMenuListView.model.clear()
        for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.count; ++i) {
            /*console.log("Sub Menu indice " + i + " - "
                        + mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName);*/
            mainFormID.propertySubMenuListView.model
            .append({
                        "subName": qsTranslate("MainMenuBottomModel",
                                               mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName),
                        "expand": mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i)
                        .expand,
                        "url": mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i)
                        .url
                    })
        }

        updateGraphicsTextField()

        mainFormID.propertySubMenuListView.currentIndex = 2
        mainFormID.propertyPageLoader.forceActiveFocus()
    }
    function handleKeyPressed(key, callingObject){
        if (propertyComboBoxScaling.focus)
                    return;
        var direction = getDirection(key)
        switch(direction){
            case Constants.DIRECTION_UP:
                if(callingObject === propertyTextReader && !propertyRowMain.atYEnd){
                    propertyRowMain.flick(0, - getMaxFlickVelocity())
                }
                else if(!propertyRowMain.atYBeginning)
                    propertyRowMain.flick(0, Constants.FLICK_Y_VELOCITY)
                break;

            case Constants.DIRECTION_DOWN:
                if(callingObject === propertyTextFieldAutPass && !propertyRowMain.atYBeginning){
                    propertyRowMain.flick(0, getMaxFlickVelocity());
                }
                else if(callingObject === propertyCheckboxAutProxy
                        && !propertyRowMain.atYBeginning
                        && !propertyCheckboxAutProxy.checked){
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
    function toggleSwitch(element){
        element.checked = !element.checked
    }
    function toggleRadio(element){
        if (!element.checked)
            element.checked = true
    }
}
