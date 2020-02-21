/*-****************************************************************************

 * Copyright (C) 2017 - 2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

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
            console.log("onSignalAutoUpdateFail")
            var tempTextDescription = ""
            if (error_code == GAPI.GenericError) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_ERROR")
                        + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_CONTACT_SUPPORT")
            }else if (error_code == GAPI.NoUpdatesAvailable) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NO_UPDATES")
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
            }else if (error_code == GAPI.InstallFailed) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_INSTALL_FAIL")
                        + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_CONTACT_SUPPORT")
            }else if (error_code == GAPI.NetworkError) {
                tempTextDescription =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NETWORK_ERROR")
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
            } else {
                propertyTextDescriptionCerts.text = tempTextDescription
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
            var titlePopup = qsTranslate("PageDefinitionsUpdates","STR_UPDATED_CERTIFICATES")
            var bodyPopup = qsTranslate("Popup Card","STR_POPUP_RESTART_APP")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalAutoUpdateAvailable: {
            console.log("PageDefinitionsUpdates onSignalAutoUpdateAvailable")

            if(updateType == GAPI.AutoUpdateApp){
                //make sure we have something to display
                if (release_notes != ""){
                    propertyReleaseNoteScrollView.visible = true
                    propertyReleaseScrollViewText.text = release_notes
                    propertyReleaseScrollViewText.Accessible.name =
                            Functions.filterText(propertyReleaseScrollViewText.text)
                    propertyReleaseScrollViewText.visible = true
                    propertyTextDescription.text = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_AVAILABLE")
                    propertyInstalledVersion.propertyDateField.text = installed_version
                    propertyRemoteVersion.propertyDateField.text = remote_version

                    propertyInstalledVersion.visible = true
                    propertyRemoteVersion.visible = true

                    propertyButtonSearch.visible = false
                    propertyButtonStartUpdate.text =
                            qsTranslate("PageDefinitionsUpdates","STR_UPDATE_BUTTON_START") + ' ' +
                            qsTranslate("PageDefinitionsUpdates","STR_VERSION") + ' ' + remote_version
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
            } else {
                if (certs_list != ""){
                    propertyReleaseNoteScrollViewCerts.visible = true
                    propertyReleaseScrollViewTextCerts.text = certs_list
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
                            + controler.getAppCertsUpdate()
                            + ". " + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
                    propertyButtonSearchCerts.visible = true
                }
                propertyProgressBarCerts.visible = false
                propertyProgressBarCerts.value = 0
                propertyProgressBarCerts.indeterminate = false
                propertyTextDescriptionTextCerts.forceActiveFocus()
            }
        }
        onSignalAutoUpdateNotAvailable: {
            console.log("PageDefinitionsUpdates onSignalAutoUpdateNotAvailable")
            propertyTextDescriptionCerts.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_CERTS_NO_UPDATES")
                    + " " +qsTranslate("PageHelpAbout","STR_HELP_APP_CERTS_UPDATE") + " : "
                    + controler.getAppCertsUpdate()
                    + ". " + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
            propertyButtonSearchCerts.visible = true
            propertyProgressBarCerts.visible = false
            propertyProgressBarCerts.value = 0
            propertyProgressBarCerts.indeterminate = false
            propertyTextDescriptionTextCerts.forceActiveFocus()
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
            propertyProgressBar.visible = true
            propertyProgressBar.indeterminate = true
            propertyButtonSearch.visible = false
            propertyButtonStartUpdate.visible = false //instant hide it after press guarantees that user never accidentally clicks two times
            controler.startUpdateApp()
        }
    }
    propertyButtonCancelUpdate {
        onClicked: {
            console.log("propertyButtonCancelUpdate clicked")
            propertyProgressBar.visible = false
            propertyButtonSearch.visible = true
            controler.userCancelledUpdateDownload()
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


    Component.onCompleted: {
        console.log("Page definitionsUpdate onCompleted")
        propertyMainItem.forceActiveFocus()
        console.log("Page definitionsUpdate onCompleted finished")
        propertyTextDescriptionCerts.text =
                qsTranslate("PageHelpAbout","STR_HELP_APP_CERTS_UPDATE") + " : "
                + controler.getAppCertsUpdate()
                + ". "
                + "<br><br>" + qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
    }
}
