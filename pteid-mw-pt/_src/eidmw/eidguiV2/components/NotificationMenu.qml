/*-****************************************************************************

 * Copyright (C) 2022 Tiago Barroso - <tiago.barroso@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../scripts/Constants.js" as Constants
import "../scripts/Functions.js" as Functions
import "../components/" as Components
//Import C++ defined enums
import eidguiV2 1.0

Item {

    id: menuContainer
    parent: mainWindow.contentItem
    anchors.fill: parent

    property bool reload: false
    property bool hasMandatory: false

    Popup {
        id: notificationMenuPopup
        width: 650
        height: 450
        modal: true
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose

        ListModel {
            id: model_recent
        }

        ListModel {
            id: model_read
        }
    
        Component {
            id: delegate

            Rectangle {
                id: notification_box
                width: parent.width
                height: model.activated ? news.height + update.height + definitions_cmd.height 
                        + definitions_cache.height : Constants.SIZE_IMAGE_BOTTOM_MENU * 2 
                color: Constants.COLOR_MAIN_SOFT_GRAY
                visible: !hasMandatory || model.mandatory
                clip: true

                Rectangle {
                    id: selected_part
                    width: 4
                    height: notification_box.height
                    color: model.read ? Constants.COLOR_GRAY : Constants.COLOR_MAIN_BLUE

                    anchors.left: parent.left
                }

                Image {
                    id: icon
                    width: Constants.SIZE_IMAGE_BOTTOM_MENU 
                    height: Constants.SIZE_IMAGE_BOTTOM_MENU 
                    fillMode: Image.PreserveAspectFit
                    source: chooseIcon(model.category, model.read)

                    anchors.left: selected_part.right
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 10
                }
        
                Text {
                    id: type
                    text: chooseCategory(model.category)
                    maximumLineCount: 1
                    color: model.read ? Constants.COLOR_GRAY : Constants.COLOR_MAIN_BLUE

                    font.family: lato.name
                    font.pixelSize: 10
                    
                    anchors.left: icon.right
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 10
                }

                Text {
                    text: model.title
                    visible: !model.activated
                    width: parent.width - icon.width - 40
                    color: Constants.COLOR_TEXT_BODY
                    elide: Text.ElideRight
                    maximumLineCount: 1

                    font.family: lato.name
                    font.pixelSize: 14

                    anchors.left: icon.right
                    anchors.leftMargin: 10
                    anchors.top: type.bottom
                    anchors.topMargin: 5
                }   

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: openNotification(index, model.read, model.activated)
                } 

                Components.Notification {
                    id: news
                    height: visible ? title.height + description.height + rightButton.height + 75 : 0
                    visible: model.category === "news" && model.activated

                    title.text: model.title
                    description.text: model.text
                    propertyUrl: model.link
                }

                Components.Notification {
                    id: definitions_cache
                    height: visible ? title.height + description.height + activatedCache.height * 2 + rightButton.height + 75 : 0
                    visible: model.category === "definitions_cache" && model.activated

                    title.text: model.title
                    description.text: model.text

                    CheckBox {
                        id: activatedCache
                        text: qsTranslate("main","STR_SET_CACHE_YES")
                        checked: false
                        onClicked: deactivatedCache.checked = false

                        anchors.top: definitions_cache.description.bottom
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE 
                        anchors.left: parent.left
                        anchors.leftMargin: Constants.SIZE_IMAGE_BOTTOM_MENU + 2 * 10 

                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_LABEL_FOCUS
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                    }

                    CheckBox {
                        id: deactivatedCache
                        text: qsTranslate("main","STR_SET_CACHE_NO")
                        checked: false
                        onClicked: activatedCache.checked = false

                        anchors.top: activatedCache.bottom
                        anchors.left: parent.left
                        anchors.leftMargin: Constants.SIZE_IMAGE_BOTTOM_MENU + 2 * 10 
                        anchors.right: parent.right
                        anchors.rightMargin: Constants.MARGIN_NOTIFICATION_CENTER

                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_LABEL_FOCUS
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                    }

                    rightButton {
                        text: qsTranslate("main","STR_SET_CACHE_PROCEED")
                        enabled: activatedCache.checked || deactivatedCache.checked
                        onClicked: setCacheSettings(index, model, activatedCache.checked)
                    }
                }

                Components.Notification {
                    id: definitions_cmd
                    height: visible ? title.height + description.height + rightButton.height + 75 : 0
                    visible: model.category === "definitions_cmd" && model.activated
                    
                    title.text: model.title
                    description.text: model.text
                    propertyUrl: model.link

                    link {
                        propertyText.text: "<a href='" + definitions_cmd.propertyUrl + "'>" + qsTranslate("PageServicesSign","STR_SIGN_CMD_URL")
                    }

                    rightButton {
                        text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_BUTTON")
                        onClicked: setCmdSettings()
                    }
                }

                Components.Notification {
                    id: update
                    height: visible ? title.height + description.height + rightButton.height + 75 : 0
                    visible: model.category === "update" && model.activated

                    title.text: model.title
                    description.text: model.text

                    rightButton {
                        text: qsTranslate("main", "STR_UPDATE_INSTALL_BUTTON")
                        onClicked: goToUpdate(model.update_type, model.release_notes, model.installed_version, model.remote_version, model.url_list)
                    }
                }
            }
        }

        Label {
            id: title
            text: qsTranslate("main", "STR_NOTIFICATION_CENTER") + controler.autoTr
            elide: Label.ElideRight
            wrapMode: Text.WordWrap
            lineHeight: 1.2
            color: Constants.COLOR_MAIN_BLUE

            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name

            anchors.top: parent.top
            anchors.topMargin: Constants.MARGIN_NOTIFICATION_CENTER
            anchors.left: parent.left
            anchors.leftMargin: Constants.MARGIN_NOTIFICATION_CENTER
        }

        Image {
            id: exitIcon
            width: Constants.SIZE_IMAGE_BOTTOM_MENU 
            height: Constants.SIZE_IMAGE_BOTTOM_MENU 
            fillMode: Image.PreserveAspectFit
            source: "../images/titleBar/quit_hover.png"

            anchors.top: title.top
            anchors.right: notificationArea.right
        }

        MouseArea {
            id: exitArea
            anchors.fill: exitIcon
            hoverEnabled: true
            onClicked: close()
        } 

        Flickable {
            id: notificationArea
            width: parent.width - Constants.MARGIN_NOTIFICATION_CENTER * 2
            height: parent.height - title.height - 30
            contentHeight: listView_read.height + listView.height + new_notification_title.height + read_notification_title.height + 50
            clip: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: title.bottom
            anchors.topMargin: Constants.MARGIN_NOTIFICATION_CENTER 

            ScrollBar.vertical: ScrollBar {
                id: viewScroll
                active: true
                visible: false
                policy: ScrollBar.AlwaysOn
            }

            Text {
                id: new_notification_title
                text: qsTranslate("main", "STR_NOTIFICATION_RECENT") + controler.autoTr
                visible: listView.model.count > 0
                color: Constants.COLOR_GRAY
                
                anchors.top: parent.top

                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
            }

            ListView {
                id: listView
                width: parent.width
                height: childrenRect.height 
                clip: true
                model: model_recent
                delegate: delegate
                focus: true
                spacing: 6
                interactive: false

                anchors.top: new_notification_title.bottom
                anchors.topMargin: Constants.MARGIN_NOTIFICATION_CENTER / 2
            }

            Text {
                id: read_notification_title
                text: qsTranslate("main", "STR_NOTIFICATION_READ") + controler.autoTr
                visible: listView_read.model.count > 0
                color: Constants.COLOR_GRAY
                
                anchors.top: listView.model.count > 0 ? listView.bottom : parent.top
                anchors.topMargin: listView.model.count > 0 ? Constants.MARGIN_NOTIFICATION_CENTER * 2 : 0

                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
            }

            ListView {
                id: listView_read
                width: listView.width
                height: childrenRect.height 
                clip: true
                model: model_read
                delegate: delegate
                focus: true
                spacing: 6
                interactive: false

                anchors.top: read_notification_title.bottom
                anchors.topMargin: Constants.MARGIN_NOTIFICATION_CENTER / 2
            }
        }

        onOpened: {
            console.log("Notification center opened")
            notificationMenuPopup.forceActiveFocus()
        }
    }

    function open() {
        notificationMenuPopup.open()
    }

    function close() {
        notificationMenuPopup.close()
    }

    function openNotification(id, read, activated) {

        var model = read ? model_read.get(id) : model_recent.get(id)
        
        if (model.mandatory) {
            return;
        }

        model.activated = !model.activated

        if (activated && !read) {
            model.read = true
            insertInModel(model.priority, model)
            listView.model.remove(id)
        }
        else {
            switch (model.category) {
                case "news":
                    controler.updateNewsLog(model.id)
                    break
                case "definitions_cmd":
                    controler.setAskToRegisterCmdCertValue(false)
                    break
                default:
            }
        }
    }

    function addAppNews(news_list) {
        for (var index in news_list) {
            insertInModel(0, {
                "id": news_list[index]["id"],
                "title": news_list[index]["title"],
                "text": news_list[index]["text"],
                "link": news_list[index]["link"],
                "read": news_list[index]["read"],
                "category": "news",
                "priority": 0,
                "activated": false,
                "mandatory": false
            })

            if (!news_list[index]["read"] && !reload) {
                mainFormID.propertyNotificationMenu.open()
            }
        }
    }

    function addCmdSettings(read) {
        insertInModel(2, {
            "title": qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_TITLE") + controler.autoTr,
            "text": qsTranslate("DialogCMD", "STR_REGISTER_CMD_CERT_DESC") + controler.autoTr,
            "link": "https://www.autenticacao.gov.pt/cmd-pedido-chave",
            "category": "definitions_cmd",
            "read": read,
            "priority": 2,
            "activated": false,
            "mandatory": false
        })

        if (!read && !reload) {
            mainFormID.propertyNotificationMenu.open()
        }
    }   

    function setCmdSettings() {
        mainFormID.propertyCmdDialog.open(GAPI.RegisterCert)  
    }   

    function addCacheSettings() {
        insertInModel(3, {
            "title": qsTranslate("main","STR_SET_CACHE_TITLE"),
            "text": qsTranslate("main","STR_SET_CACHE_TEXT") + "<br></br><br></br><b>" + qsTranslate("main","STR_SET_CACHE_TEXT_MANDATORY") + "</b>",
            "link": "",
            "category": "definitions_cache",
            "read": false,
            "priority": 3,
            "activated": true,
            "mandatory": true
        })

        hasMandatory = true
        exitIcon.visible = false
        exitArea.enabled = false
        notificationArea.interactive = false
        mainFormID.propertyNotificationMenu.open()
    }

    function setCacheSettings(index, model, activatedCache) {
        model.mandatory = false
        hasMandatory = false
        exitIcon.visible = true
        exitArea.enabled = true
        notificationArea.interactive = true

        openNotification(index, model.read, model.activated)
        controler.setAskToSetCacheValue(false)
        
        if (activatedCache) {
            controler.setEnablePteidCache(true);
        }
        else {
            controler.setEnablePteidCache(false);
            controler.flushCache();
        }     
    }

    function addUpdate(release_notes, installed_version, remote_version, url_list, type) {       
        var title = ""
        var text = ""

        if (type == 1) {
            title = qsTranslate("main", "STR_AUTOUPDATE_APP_TITLE")
            text = qsTranslate("PageDefinitionsUpdates", "STR_AUTOUPDATE_TEXT")
        }
        else {
            title = qsTranslate("main", "STR_AUTOUPDATE_CERT_TITLE")
            text = qsTranslate("PageDefinitionsUpdates", "STR_AUTOUPDATE_CERTS_TEXT")
        }
        
        insertInModel(1, {
            "title": title,
            "text": text + "<br><br>" 
                  + qsTranslate("PageDefinitionsUpdates", "STR_AUTOUPDATE_OPEN_TEXT")  + "<br><br>"
                  + qsTranslate("PageDefinitionsUpdates", "STR_DISABLE_AUTOUPDATE_INFO"),
            "link": "",
            "category": "update",
            "read": false,
            "release_notes": release_notes,
            "installed_version": installed_version,
            "remote_version": remote_version,
            "url_list": url_list,
            "update_type": type,
            "priority": 1,
            "activated": false,
            "mandatory": false
        })

        if (!reload) {
            mainFormID.propertyNotificationMenu.open()
        }
    }

    function goToUpdate(type, release_notes, installed_version, remote_version, certs_list) {

        if (controler.getAskToSetCacheValue()) {
            return 
        }

        mainMenuBottomPressed(0)
        mainFormID.propertyPageLoader.source = "../" + 
                mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(5).url
        mainFormID.propertySubMenuListView.currentIndex = 5
        mainFormID.propertySubMenuListView.forceActiveFocus()
        mainFormID.propertyNotificationMenu.close() 
        console.log("Source" + mainFormID.propertyPageLoader.source)

        //re-emit signal for PageDefinitionsUpdatesForm
        if(type == GAPI.AutoUpdateApp){
            controler.signalAutoUpdateAvailable(GAPI.AutoUpdateApp,
                                                release_notes,
                                                installed_version,
                                                remote_version,
                                                "")
        }
        if(type == GAPI.AutoUpdateCerts){
            controler.signalAutoUpdateAvailable(GAPI.AutoUpdateCerts,
                                                "",
                                                "",
                                                "",
                                                certs_list)
        }
    }
    
    function insertInModel(priority, item) {
        var index = 0
        var model = item.read ? model_read : model_recent

        while (index < model.count && priority <= model.get(index).priority) {
            index++
        }

        if (index == model.count) {   
            model.append(item)
        }
        else {
            model.insert(index, item)
        }
    }

    function chooseIcon(category, read) {
        switch (category) {
            case "news":
                if (read)
                    return "../images/news_icon.png" 
                else  
                    return "../images/news_icon_selected.png"
            case "update":
                if (read)
                    return "../images/update_icon.png"
                else  
                    return "../images/update_icon_selected.png"
            case "definitions_cmd":
                if (read)
                    return "../images/services_icon.png"
                else  
                    return "../images/services_icon_selected.png"
            case "definitions_cache":
                if (read)
                    return "../images/definitions_icon.png"
                else  
                    return "../images/definitions_icon_selected_notification.png"
            default:
                return null
        }
    }

    function chooseCategory(category) {
        switch (category) {
            case "news":
                return qsTranslate("main", "STR_NOTIFICATION_NEWS") + controler.autoTr
            case "update":
                return qsTranslate("main", "STR_NOTIFICATION_UPDATE") + controler.autoTr
            case "definitions_cmd":
                return qsTranslate("main", "STR_NOTIFICATION_SERVICES") + controler.autoTr
            case "definitions_cache":
                return qsTranslate("main", "STR_NOTIFICATION_CONFIG") + controler.autoTr
            default:
                return null
        }
    }

    function clearModels() {
        reload = true
        while (model_recent.count > 0 && model_recent.get(0).priority > 0) {
            model_recent.remove(0)
        }
        while (model_read.count > 0 && model_read.get(0).priority > 0) {
            model_read.remove(0)
        }
    }
}

