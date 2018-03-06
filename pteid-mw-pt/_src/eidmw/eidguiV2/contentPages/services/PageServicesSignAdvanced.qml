import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignAdvancedForm {

    property bool isAnimationFinished: mainFormID.propertyPageLoader.propertyAnimationExtendedFinished
    property string propertyOutputSignedFile : ""

    ToolTip {
        id: controlToolTip
        timeout: Constants.TOOLTIP_TIMEOUT_MS
        contentItem: Text {
            text: controlToolTip.text
            font: controlToolTip.font
            color: Constants.COLOR_MAIN_PRETO
        }
        background: Rectangle {
            border.color: Constants.COLOR_MAIN_DARK_GRAY
            color: Constants.COLOR_MAIN_SOFT_GRAY
        }
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalEntityAttributesLoadedError: {
            console.log("Definitions SCAP - Signal SCAP entities loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    "Error"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    "SCAP entities loaded error"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            if(propertyBar.currentIndex == 0)
                propertyBusyIndicator.running = false
        }
        onSignalCompanyAttributesLoadedError: {
            console.log("Definitions SCAP - Signal SCAP company loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    "Error"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    "SCAP company loaded error"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            if(propertyBar.currentIndex == 1)
                propertyBusyIndicator.running = false
        }
        onSignalAttributesLoaded:{
            console.log("Definitions SCAP - Signal SCAP attributes loaded")

            for(var i = 0; i < attribute_list.length; i=i+2)
            {
                entityAttributesModel.append({
                                                  entityName: attribute_list[i], attribute: attribute_list[i+1], checkBoxAttr: false
                                              });
            }

            for (var i = 0; i < propertyPageLoader.attributeListBackup.length; i++){
                console.log(propertyPageLoader.attributeListBackup[i])
                entityAttributesModel.get(propertyPageLoader.attributeListBackup[i]).checkBoxAttr = true
            }

            propertyBusyIndicator.running = false
        }
        onSignalOpenCMDSucess: {
            console.log("Sign Advanced - Signal Open CMD Sucess")
            progressBarIndeterminate.visible = false
            rectReturnCode.visible = true
            buttonCMDProgressConfirm.visible = true
        }
        onSignalCloseCMDSucess: {
            console.log("Sign Advanced - Signal Close CMD Sucess")
            progressBarIndeterminate.visible = false
            rectLabelCMDText.visible = true
            buttonCMDProgressConfirm.visible = true
            buttonCMDProgressConfirm.text = qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
        }
        onSignalPdfSignSucess: {
            mainFormID.opacity = 0.5
            signsuccess_dialog.visible = true
            propertyBusyIndicator.running = false
            // test time stamp
            if(error_code == GAPI.SignMessageTimestampFailed){
                if(propertyListViewFiles.count > 1){
                    signsuccess_dialog.propertySignSuccessDialogText.text =
                            qsTranslate("PageServicesSign","STR_TIME_STAMP_MULTI_FAILED")
                }else{
                    signsuccess_dialog.propertySignSuccessDialogText.text =
                            qsTranslate("PageServicesSign","STR_TIME_STAMP_FAILED")
                }
            }else{ // Sign with time stamp succefull
                signsuccess_dialog.propertySignSuccessDialogText.text = ""
            }
        }
        onSignalPdfSignFail: {
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalCardAccessError: {
            console.log("Sign Advanced Page onSignalCardAccessError")
            if(cardLoaded && error_code != GAPI.CardUserPinCancel){
                if (error_code == GAPI.NoReaderFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                }
                else if (error_code == GAPI.NoCardFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                }
                else if (error_code == GAPI.SodCardReadError) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                }
                else {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                cardLoaded = false
            }
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Advanced --> Data Changed")
            //console.trace();
            propertyPDFPreview.propertyDragSigSignedByNameText.text =
                    qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Givenname)
                    + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)

            propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertyBusyIndicator.running = false
            cardLoaded = true
        }
        onSignalCardChanged: {
            console.log("Services Sign Advanced onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyPDFPreview.propertyDragSigSignedByNameText.text =
                        qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                propertyPDFPreview.propertyDragSigNumIdText.text =  qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                         qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                cardLoaded = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                cardLoaded = false
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
        onSignalUpdateProgressStatus: {
            console.log("CMD sign change --> update progress status with text = " + statusMessage)
            textMessageTop.text = statusMessage
        }
        onSignalUpdateProgressBar: {
            console.log("CMD sign change --> update progress bar with value = " + value)
            progressBar.value = value
            if(value === 100) {
                progressBarIndeterminate.visible = false
            }
        }
    }
    Connections {
        target: image_provider_pdf
        onSignalPdfSourceChanged: {
            console.log("Receive signal onSignalPdfSourceChanged pdfWidth = "+pdfWidth+" pdfHeight = "+pdfHeight);
            propertyPDFPreview.propertyPdfOriginalWidth=pdfWidth
            propertyPDFPreview.propertyPdfOriginalHeight=pdfHeight
            propertyPDFPreview.setSignPreview(propertyPageLoader.propertyBackupCoordX,propertyPageLoader.propertyBackupCoordY)
        }
    }

    Dialog {
        id: dialogSignCMD
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogSignCMD.width * 0.5
        y: parent.height * 0.5 - dialogSignCMD.height * 0.5

        header: Label {
            id: labelTextTitle
            text: qsTranslate("PageServicesSign","STR_SIGN_CMD")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectMessageTopLogin.height + rectMobilNumber.height + rectPin.height

            Item {
                id: rectMessageTopLogin
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTopLogin
                    text: qsTranslate("PageServicesSign","STR_SIGN_INSERT_LOGIN")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
            Item {
                id: rectMobilNumber
                width: parent.width
                height: 50
                anchors.top: rectMessageTopLogin.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text: qsTranslate("PageServicesSign","STR_SIGN_CMD_MOVEL_NUM")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width * 0.3
                    anchors.bottom: parent.bottom
                }
                ComboBox {
                    id: comboBoxIndicative
                    width: parent.width * 0.4
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    model: ["+351 - Portugal","+55 - Brazil","+34 - Spain","-------------------","+93 - Afghanistan",
                        "+355 - Albania","+213 - Algeria","+684 - American Samoa","+376 - Andorra","+244 - Angola",
                        "+809 - Anguilla","+268 - Antigua","+54 - Argentina","+374 - Armenia","+297 - Aruba",
                        "+247 - Ascension Island","+61 - Australia","+672 - Australian External Territories",
                        "+43 - Austria","+994 - Azerbaijan","+242 - Bahamas","+246 - Barbados","+973 - Bahrain",
                        "+880 - Bangladesh","+375 - Belarus","+32 - Belgium","+501 - Belize","+229 - Benin","+809 - Bermuda",
                        "+975 - Bhutan","+284 - British Virgin Islands","+591 - Bolivia","+387 - Bosnia and Hercegovina",
                        "+267 - Botswana","+55 - Brazil","+284 - British V.I.","+673 - Brunei Darussalm","+359 - Bulgaria",
                        "+226 - Burkina Faso","+257 - Burundi","+855 - Cambodia","+237 - Cameroon","+1 - Canada",
                        "+238 - CapeVerde Islands","+1 - Caribbean Nations","+345 - Cayman Islands","+238 - Cape Verdi",
                        "+236 - Central African Republic","+235 - Chad","+56 - Chile","+86 - China (People's Republic)",
                        "+886 - China-Taiwan","+57 - Colombia","+269 - Comoros and Mayotte","+242 - Congo",
                        "+506 - Costa Rica","+385 - Croatia","+53 - Cuba","+357 - Cyprus","+420 - Czech Republic",
                        "+45 - Denmark","+246 - Diego Garcia","+767 - Dominca","+809 - Dominican Republic","+253 - Djibouti",
                        "+593 - Ecuador","+20 - Egypt","+503 - El Salvador","+240 - Equatorial Guinea",
                        "+291 - Eritrea","+372 - Estonia","+251 - Ethiopia","+500 - Falkland Islands",
                        "+298 - Faroe (Faeroe) Islands (Denmark)","+679 - Fiji","+358 - Finland","+33 - France",
                        "+596 - French Antilles","+594 - French Guiana","+241 - Gabon (Gabonese Republic)","+220 - Gambia",
                        "+995 - Georgia","+49 - Germany","+233 - Ghana","+350 - Gibraltar","+30 - Greece","+299 - Greenland",
                        "+473 - Grenada/Carricou","+671 - Guam","+502 - Guatemala","+224 - Guinea","+245 - Guinea-Bissau",
                        "+592 - Guyana","+509 - Haiti","+504 - Honduras","+852 - Hong Kong","+36 - Hungary","+354 - Iceland",
                        "+91 - India","+62 - Indonesia","+98 - Iran","+964 - Iraq","+353 - Ireland (Irish Republic; Eire)",
                        "+972 - Israel","+39 - Italy","+225 - Ivory Coast (La Cote d'Ivoire)","+876 - Jamaica","+81 - Japan",
                        "+962 - Jordan","+7 - Kazakhstan","+254 - Kenya","+855 - Khmer Republic (Cambodia/Kampuchea)",
                        "+686 - Kiribati Republic (Gilbert Islands)","+82 - Korea, Republic of (South Korea)",
                        "+850 - Korea, People's Republic of (North Korea)","+965 - Kuwait","+996 - Kyrgyz Republic",
                        "+371 - Latvia","+856 - Laos","+961 - Lebanon","+266 - Lesotho","+231 - Liberia","+370 - Lithuania",
                        "+218 - Libya","+423 - Liechtenstein","+352 - Luxembourg","+853 - Macao","+389 - Macedonia",
                        "+261 - Madagascar","+265 - Malawi","+60 - Malaysia","+960 - Maldives","+223 - Mali","+356 - Malta",
                        "+692 - Marshall Islands","+596 - Martinique (French Antilles)","+222 - Mauritania",
                        "+230 - Mauritius","+269 - Mayolte","+52 - Mexico","+691 - Micronesia (F.S. of Polynesia)",
                        "+373 - Moldova","+33 - Monaco","+976 - Mongolia","+473 - Montserrat","+212 - Morocco",
                        "+258 - Mozambique","+95 - Myanmar (former Burma)","+264 - Namibia (former South-West Africa)",
                        "+674 - Nauru","+977 - Nepal","+31 - Netherlands","+599 - Netherlands Antilles","+869 - Nevis",
                        "+687 - New Caledonia","+64 - New Zealand","+505 - Nicaragua","+227 - Niger","+234 - Nigeria",
                        "+683 - Niue","+850 - North Korea","+1 670 - North Mariana Islands (Saipan)","+47 - Norway",
                        "+968 - Oman","+92 - Pakistan","+680 - Palau","+507 - Panama","+675 - Papua New Guinea",
                        "+595 - Paraguay","+51 - Peru","+63 - Philippines","+48 - Poland","+351 - Portugal (includes Azores)",
                        "+1 787 - Puerto Rico","+974 - Qatar","+262 - Reunion (France)","+40 - Romania","+7 - Russia",
                        "+250 - Rwanda (Rwandese Republic)","+670 - Saipan","+378 - San Marino","+239 - Sao Tome and Principe",
                        "+966 - Saudi Arabia","+221 - Senegal","+381 - Serbia and Montenegro","+248 - Seychelles",
                        "+232 - Sierra Leone","+65 - Singapore","+421 - Slovakia","+386 - Slovenia","+677 - Solomon Islands",
                        "+252 - Somalia","+27 - South Africa","+34 - Spain","+94 - Sri Lanka","+290 - St. Helena",
                        "+869 - St. Kitts/Nevis","+508 - St. Pierre &(et) Miquelon (France)","+249 - Sudan","+597 - Suriname",
                        "+268 - Swaziland","+46 - Sweden","+41 - Switzerland","+963 - Syrian Arab Republic (Syria)",
                        "+689 - Tahiti (French Polynesia)","+886 - Taiwan","+7 - Tajikistan","+255 - Tanzania (includes Zanzibar)",
                        "+66 - Thailand","+228 - Togo (Togolese Republic)","+690 - Tokelau","+676 - Tonga","+1 868 - Trinidad and Tobago",
                        "+216 - Tunisia","+90 - Turkey","+993 - Turkmenistan","+688 - Tuvalu (Ellice Islands)","+256 - Uganda",
                        "+380 - Ukraine","+971 - United Arab Emirates","+44 - United Kingdom","+598 - Uruguay","+1 - USA",
                        "+7 - Uzbekistan","+678 - Vanuatu (New Hebrides)","+39 - Vatican City","+58 - Venezuela","+84 - Viet Nam",
                        "+1 340 - Virgin Islands","+681 - Wallis and Futuna","+685 - Western Samoa",
                        "+381 - Yemen (People's Democratic Republic of)","+967 - Yemen Arab Republic (North Yemen)",
                        "+381 - Yugoslavia (discontinued)","+243 - Zaire","+260 - Zambia","+263 - Zimbabwe",
                    ]
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    visible: true
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldMobileNumber
                    width: parent.width * 0.25
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldMobileNumber.text === "" ? true: false
                    placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_MOVEL_NUM_OP") + "?"
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: comboBoxIndicative.right
                    anchors.leftMargin:  parent.width * 0.05
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPin
                width: parent.width
                height: 50
                anchors.top: rectMobilNumber.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinNew
                    text: qsTranslate("PageServicesSign","STR_SIGN_CMD_PIN")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width * 0.3
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPin
                    width: parent.width * 0.7
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPin.text === "" ? true: false
                    placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_PIN_OP") + "?"
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectPin.bottom
                Text {
                    id: textLinkCMD
                    textFormat: Text.RichText
                    text: "<a href=\"https://www.autenticacao.gov.pt/cmd-pedido-chave\">"
                          + qsTranslate("PageServicesSign","STR_SIGN_CMD_URL")
                    font.italic: true
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                }
            }
            Item {
                width: dialogSignCMD.availableWidth
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                y: 190
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        dialogSignCMD.close()
                        mainFormID.opacity = 1.0
                    }
                }
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    enabled: textFieldMobileNumber.length !== 0 && textFieldPin.length !== 0 ? true : false
                    onClicked: {
                        var loadedFilePath = filesModel.get(0).fileUrl
                        var isTimestamp = propertySwitchSignTemp.checked
                        var outputFile = propertyFileDialogCMDOutput.fileUrl.toString()
                        if (Qt.platform.os === "windows") {
                            outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                        }else{
                            outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                        }
                        outputFile = decodeURIComponent(outputFile)

                        var page = 1
                        propertyCheckLastPage.checked ? page = gapi.getPDFpageCount(loadedFilePath) :
                                                    page = propertySpinBoxControl.value
                        var reason = propertyTextFieldReason.text
                        var location = propertyTextFieldLocal.text
                        var isSmallSignature = propertyCheckSignReduced.checked
                        var coord_x = -1
                        var coord_y = -1
                        if(propertyCheckSignShow.checked){
                            coord_x = propertyPDFPreview.propertyCoordX
                            //coord_y must be the lower left corner of the signature rectangle
                            coord_y = propertyPDFPreview.propertyCoordY
                        }
                        console.log("Output filename: " + outputFile)
                        console.log("Signing in position coord_x: " + coord_x
                                    + " and coord_y: "+coord_y)

                        var indicatice = comboBoxIndicative.currentText.substring(0, comboBoxIndicative.currentText.indexOf(' '));
                        var mobileNumber = indicatice + " " + textFieldMobileNumber.text

                        propertyOutputSignedFile = outputFile
                        rectLabelCMDText.visible = false
                        gapi.signOpenCMD(mobileNumber,textFieldPin.text,
                                     loadedFilePath,outputFile,page,
                                     coord_x,coord_y,
                                     reason,location,
                                     isTimestamp, isSmallSignature)

                        progressBarIndeterminate.visible = true
                        progressBar.visible = true
                        textFieldMobileNumber.text = ""
                        textFieldPin.text = ""
                        dialogSignCMD.close()
                        buttonCMDProgressConfirm.visible = false
                        buttonCMDProgressConfirm.text = qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
                        dialogCMDProgress.open()
                    }
                }
            }
        }
    }

    Dialog {
        id: dialogCMDProgress
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogCMDProgress.width * 0.5
        y: parent.height * 0.5 - dialogCMDProgress.height * 0.5

        header: Label {
            id: labelConfirmOfAddressProgressTextTitle
            text: qsTranslate("PageServicesSign","STR_SIGN_CMD")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }
        ProgressBar {
            id: progressBar
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            height: 20
            to: 100
            value: 0
            visible: true
            indeterminate: false
            z:1
        }

        Item {
            width: parent.width
            height: rectMessageTop.height + rectReturnCode.height + progressBarIndeterminate.height
            anchors.top: progressBar.bottom

            Item {
                id: rectMessageTop
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
                    text: ""
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
            Item {
                id: rectLabelCMDText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectMessageTop.bottom
                visible: false
                Text {
                    id: labelCMDText
                    text: qsTranslate("PageServicesSign","STR_SIGN_OPEN")
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    wrapMode: Text.Wrap
                }
            }
            Item {
                id: rectReturnCode
                width: parent.width
                height: 50
                anchors.top: rectMessageTop.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
                Text {
                    id: textReturnCode
                    text: qsTranslate("PageServicesSign","STR_SIGN_CMD_CODE") + ":"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldReturnCode
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldReturnCode.text === "" ? true: false
                    placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_CODE_OP") + "?"
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textReturnCode.right
                    anchors.bottom: parent.bottom
                }
            }
            ProgressBar {
                id: progressBarIndeterminate
                width: parent.width
                anchors.top: rectReturnCode.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                to: 100
                value: 0
                visible: true
                indeterminate: true
                z:1
            }

        }

        Item {
            width: dialogCMDProgress.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.horizontalCenter: parent.horizontalCenter
            y: 190
            Button {
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CANCEL")
                anchors.left: parent.left
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: {
                    dialogCMDProgress.close()
                    rectReturnCode.visible = false
                    mainFormID.opacity = 1.0
                }
            }
            Button {
                id: buttonCMDProgressConfirm
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                visible: false
                onClicked: {
                    console.log("Send sms_token : " + textFieldReturnCode.text)
                    if( progressBar.value < 100){
                        gapi.signCloseCMD(textFieldReturnCode.text)
                        progressBarIndeterminate.visible = true
                        rectReturnCode.visible = false
                        buttonCMDProgressConfirm.visible = false
                        textFieldReturnCode.text = ""
                        dialogCMDProgress.open()
                    }else{
                        dialogCMDProgress.close()
                        if (Qt.platform.os === "windows") {
                            propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
                        }else{
                            propertyOutputSignedFile = "file://" + propertyOutputSignedFile
                        }
                        console.log("Open Url Externally: " + propertyOutputSignedFile)
                        Qt.openUrlExternally(propertyOutputSignedFile)
                        mainFormID.opacity = 1.0
                    }
                }
            }
        }
    }

    Dialog {
        id: signsuccess_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signsuccess_dialog.width * 0.5
        y: parent.height * 0.5 - signsuccess_dialog.height * 0.5
        property alias propertySignSuccessDialogText: labelText

        header: Label {
            text: qsTranslate("PageServicesSign","STR_SIGN_SUCESS_MULTI")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            width: signsuccess_dialog.availableWidth
            height: 50
            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                }
            }
            Item {
                id: rectLabelOpenText
                width: parent.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectLabelText.bottom
                Text {
                    id: labelOpenText
                    text: qsTranslate("PageServicesSign","STR_SIGN_OPEN_MULTI")
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
            width: signsuccess_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signsuccess_dialog.close()
                        mainFormID.opacity = 1.0
                    }
                }
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        if (Qt.platform.os === "windows") {
                            propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
                        }else{
                            propertyOutputSignedFile = "file://" + propertyOutputSignedFile
                        }
                        console.log("Open Url Externally: " + propertyOutputSignedFile)
                        Qt.openUrlExternally(propertyOutputSignedFile)
                        signsuccess_dialog.close()
                        mainFormID.opacity = 1.0
                    }
                }
            }
        }
    }

    Dialog {
        id: signerror_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signerror_dialog.width * 0.5
        y: parent.height * 0.5 - signerror_dialog.height * 0.5

        header: Label {
            text: qsTranslate("PageServicesSign","STR_SIGN_FAIL")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        standardButtons: DialogButtonBox.Ok
    }

    propertyMouseAreaToolTipPades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILES")
            controlToolTip.x = propertyMouseAreaToolTipPadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }
    }
    propertyMouseAreaToolTipXades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_SIGN_PACKAGE")
            controlToolTip.x = propertyMouseAreaToolTipXadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }
    }

    propertyDropArea {

        onEntered: {
            console.log("Signature advanced ! You chose file(s): " + drag.urls);
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            // Update sign preview position variables to be used to send to sdk
            propertyPDFPreview.updateSignPreview(drop.x,drop.y)
            propertyPageLoader.propertyBackupCoordX = propertyPDFPreview.propertyDragSigRect.x
            propertyPageLoader.propertyBackupCoordY = propertyPDFPreview.propertyDragSigRect.y

            for(var i = 0; i < filesArray.length; i++){
                console.log("Adding file: " + filesArray[i])
                var path =  filesArray[i]
                //  Get the path itself without a regex
                if (Qt.platform.os === "windows") {
                    path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                }else{
                    path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                }
				path = decodeURIComponent(path)
                filesModel.append({
                                      "fileUrl": path
                                  })
                mainFormID.propertyBackupfilesModel.append({
                                      "fileUrl": path
                                  })
            }
            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }
    propertyDropFileArea {
        onEntered: {
            console.log("Signature advanced Drop File Area! You chose file(s): " + drag.urls);
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            //TODO: Validate files type
            for(var i = 0; i < filesArray.length; i++){
                console.log("Adding file: " + filesArray[i])
                var path =  filesArray[i]
                //  Get the path itself without a regex
                if (Qt.platform.os === "windows") {
                    path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                }else{
                    path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                }
				path = decodeURIComponent(path)
                filesModel.append({
                                      "fileUrl": path
                                  })
                propertyPageLoader.propertyBackupfilesModel.append({
                                      "fileUrl": path
                                  })
            }
            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyFileDialogOutput {
        onAccepted: {
            propertyBusyIndicator.running = true
            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
            outputFile = decodeURIComponent(outputFile)
            if (propertyRadioButtonPADES.checked) {
                var page = propertySpinBoxControl.value
                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked

                var coord_x = -1
                var coord_y = -1
                if(propertyCheckSignShow.checked){
                    coord_x = propertyPDFPreview.propertyCoordX
                    //coord_y must be the lower left corner of the signature rectangle
                    coord_y = propertyPDFPreview.propertyCoordY
                }

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y + " page: " + page + " timestamp: " + isTimestamp)

                propertyOutputSignedFile = outputFile;
                if(propertySwitchSignAdd.checked){
                    coord_x = gapi.getPageSize(page).width * coord_x
                    coord_y = gapi.getPageSize(page).height * (1 - coord_y)

                    var attributeList = []
                    var count = 0
                    for (var i = 0; i < entityAttributesModel.count; i++){
                        if(entityAttributesModel.get(i).checkBoxAttr == true){
                            attributeList[count] = i
                            count++
                        }
                    }
                    console.log("QML AttributeList: ", attributeList)
                    gapi.startSigningSCAP(loadedFilePath, outputFile, page, coord_x, coord_y,
                                          0, attributeList)
                }else{

                    gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                         reason, location, isTimestamp, isSmallSignature)
                }
            }
            else {
                propertyOutputSignedFile = outputFile;
                propertyOutputSignedFile =
                        propertyOutputSignedFile.substring(0, propertyOutputSignedFile.lastIndexOf('/'))
                if (propertyListViewFiles.count == 1){
                    gapi.startSigningXADES(loadedFilePath, outputFile, isTimestamp)
                }else{
                    var batchFilesArray = []
                    for(var i = 0; i < propertyListViewFiles.count; i++){
                        batchFilesArray[i] =  propertyListViewFiles.model.get(i).fileUrl;
                    }
                    gapi.startSigningBatchXADES(batchFilesArray, outputFile, isTimestamp)
                }
            }
        }
    }
    propertyFileDialogCMDOutput {
        onAccepted: {
            mainFormID.opacity = 0.5
            dialogSignCMD.open()
        }
    }
    propertyFileDialogBatchOutput {
        onAccepted: {
            propertyBusyIndicator.running = true
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogBatchOutput.folder.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
            outputFile = decodeURIComponent(outputFile)
            if (propertyRadioButtonPADES.checked) {

                if(propertyCheckLastPage.checked){
                    var page = 0 // Sign last page in all documents
                }else{
                    var page = propertySpinBoxControl.value
                }

                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked
                var coord_x = -1
                var coord_y = -1
                if(propertyCheckSignShow.checked){
                    coord_x = propertyPDFPreview.propertyCoordX
                    //coord_y must be the lower left corner of the signature rectangle
                    coord_y = propertyPDFPreview.propertyCoordY
                }

                console.log("Output filename: " + outputFile)
                console.log("Signing Batch in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y + " page: " + page + " timestamp: " + isTimestamp)

                var batchFilesArray = []
                for(var i = 0; i < propertyListViewFiles.count; i++){
                    batchFilesArray[i] =  propertyListViewFiles.model.get(i).fileUrl;
                }
                propertyOutputSignedFile = outputFile;
                gapi.startSigningBatchPDF(batchFilesArray, outputFile, page, coord_x, coord_y,
                                          reason, location, isTimestamp, isSmallSignature)

            }
        }
    }

    propertyTextFieldReason{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
            propertyPageLoader.propertyBackupLocal = propertyTextFieldReason.text
        }
    }
    propertyTextFieldLocal{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text
            propertyPageLoader.propertyBackupReason = propertyTextFieldLocal.text
        }
    }

    propertyCheckSignReduced{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignReduced = propertyCheckSignReduced.checked
            if(propertyCheckSignReduced.checked){
                propertyPDFPreview.propertySigHidth = 45
                propertyPDFPreview.propertySigLineHeight = propertyPDFPreview.propertyDragSigRect.height * 0.2
                propertyPDFPreview.propertyDragSigReasonText.height = 0
                propertyPDFPreview.propertyDragSigLocationText.height = 0
                propertyPDFPreview.propertyDragSigReasonText.text = ""
                propertyPDFPreview.propertyDragSigLocationText.text = ""
                propertyPDFPreview.propertyDragSigImg.height = 0
                propertyPDFPreview.propertyDragSigWaterImg.height = 0
            }else{
                propertyPDFPreview.propertySigHidth = 90
                propertyPDFPreview.propertySigLineHeight = propertyPDFPreview.propertyDragSigRect.height * 0.1
                propertyPDFPreview.propertyDragSigReasonText.height = propertyPDFPreview.propertySigLineHeight
                propertyPDFPreview.propertyDragSigLocationText.height = propertyPDFPreview.propertySigLineHeight
                propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
                propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text
                propertyPDFPreview.propertyDragSigImg.height = propertyPDFPreview.propertyDragSigRect.height * 0.3
                propertyPDFPreview.propertyDragSigWaterImg.height = propertyPDFPreview.propertyDragSigRect.height * 0.4
            }
        }
    }
    ListModel {
        id: entityAttributesModel
        onCountChanged: {
            propertyTextAttributesMsg.visible = false
            propertyMouseAreaTextAttributesMsg.enabled = false
            propertyMouseAreaTextAttributesMsg.z = 0
        }
    }

    Component {
        id: attributeListDelegate
        Rectangle {
            id: container
            width: parent.width - Constants.SIZE_ROW_H_SPACE
            height: columnItem.height + 15
            color: Constants.COLOR_MAIN_SOFT_GRAY

            CheckBox {
                id: checkboxSel
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                anchors.verticalCenter: parent.verticalCenter
                checked: checkBoxAttr
                onCheckedChanged: {
                    entityAttributesModel.get(index).checkBoxAttr = checkboxSel.checked
                    var count = 0
                    for (var i = 0; i < entityAttributesModel.count; i++){
                        if(entityAttributesModel.get(i).checkBoxAttr == true){
                            propertyPageLoader.attributeListBackup[count] = i
                            count++
                        }
                    }
                }
            }
            Column {
                id: columnItem
                anchors.left: checkboxSel.right
                width: parent.width - checkboxSel.width
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text: entityName + " - " + attribute
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
            }

        }
    }

    propertyMouseAreaTextAttributesMsg{
        onClicked: {
            // Jump to Menu Definitions - PageDefinitionsSCAP
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
            mainFormID.propertyMainMenuListView.currentIndex = -1
            mainFormID.propertyMainMenuBottomListView.currentIndex = 0
            mainFormID.propertySubMenuListView.currentIndex = 1
            mainFormID.propertyPageLoader.source = "/contentPages/definitions/PageDefinitionsSCAP.qml"
        }
    }

    propertySwitchSignTemp{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupTempSign = propertySwitchSignTemp.checked
        }
    }

    propertyCheckSignShow{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignShow = propertyCheckSignShow.checked
            if(propertyCheckSignShow.checked){
                propertyCheckLastPage.enabled = true
                propertySpinBoxControl.enabled = true
                propertySpinBoxControl.up.indicator.enabled = true
                propertySpinBoxControl.down.indicator.enabled = true
                propertySpinBoxControl.up.indicator.opacity = 1
                propertySpinBoxControl.down.indicator.opacity = 1
                propertyTextSpinBox.visible = true
                propertyPageText.opacity = 1
            }else{
                propertyCheckLastPage.enabled = false
                propertySpinBoxControl.enabled = false
                propertySpinBoxControl.up.indicator.enabled = false
                propertySpinBoxControl.down.indicator.enabled = false
                propertySpinBoxControl.up.indicator.opacity = Constants.OPACITY_SIGNATURE_TEXT_DISABLED
                propertySpinBoxControl.down.indicator.opacity = Constants.OPACITY_SIGNATURE_TEXT_DISABLED
                propertyTextSpinBox.visible = false
                propertyPageText.opacity = Constants.OPACITY_SIGNATURE_TEXT_DISABLED
            }
        }
    }

    propertySwitchSignAdd{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignAdd = propertySwitchSignAdd.checked
            if(propertySwitchSignAdd.checked){
                console.log("propertySwitchSignAdd checked")
                propertyBusyIndicator.running = true
                propertyTextFieldReason.enabled = false
                propertyTextFieldLocal.enabled = false
                propertyTextFieldReason.opacity = Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                propertyTextFieldLocal.opacity = Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                propertyTextFieldReason.text = ""
                propertyTextFieldLocal.text = ""
                propertySwitchSignTemp.checked = false
                propertySwitchSignTemp.enabled = false
                propertyButtonSignCMD.enabled = false
                propertyCheckSignShow.checked = true
                propertyCheckSignShow.enabled = false
                propertyCheckSignReduced.checked = false
                propertyCheckSignReduced.enabled = false
                propertyRadioButtonXADES.enabled = false
                propertyTextAttributesMsg.visible = true
                propertyMouseAreaTextAttributesMsg.enabled = true
                propertyMouseAreaTextAttributesMsg.z = 1
                // Load attributes from cache (all, isShortDescription)
                gapi.startLoadingAttributesFromCache(2, 1)
            }else{
                console.log("propertySwitchSignAdd not checked")
                entityAttributesModel.clear()
                propertyTextFieldReason.enabled = true
                propertyTextFieldLocal.enabled = true
                propertyTextFieldReason.opacity = 1.0
                propertyTextFieldLocal.opacity = 1.0
                propertySwitchSignTemp.enabled = true
                propertyButtonSignCMD.enabled = true
                propertyCheckSignReduced.enabled = true
                propertyCheckSignShow.enabled = true
                propertyRadioButtonXADES.enabled = true
                propertyTextAttributesMsg.visible = false
                propertyMouseAreaTextAttributesMsg.enabled = false
                propertyMouseAreaTextAttributesMsg.z = 0
            }
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Num files: " + propertyFileDialog.fileUrls.length)

            for(var i = 0; i < propertyFileDialog.fileUrls.length; i++){
                console.log("Adding file: " + propertyFileDialog.fileUrls[i])
                var path = propertyFileDialog.fileUrls[i];
                //  Get the path itself without a regex
                if (Qt.platform.os === "windows") {
                    path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                }else{
                    path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                }
				path = decodeURIComponent(path)
                filesModel.append({
                                      "fileUrl": path
                                  })
                propertyPageLoader.propertyBackupfilesModel.append({
                                      "fileUrl": path
                                  })
            }

            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onRejected: {
            console.log("Canceled")
        }
    }


    propertyMouseAreaRectMainRigh {
        onClicked: {
            console.log("propertyMouseAreaRectMainRigh clicked")
            propertyFileDialog.visible = true
        }
    }

    propertyMouseAreaItemOptionsFiles {
        onClicked: {
            console.log("propertyMouseAreaItemOptionsFiles clicked")
            propertyFileDialog.visible = true
        }
    }
    propertyButtonAdd {
        onClicked: {
            console.log("propertyButtonAdd clicked")
            propertyFileDialog.visible = true
        }
    }
    propertyButtonRemoveAll {
        onClicked: {
            console.log("Removing all files")
            filesModel.clear()
            propertyPageLoader.propertyBackupfilesModel.clear()
        }
    }

    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            if (propertyListViewFiles.count == 1){
                propertyFileDialogBatchOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
                if (propertyRadioButtonPADES.checked) {
                    if(propertySwitchSignAdd.checked){
                        var count = 0
                        //15 MB filesize limit for SCAP
                        var MAX_SIZE = 15 * 1024 *1024
                        var outputNativePath = stripFilePrefix(propertyListViewFiles.model.get(0).fileUrl)
                        for (var i = 0; i < entityAttributesModel.count; i++){
                            if(entityAttributesModel.get(i).checkBoxAttr == true){
                                count++
                            }
                        }
                        if(count == 0) {
                            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                    qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                    qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                        }
                        else if (gapi.getFileSize(outputNativePath) > MAX_SIZE) {
                            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                    qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                    qsTranslate("PageServicesSign","STR_SCAP_MAX_FILESIZE") + " 15 MB"
                            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                        }
                        else {
                            var outputFile = propertyListViewFiles.model.get(0).fileUrl
                            //Check if filename has extension and remove it.
                            if( outputFile.lastIndexOf('.') > 0)
                                var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                            propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
                            propertyFileDialogOutput.open()
                        }
                    }else{
                        var outputFile =  propertyListViewFiles.model.get(0).fileUrl
                        //Check if filename has extension and remove it.
                        if( outputFile.lastIndexOf('.') > 0)
                            var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                        propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
                        propertyFileDialogOutput.open()
                    }
                }
                else {
                    var outputFile = propertyListViewFiles.model.get(0).fileUrl
                    //Check if filename has extension and remove it.
                    if( outputFile.lastIndexOf('.') > 0)
                        var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))

                    propertyFileDialogOutput.filename = outputFile + "_xadessign.ccsigned"
                    propertyFileDialogOutput.open()
                }
            }else{
                if (propertySwitchSignAdd.checked){
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageServicesSign","STR_MULTI_FILE_ATTRIBUTES_WARNING_MSG")
                    mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                }else{
                    var outputFile = propertyListViewFiles.model.get(propertyListViewFiles.count-1).fileUrl
                    //Check if filename has extension and remove it.
                    if( outputFile.lastIndexOf('.') > 0)
                        var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                    //Check if filename has file name and remove it.
                    if( outputFile.lastIndexOf('/') > 0)
                        var outputFile = outputFile.substring(0, outputFile.lastIndexOf('/'))

                    if(propertyRadioButtonPADES.checked){
                        propertyFileDialogBatchOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT_FOLDER")
                        propertyFileDialogBatchOutput.open()
                    }else{

                        propertyFileDialogOutput.filename = outputFile + "/" + "xadessign.ccsigned"
                        propertyFileDialogOutput.open()
                    }
                }
            }
        }
    }
    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD" )
            if (propertyListViewFiles.count == 1){
                var outputFile =  filesModel.get(0).fileUrl
                //Check if filename has extension and remove it.
                if( outputFile.lastIndexOf('.') > 0)
                    var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                propertyFileDialogCMDOutput.filename = outputFile + "_signed.pdf"
                propertyFileDialogCMDOutput.open()
            }else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("PageServicesSign","STR_MULTI_FILE_CMD_WARNING")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("PageServicesSign","STR_MULTI_FILE_CMD_WARNING_MSG")
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            }
        }
    }

    propertyTextSpinBox{
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
    }

    propertyFileDialog{

        nameFilters: propertyRadioButtonPADES.checked ?
                         [ "PDF document (*.pdf)", "All files (*)" ] :
                         [ "All files (*)" ]
    }

    propertyRadioButtonPADES{
        onCheckedChanged:{
            propertyPageLoader.propertyBackupFormatPades = propertyRadioButtonPADES.checked
            if(propertyRadioButtonPADES.checked){
				propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                        qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")
                propertyTextFieldReason.enabled = true
                propertyTextFieldLocal.enabled = true
                propertyTextFieldReason.opacity = 1.0
                propertyTextFieldLocal.opacity = 1.0
                propertySwitchSignAdd.enabled = true
                filesModel.clear()
                for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
                {
                    filesModel.append({
                                          "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                                      })
                }
            }else{
				propertyTextDragMsgImg.text =
                        qsTranslate("PageServicesSign","STR_SIGN_NOT_PREVIEW")
                propertyTextFieldReason.enabled = false
                propertyTextFieldLocal.enabled = false
                propertyTextFieldReason.opacity = Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                propertyTextFieldLocal.opacity = Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                propertySwitchSignAdd.enabled = false
                filesModel.clear()
                for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
                {
                    filesModel.append({
                                          "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                                      })
                }
            }
        }
    }

    Component {
        id: listViewFilesDelegate
        Rectangle{
            id: rectlistViewFilesDelegate
            width: parent.width - propertyFilesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            color: getColorItem(mouseAreaFileName.containsMouse,
                                mouseAreaIconDelete.containsMouse)
            MouseArea {
                id: mouseAreaFileName
                anchors.fill: parent
                hoverEnabled : true
            }

            Item {
                width: parent.width
                height: parent.height

                Text {
                    id: fileName
                    text: fileUrl
                    width: parent.width - iconRemove.width - Constants.SIZE_LISTVIEW_IMAGE_SPACE
                    x: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: Constants.COLOR_TEXT_BODY
                    wrapMode: Text.WrapAnywhere
                    Component.onCompleted: {
                        if(fileName.height > iconRemove.height){
                            rectlistViewFilesDelegate.height = fileName.height + 5
                        }else{
                            rectlistViewFilesDelegate.height = iconRemove.height
                        }
                    }
                }
                Image {
                    id: iconRemove
                    x: fileName.width + Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    width: Constants.SIZE_IMAGE_FILE_REMOVE
                    height: Constants.SIZE_IMAGE_FILE_REMOVE
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                    source:  mouseAreaIconDelete.containsMouse ?
                                 "../../images/remove_file_hover.png" :
                                 "../../images/remove_file.png"
                    MouseArea {
                        id: mouseAreaIconDelete
                        anchors.fill: parent
                        hoverEnabled : true
                        onClicked: {
                            console.log("Delete file index:" + index);
                            filesModel.remove(index)
                        }
                    }
                }
            }
        }
    }
    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("propertyListViewFiles onCountChanged count:"
                        + propertyListViewFiles.count)
            if(filesModel.count === 0) {
                fileLoaded = false
                propertyTextDragMsgImg.visible = true
                propertyPDFPreview.propertyBackground.source = ""
                propertyPDFPreview.propertyDragSigImg.visible = false
                propertyPDFPreview.propertyDragSigReasonText.visible = false
                propertyPDFPreview.propertyDragSigSignedByText.visible = false
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = false
                propertyPDFPreview.propertyDragSigNumIdText.visible = false
                propertyPDFPreview.propertyDragSigDateText.visible = false
                propertyPDFPreview.propertyDragSigLocationText.visible = false
                propertyPDFPreview.propertyDragSigImg.visible = false
            }
            else {
                fileLoaded = true
                if(propertyRadioButtonPADES.checked){
                    propertyTextDragMsgImg.visible = false
                    // Preview the last file selected
                    var loadedFilePath = filesModel.get(filesModel.count-1).fileUrl
                    var pageCount = gapi.getPDFpageCount(loadedFilePath)
                    if(pageCount > 0){
                        propertyBusyIndicator.running = true
                        if(propertyCheckLastPage.checked==true
                                || propertySpinBoxControl.value > pageCount)
                            propertySpinBoxControl.value = pageCount
                        if(propertySpinBoxControl.value > getMinimumPage())
                            propertySpinBoxControl.value = getMinimumPage()
                        updateIndicators(pageCount)
                        propertyPDFPreview.propertyBackground.cache = false
                        propertyPDFPreview.propertyBackground.source =
                                "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
                        var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.png"
                        if(gapi.getUseCustomSignature() && gapi.customSignImageExist()){
                            if (Qt.platform.os === "windows") {
                                urlCustomImage = "file:///"+urlCustomImage
                            }else{
                                urlCustomImage = "file://"+urlCustomImage
                            }
                            propertyPDFPreview.propertyDragSigImg.source = urlCustomImage
                        }else{
                            propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC.png"
                        }
                        propertyPDFPreview.propertyDragSigImg.visible = true
                        propertyPDFPreview.propertyDragSigWaterImg.source = "qrc:/images/pteid_signature_watermark.jpg"
                        propertyPDFPreview.propertyDragSigWaterImg.visible = true
                        propertyPDFPreview.propertyDragSigReasonText.visible = true
                        propertyPDFPreview.propertyDragSigSignedByText.visible = true
                        propertyPDFPreview.propertyDragSigSignedByNameText.visible = true
                        propertyPDFPreview.propertyDragSigNumIdText.visible = true
                        propertyPDFPreview.propertyDragSigDateText.visible = true
                        propertyPDFPreview.propertyDragSigLocationText.visible = true
                        propertyPDFPreview.propertyDragSigImg.visible = true
                        propertyBusyIndicator.running = false
                    }else{
                        console.log("Error loading pdf file")
                        filesModel.remove(propertyListViewFiles.count-1)
                        mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR")
                        mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                qsTranslate("PageServicesSign","STR_LOAD_ADVANCED_PDF_ERROR_MSG")
                        mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                    }
                }else{
				    propertyTextDragMsgImg.visible = true
				}
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            var loadedFilePath = filesModel.get(filesModel.count-1).fileUrl
            propertyPDFPreview.propertyBackground.source =
                    "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            propertyPageLoader.propertyBackupPage =  propertySpinBoxControl.value

            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            updateIndicators(pageCount)
        }
    }
    propertyCheckLastPage {
        onCheckedChanged: {
            var loadedFilePath = propertyListViewFiles.model.get(filesModel.count-1).fileUrl
            propertyPageLoader.propertyBackupLastPage =  propertyCheckLastPage.checked
            var pageCount = gapi.getPDFpageCount(loadedFilePath)

            if(propertyCheckLastPage.checked){
                propertySpinBoxControl.enabled = false
                propertyPageText.enabled = false
                propertyTextSpinBox.visible = false
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
            }else{
                propertySpinBoxControl.enabled = true
                propertyPageText.enabled = true
                propertyTextSpinBox.visible = true
                if(propertySpinBoxControl.value > getMinimumPage())
                    propertySpinBoxControl.value = getMinimumPage()
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            }
        }
    }

    onIsAnimationFinishedChanged: {
        if(isAnimationFinished == true){
            loadUnfinishedSignature()
        }
    }

    Component.onCompleted: {
        console.log("Page Services Sign Advanced mainWindowCompleted")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }

    function loadUnfinishedSignature(){
        // Load backup data about unfinished advance signature
        propertyRadioButtonPADES.checked = propertyPageLoader.propertyBackupFormatPades
        propertyRadioButtonXADES.checked = !propertyPageLoader.propertyBackupFormatPades
        propertySwitchSignTemp.checked = propertyPageLoader.propertyBackupTempSign
        propertySwitchSignAdd.checked = propertyPageLoader.propertyBackupSignAdd
        propertyCheckSignShow.checked = propertyPageLoader.propertyBackupSignShow
        propertyCheckSignReduced.checked = propertyPageLoader.propertyBackupSignReduced

        filesModel.clear()
        for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
        {
            filesModel.append({
                                  "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                              })
        }

        propertySpinBoxControl.value = propertyPageLoader.propertyBackupPage
        propertyCheckLastPage.checked = propertyPageLoader.propertyBackupLastPage
        propertyTextFieldReason.text = propertyPageLoader.propertyBackupLocal
        propertyTextFieldLocal.text = propertyPageLoader.propertyBackupReason

        if (gapi.getShortcutFlag() > 0)
            filesModel.append(
                        {
                            "fileUrl": gapi.getShortcutInputPDF()
                        });

        propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")
        propertyPDFPreview.propertyDragSigSignedByNameText.text =
                qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
        propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
    }

    function stripFilePrefix(filePath) {
        if (Qt.platform.os === "windows") {
               return filePath.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
        }
        else {
            return filePath.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
        }
    }

    function forceScrollandFocus() {
        // Force scroll and focus to the last item added
        propertyListViewFiles.positionViewAtEnd()
        propertyListViewFiles.forceActiveFocus()
        propertyListViewFiles.currentIndex = propertyListViewFiles.count -1
        if(propertyFilesListViewScroll.position > 0)
            propertyFilesListViewScroll.active = true
    }
    function getColorItem(mouseItem, mouseImage){

        var handColor
        if(mouseItem || mouseImage){
            handColor = Constants.COLOR_MAIN_DARK_GRAY
        }else{
            handColor = Constants.COLOR_MAIN_SOFT_GRAY
        }
        return handColor
    }
    function getMinimumPage(){
        // Function to detect minimum number of pages of all loaded pdfs to sign
        var minimumPage = 0
        for(var i = 0 ; i < filesModel.count ; i++ ){
            var loadedFilePath = filesModel.get(i).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            if(minimumPage == 0 || pageCount < minimumPage)
                minimumPage = pageCount

            console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount
                        + "minimum: " + minimumPage)
        }
        return minimumPage
    }

    function updateIndicators(pageCount){
        propertySpinBoxControl.up.indicator.visible = true
        propertySpinBoxControl.down.indicator.visible = true
        propertySpinBoxControl.up.indicator.enabled = true
        propertySpinBoxControl.down.indicator.enabled = true

        if(propertySpinBoxControl.value === pageCount
                || propertySpinBoxControl.value === getMinimumPage()){
            propertySpinBoxControl.up.indicator.visible = false
            propertySpinBoxControl.up.indicator.enabled = false
        }
        if (propertySpinBoxControl.value === 1){
            propertySpinBoxControl.down.indicator.visible = false
            propertySpinBoxControl.down.indicator.enabled = false
        }
    }
}
