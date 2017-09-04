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
    }
    Connections {
        target: controler
    }

    propertyComboBoxReader.onActivated:  {
            console.log("propertyComboBoxReader onActivated index = "
                        + propertyComboBoxReader.currentIndex)
            gapi.setReaderByUser(propertyComboBoxReader.currentIndex)
    }
    propertyCheckboxAutoRead{
        onCheckedChanged: propertyCheckboxAutoRead.checked ? controler.setAutoCardReadingValue(true) :
                                                            controler.setAutoCardReadingValue(false)

    }
    propertyCheckboxAutoStart{
        onCheckedChanged: propertyCheckboxAutoStart.checked ? controler.setStartAutoValue(true) :
                                                            controler.setStartAutoValue(false)
    }
    propertyCheckboxStart{
        onCheckedChanged: propertyCheckboxStart.checked ? controler.setStartMinimizedValue(true) :
                                                            controler.setStartMinimizedValue(false)
    }
    propertyRadioButtonUK{
        onCheckedChanged: propertyRadioButtonUK.checked ?
                              controler.setGuiLanguageCodeValue(GAPI.LANG_EN) :
                              controler.setGuiLanguageCodeValue(GAPI.LANG_NL)
    }
    propertyRadioButtonPT{
        onCheckedChanged: propertyRadioButtonPT.checked ?
                              controler.setGuiLanguageCodeValue(GAPI.LANG_NL) :
                              controler.setGuiLanguageCodeValue(GAPI.LANG_EN)
    }


    propertyCheckboxShowNot{
        onCheckedChanged: propertyCheckboxShowNot.checked ? controler.setShowNotificationValue(true) :
                                                            controler.setShowNotificationValue(false)
    }
    propertyCheckboxShowPhoto{
        onCheckedChanged: propertyCheckboxShowPhoto.checked ? controler.setShowPictureValue(true) :
                                                            controler.setShowPictureValue(false)
    }
    propertyCheckboxShowAnime{
        onCheckedChanged: propertyCheckboxShowAnime.checked ? controler.setShowAnimationsValue(true) :
                                                            controler.setShowAnimationsValue(false)
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

        for ( var i = 0; i < gapi.getRetReaderList().length; ++i ) {
            console.log("Reader List " + "i = " + i +" : "+ gapi.getRetReaderList()[i])
            propertyComboBoxReader.model = gapi.getRetReaderList()
        }

        propertyCheckboxAutoRead.checked = controler.getAutoCardReadingValue()
        if (Qt.platform.os === "linux") {
            propertyCheckboxAutoStart.visible = false
            propertyCheckboxStart.x = propertyRectAppStartCheckBox.width * 0.5
        }else{
            propertyCheckboxAutoStart.checked = controler.getStartAutoValue()
        }

        propertyCheckboxStart.checked = controler.getStartMinimizedValue()
        if(controler.getGuiLanguageCodeValue() === GAPI.LANG_EN){
            propertyRadioButtonUK.checked = true
            propertyRadioButtonPT.checked = false
        }else{
            propertyRadioButtonUK.checked = false
            propertyRadioButtonPT.checked = true
        }
        propertyCheckboxShowNot.checked = controler.getShowNotificationValue()
        propertyCheckboxShowPhoto.checked = controler.getShowPictureValue()
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
