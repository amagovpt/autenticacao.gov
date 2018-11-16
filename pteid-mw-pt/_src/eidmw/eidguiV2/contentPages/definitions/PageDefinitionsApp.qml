import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsAppForm {

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

    propertyCheckboxAutoStart{
        onCheckedChanged: propertyCheckboxAutoStart.checked ? controler.setStartAutoValue(true) :
                                                              controler.setStartAutoValue(false)
    }
    propertyRadioButtonUK{
        onCheckedChanged: {
            if (propertyRadioButtonUK.checked)
                controler.setGuiLanguageString("en")
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
                controler.setGuiLanguageString("nl")
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
        onCheckedChanged: propertyCheckboxRegister.checked ? gapi.setRegCertValue(true) :
                                                             gapi.setRegCertValue(false)
    }
    propertyCheckboxRemove{
        onCheckedChanged: propertyCheckboxRemove.checked ? gapi.setRemoveCertValue(true) :
                                                           gapi.setRemoveCertValue(false)
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
        console.log("Page definitions onCompleted")

        propertyComboBoxReader.model = gapi.getRetReaderList()

        if (Qt.platform.os === "windows") {
            propertyCheckboxAutoStart.checked = controler.getStartAutoValue()
        }else{
            propertyRectAppStart.visible = false
            propertyRectAppLanguage.anchors.top = propertyRectReader.bottom
        }

        if(controler.getGuiLanguageString() === "nl"){
            propertyRadioButtonUK.checked = false
            propertyRadioButtonPT.checked = true
        }else{
            propertyRadioButtonUK.checked = true
            propertyRadioButtonPT.checked = false
        }

        propertyCheckboxShowAnime.checked = controler.getShowAnimationsValue()

        if (Qt.platform.os === "windows") {
            propertyCheckboxRegister.checked = gapi.getRegCertValue()
            propertyCheckboxRemove.checked = gapi.getRemoveCertValue()
        }else{
            propertyRectAppCertificates.visible = false
            propertyRectAppTimeStamp.anchors.top = propertyRectAppLook.bottom
        }

        if (controler.getTimeStampHostValue().length > 0
                && controler.getTimeStampHostValue() !== "http://ts.cartaodecidadao.pt/tsa/server"){
            propertyCheckboxTimeStamp.checked = true
            propertyTextFieldTimeStamp.text = controler.getTimeStampHostValue()
        }else{
            propertyCheckboxTimeStamp.checked = false
        }

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
    }
}
