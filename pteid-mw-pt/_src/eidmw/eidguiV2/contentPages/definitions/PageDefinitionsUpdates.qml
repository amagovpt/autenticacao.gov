/*-****************************************************************************

 * Copyright (C) 2017 - 2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsUpdatesForm {
    Keys.onPressed: {
        console.log("PageDefinitionsUpdatesForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Connections {
        target: controler
        onSignalAutoUpdateFail: {
            console.log("onSignalAutoUpdateFail updateType: " + updateType + ", error_code: " + error_code)
            var tempTextDescription = ""
            if (error_code == GAPI.GenericError) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_ERROR")
                        + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_CONTACT_SUPPORT")
            }else if (error_code == GAPI.NoUpdatesAvailable) {
                if(updateType == GAPI.AutoUpdateApp){
                    tempTextDescription =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NO_UPDATES")
                } else {
                    tempTextDescription =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_NO_UPDATES")
                }
            }else if (error_code == GAPI.LinuxNotSupported) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_LINUX_NOT_SUPPORTED")
            }else if (error_code == GAPI.UnableSaveFile) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_SAVE_FILE")
            }else if (error_code == GAPI.DownloadFailed) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_DOWNLOAD_FAIL")
            }else if (error_code == GAPI.DownloadCancelled) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
                        + "<a href='" + propertyTextDescriptionText.propertyLinkUrl + "'>"
                        + qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TEXT_LINK") + "</a>."
            }else if (error_code == GAPI.InstallFailed) {
                if (updateType == GAPI.AutoUpdateApp) {
                    tempTextDescription =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_INSTALL_FAIL")
                }
                else {
                    tempTextDescription =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_INSTALL_CERTS_FAIL")
                }
                tempTextDescription +=
                        "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_CONTACT_SUPPORT")
            }else if (error_code == GAPI.NetworkError) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NETWORK_ERROR")
                if (controler.isProxyConfigured()) {
                    tempTextDescription += " " 
                        + qsTranslate("GAPI","STR_VERIFY_PROXY")
                }
            }

            if(updateType == GAPI.AutoUpdateApp){
                propertyTextDescription.text = tempTextDescription
                propertyProgressBar.visible = false
                propertyProgressBar.value = 0
                propertyProgressBar.indeterminate = false
                propertyButtonSearch.visible = true
                propertyButtonStartUpdate.visible = false
                propertyButtonCancelUpdate.visible = false
                propertyReleaseNoteScrollView.visible = false
                propertyReleaseScrollViewText.visible = false
                propertyInstalledVersion.visible = false
                propertyRemoteVersion.visible = false
                propertyTextDescriptionText.forceActiveFocus()
            } else if(updateType == GAPI.AutoUpdateCerts){
                propertyTextDescriptionCerts.text = tempTextDescription
                        +  " " +qsTranslate("PageHelpAbout","STR_HELP_APP_CERTS_UPDATE") + " : "
                        + controler.getCertsLog()
                propertyProgressBarCerts.visible = false
                propertyProgressBarCerts.value = 0
                propertyProgressBarCerts.indeterminate = false
                propertyButtonSearchCerts.visible = true
                propertyButtonStartUpdateCerts.visible = false
                propertyButtonCancelUpdateCerts.visible = false
                propertyReleaseNoteScrollViewCerts.visible = false
                propertyReleaseScrollViewTextCerts.visible = false
                propertyTextDescriptionTextCerts.forceActiveFocus()
            }
        }
        onSignalAutoUpdateProgress: {
            if(updateType == GAPI.AutoUpdateApp){
                propertyProgressBar.indeterminate = true
                propertyProgressBar.visible = true
                //propertyProgressBar.value = value
                if (propertyButtonCancelUpdate.visible == false) {
                    propertyButtonCancelUpdate.visible = true
                }
            } else {
                propertyProgressBarCerts.indeterminate = true
                propertyProgressBarCerts.visible = true
                //propertyProgressBarCerts.value = value
                if (propertyButtonCancelUpdateCerts.visible == false) {
                    propertyButtonCancelUpdateCerts.visible = true
                }
            }
        }
        onSignalAutoUpdateSuccess: {
            if(updateType == GAPI.AutoUpdateApp){
                propertyProgressBar.visible = false
                propertyProgressBar.value = 0
                propertyProgressBar.indeterminate = false
                propertyButtonCancelUpdate.visible = false
                if (Qt.platform.os === "linux") {
                    propertyTextDescriptionText.propertyLinkUrl = "https://amagovpt.github.io/docs.autenticacao.gov/user_manual.html#atualização-da-aplicação"
                    propertyTextDescription.text = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TRANSFER_DONE")
                        + "<a href='" + propertyTextDescriptionText.propertyLinkUrl + "'>"
                        + qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TEXT_LINK") + "</a>."
                    propertyTextDescriptionText.forceActiveFocus()
                    propertyButtonOpenTransferFolder.visible = true
                    propertyButtonOpenTransferFolder.forceActiveFocus()
                }
            } else {
                propertyTextDescriptionCerts.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_SUCCESS")
                    + "<br><br>" + qsTranslate("Popup Card","STR_POPUP_RESTART_APP")
                propertyProgressBarCerts.visible = false
                propertyProgressBarCerts.value = 0
                propertyProgressBarCerts.indeterminate = false
                propertyButtonSearchCerts.visible = true
                propertyButtonStartUpdateCerts.visible = false
                propertyButtonCancelUpdateCerts.visible = false
                propertyReleaseNoteScrollViewCerts.visible = false
                propertyReleaseScrollViewTextCerts.visible = false
                restart_dialog.headerTitle = qsTranslate("PageDefinitionsUpdates","STR_UPDATED_CERTIFICATES") + controler.autoTr
                restart_dialog.open()
            }
        }
        onSignalAutoUpdateAvailable: {
            console.log("PageDefinitionsUpdates onSignalAutoUpdateAvailable")

            if(updateType == GAPI.AutoUpdateApp){
                //make sure we have something to display
                if (arg1 != ""){
                    propertyReleaseNoteScrollView.visible = true
                    propertyReleaseScrollViewText.text = arg1
                    propertyReleaseScrollViewText.Accessible.name =
                            Functions.filterText(propertyReleaseScrollViewText.text)
                    propertyReleaseScrollViewText.visible = true
                    propertyTextDescription.text = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_AVAILABLE")
                        + "<a href='" + propertyTextDescriptionText.propertyLinkUrl + "'>"
                        + qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TEXT_LINK") + "</a>."
                    propertyInstalledVersion.propertyDateField.text = arg2
                    propertyRemoteVersion.propertyDateField.text = arg3

                    propertyInstalledVersion.visible = true
                    propertyRemoteVersion.visible = true

                    propertyButtonSearch.visible = false
                    propertyButtonStartUpdate.text =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_BUTTON_START") + ' ' +
                            qsTranslate("PageDefinitionsUpdates","STR_VERSION") + ' ' + arg3
                    propertyButtonStartUpdate.visible = true

                } else {
                    propertyTextDescription.text =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NO_UPDATES")
                    propertyButtonSearch.visible = true
                }
                propertyProgressBar.visible = false
                propertyProgressBar.value = 0
                propertyProgressBar.indeterminate = false
                propertyTextDescriptionText.forceActiveFocus()
            } else if(updateType == GAPI.AutoUpdateCerts){
                if (url_list != ""){
                    propertyReleaseNoteScrollViewCerts.visible = true
                    propertyReleaseScrollViewTextCerts.text = url_list
                    propertyReleaseScrollViewTextCerts.Accessible.name =
                            Functions.filterText(propertyReleaseScrollViewText.text)
                    propertyReleaseScrollViewTextCerts.visible = true
                    propertyTextDescriptionCerts.text = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_AVAILABLE")

                    propertyButtonSearchCerts.visible = false
                    propertyButtonStartUpdateCerts.text =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_BUTTON_START")
                    propertyButtonStartUpdateCerts.visible = true

                } else {
                    propertyTextDescriptionCerts.text =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_NO_UPDATES")
                            + " " + qsTranslate("PageHelpAbout","STR_HELP_APP_CERTS_UPDATE") + " : "
                            + controler.getCertsLog()
                            + ". " + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_TEXT")
                    propertyButtonSearchCerts.visible = true
                }
                propertyProgressBarCerts.visible = false
                propertyProgressBarCerts.value = 0
                propertyProgressBarCerts.indeterminate = false
                propertyTextDescriptionTextCerts.forceActiveFocus()
            }
        }
        onSignalStartUpdate: {
            console.log("onSignalStartUpdate updateType = " + updateType)
            if(updateType == GAPI.AutoUpdateApp){
                propertyReleaseNoteScrollView.visible = false
                propertyReleaseScrollViewText.visible = false
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_STARTING_DOWNLOAD") + " " + filename
                propertyInstalledVersion.visible = false
                propertyRemoteVersion.visible = false
            } else{
                propertyReleaseNoteScrollViewCerts.visible = false
                propertyReleaseScrollViewTextCerts.visible = false
                propertyTextDescriptionCerts.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_STARTING_DOWNLOAD") + " " + filename
            }
        }
    }

    propertyButtonSearch {
        onClicked: {
            console.log("propertyButtonSearch clicked")
            propertyTextDescription.forceActiveFocus()
            propertyButtonStartUpdate.visible = false
            propertyProgressBar.indeterminate = true
            propertyProgressBar.visible = true
            propertyButtonSearch.visible = false
            propertyTextDescription.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_VERSION_LOOKUP")
            controler.autoUpdateApp()
        }
    }
    propertyButtonStartUpdate {
        onClicked: {
            console.log("propertyButtonStartUpdate clicked")
            propertyTextDescription.forceActiveFocus()
            propertySupportedSystems.visible = false
            propertyProgressBar.visible = true
            propertyProgressBar.indeterminate = true
            propertyButtonSearch.visible = false
            propertyButtonStartUpdate.visible = false //instant hide it after press guarantees that user never accidentally clicks two times
            controler.startUpdateApp()
        }
    }
    propertyButtonCancelUpdate {
        onClicked: {
            console.log("propertyButtonCancelAppUpdate clicked")
            propertySupportedSystems.visible = true
            propertyProgressBar.visible = false
            propertyButtonSearch.visible = true
            controler.userCancelledUpdateAppDownload()
            propertyTextDescription.forceActiveFocus()
        }
    }

    propertyButtonSearchCerts {
        onClicked: {
            console.log("propertyButtonSearchCerts clicked")
            propertyTextDescriptionCerts.forceActiveFocus()
            propertyButtonStartUpdateCerts.visible = false
            propertyProgressBarCerts.indeterminate = true
            propertyProgressBarCerts.visible = true
            propertyButtonSearchCerts.visible = false
            propertyTextDescriptionCerts.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_VERSION_LOOKUP")
            controler.autoUpdatesCerts()
        }
    }
    propertyButtonStartUpdateCerts {
        onClicked: {
            console.log("propertyButtonStartUpdateCerts clicked")
            propertyTextDescriptionCerts.forceActiveFocus()
            propertyProgressBarCerts.visible = true
            propertyProgressBarCerts.indeterminate = true
            propertyButtonSearchCerts.visible = false
            propertyButtonStartUpdateCerts.visible = false //instant hide it after press guarantees that user never accidentally clicks two times
            controler.startUpdateCerts()
        }
    }
    propertyButtonCancelUpdateCerts {
        onClicked: {
            console.log("propertyButtonCancelUpdateCerts clicked")
            propertyProgressBarCerts.visible = false
            propertyButtonSearchCerts.visible = true
            controler.userCancelledUpdateCertsDownload()
            propertyTextDescriptionCerts.forceActiveFocus()
        }
    }
    propertyButtonOpenTransferFolder {
        onClicked: {
            console.log("propertyButtonOpenTransferFolder clicked")
            controler.openTransfersFolder();
        }
    }


    Component.onCompleted: {
        console.log("Page definitionsUpdate onCompleted")
        propertyMainItem.forceActiveFocus()
        console.log("Page definitionsUpdate onCompleted finished")
        propertyTextDescriptionCerts.text =
                qsTranslate("PageHelpAbout","STR_HELP_APP_CERTS_UPDATE") + " : "
                + controler.getCertsLog()
                + ". "
                + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_TEXT")
    }
}
