import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsAppForm {
    Connections {
        target: gapi
        onSignalSetReaderComboIndex: {
            console.log("onSignalSetReaderComboIndex index = " + selected_reader)
            propertyComboBoxReader.currentIndex = selected_reader
        }
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
    propertyCheckboxAutoStart{
        onCheckedChanged: propertyCheckboxAutoStart.checked ? controler.setStartAutoValue(true) :
                                                              controler.setStartAutoValue(false)
    }
    propertyRadioButtonUK{
        onCheckedChanged: {
            if (propertyRadioButtonUK.checked)
                controler.setGuiLanguageCodeValue(GAPI.LANG_EN)
            // Update submenu
            mainFormID.state = "STATE_NORMAL"
            mainFormID.propertySubMenuListView.model.clear()
            for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.count; ++i) {
                console.log("Sub Menu indice " + i + " - "
                            + mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName);
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
            mainFormID.propertySubMenuListView.currentIndex = 2
        }
    }
    propertyRadioButtonPT{
        onCheckedChanged: {
            if (propertyRadioButtonPT.checked)
                controler.setGuiLanguageCodeValue(GAPI.LANG_NL)
            // Update submenu
            mainFormID.state = "STATE_NORMAL"
            mainFormID.propertySubMenuListView.model.clear()
            for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.count; ++i) {
                console.log("Sub Menu indice " + i + " - "
                            + mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName);
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
            mainFormID.propertySubMenuListView.currentIndex = 2
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
    propertyCheckboxRegister{
        onCheckedChanged: propertyCheckboxRegister.checked ? controler.setRegCertValue(true) :
                                                             controler.setRegCertValue(false)
    }
    propertyCheckboxRemove{
        onCheckedChanged: propertyCheckboxRemove.checked ? controler.setRemoveCertValue(true) :
                                                           controler.setRemoveCertValue(false)
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
                              controler.setTimeStampHostValue("")
                              propertyTextFieldTimeStamp.text = ""
                          }
    }

    propertyCheckboxProxy{
        onCheckedChanged: if (!propertyCheckboxProxy.checked ){
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
            console.log("Editing proxy username finished - username: " + propertyTextFieldAutUser.text);
            controler.setProxyUsernameValue(propertyTextFieldAutUser.text)
        }
    }
    Connections {
        target: propertyTextFieldAutPass
        onEditingFinished: {
            console.log("Editing proxy password finished - password: " + propertyTextFieldAutPass.text);
            controler.setProxyPwdValue(parseInt(propertyTextFieldAutPass.text))
        }
    }

    Component.onCompleted: {
        console.log("Page difinitions onCompleted")

        propertyComboBoxReader.model = gapi.getRetReaderList()

        if (Qt.platform.os === "windows") {
            propertyCheckboxAutoStart.checked = controler.getStartAutoValue()
        }else{
            propertyRectAppStart.visible = false
            propertyRectAppLanguage.anchors.top = propertyRectReader.bottom
        }

        if(controler.getGuiLanguageCodeValue() === GAPI.LANG_EN){
            propertyRadioButtonUK.checked = true
            propertyRadioButtonPT.checked = false
        }else{
            propertyRadioButtonUK.checked = false
            propertyRadioButtonPT.checked = true
        }

        propertyCheckboxShowAnime.checked = controler.getShowAnimationsValue()

        if (Qt.platform.os === "windows") {
            propertyCheckboxRegister.checked = controler.getRegCertValue()
            propertyCheckboxRemove.checked = controler.getRemoveCertValue()
        }else{
            propertyRectAppCertificates.visible = false
            propertyRectAppTimeStamp.anchors.top = propertyRectAppLook.bottom
        }

        propertyCheckboxTimeStamp.checked = controler.getTimeStampHostValue().length > 0 ? true : false
        propertyTextFieldTimeStamp.text = controler.getTimeStampHostValue()

        propertyCheckboxProxy.checked = controler.getProxyHostValue().length > 0 ? true : false
        propertyTextFieldAdress.text = controler.getProxyHostValue()
        propertyTextFieldPort.text = controler.getProxyPortValue() > 0 ?
                    controler.getProxyPortValue().toString() :
                    ""

        propertyCheckboxAutProxy.checked = controler.getProxyUsernameValue().length > 0 ? true : false
        propertyTextFieldAutUser.text = controler.getProxyUsernameValue()
        propertyTextFieldAutPass.text = controler.getProxyPwdValue()

        console.log("Page difinitions onCompleted finished")
    }
}
