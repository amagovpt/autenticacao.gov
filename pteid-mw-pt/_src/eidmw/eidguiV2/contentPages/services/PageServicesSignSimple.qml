import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignSimpleForm {

    property string propertyOutputSignedFile : ""

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalOpenCMDSucess: {
            console.log("Signal Open CMD Sucess")
            progressBarIndeterminate.visible = false
            rectReturnCode.visible = true
            buttonCMDProgressConfirm.visible = true
        }
        onSignalCloseCMDSucess: {
            console.log("Signal Close CMD Sucess")
            progressBarIndeterminate.visible = false
            rectLabelCMDText.visible = true
            buttonCMDProgressConfirm.visible = true
            buttonCMDProgressConfirm.text = qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
        }
        onSignalPdfSignSucess: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            signsuccess_dialog.visible = true
            propertyBusyIndicator.running = false
        }
        onSignalPdfSignFail: {
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalCardAccessError: {
            console.log("Sign simple Page onSignalCardAccessError")
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
                else if(error_code == GAPI.PinBlocked) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                cardLoaded = false
            }
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Simple --> Data Changed")
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
            console.log("Services Sign Simple onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyPDFPreview.propertyDragSigSignedByNameText.text =
                        qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
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
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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
            propertyBusyIndicator.running = false
        }
    }
    Dialog {
        id: dialog
        title: qsTranslate("Popup File","STR_POPUP_FILE_UNIQUE")
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: qsTranslate("Popup File","STR_POPUP_FILE_UNIQUE_MULTI")
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

            Keys.enabled: true
            Keys.onPressed: {
                  if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return
                          && textFieldMobileNumber.length !== 0 && textFieldPin.length !== 0)
                  {
                      signCMD()
                  }
            }

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
                    onCurrentIndexChanged: {
                        if(comboBoxIndicative.currentIndex > 0){
                            propertyPageLoader.propertyBackupMobileIndicatorIndex = comboBoxIndicative.currentIndex
                        }else{
                            comboBoxIndicative.currentIndex = propertyPageLoader.propertyBackupMobileIndicatorIndex
                        }
                    }
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
                    onEditingFinished: {
                        propertyPageLoader.propertyBackupMobileNumber = textFieldMobileNumber.text
                    }
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
                        textFieldPin.text = ""
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
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
                        signCMD()
                    }
                }
            }
        }
        onRejected:{
            textFieldPin.text = ""
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
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
        focus: true

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

            Keys.enabled: true
            Keys.onPressed: {
                  if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return && buttonCMDProgressConfirm.visible == true)
                  {
                      signCMDConfirm()
                  }
            }

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
                    textReturnCode.text = ""
                    rectReturnCode.visible = false
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
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
                    signCMDConfirm()
                }
            }
        }
        onRejected:{
            textReturnCode.text = ""
            rectReturnCode.visible = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
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

        header: Label {
            text: qsTranslate("PageServicesSign","STR_SIGN_SUCESS")
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

            Keys.enabled: true
            Keys.onPressed: {
                  if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                  {
                      signCMDShowSignedFile()
                  }
            }

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    text: qsTranslate("PageServicesSign","STR_SIGN_OPEN")
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
                    text: qsTranslate("PageServicesSign","STR_POPUP_FILE_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signsuccess_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
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
                        signCMDShowSignedFile()
                    }
                }
            }
        }
        onRejected:{
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
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

    propertyDropArea {

        onEntered: {
            console.log("Signature simple ! You chose file(s): " + drag.urls);

            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            // Update sign preview position variables to be used to send to sdk
            propertyPDFPreview.updateSignPreview(drop.x,drop.y)
            //TODO: Validate files type
            if(filesArray.length > 1){
                dialog.open()
            }else if(filesArray.length == 1){
                // Needed because the PDFPreview
                var path =  filesArray[0]
                //  Get the path itself without a regex
                if (Qt.platform.os === "windows") {
                    path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                }else{
                    path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                }
				path = decodeURIComponent(path)
				console.log("Adding file: " + path)
                filesModel.insert(0, {"fileUrl": path})
            }
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }
    propertyFileDialogOutput {
        onAccepted: {
            propertyBusyIndicator.running = true
            var loadedFilePath = filesModel.get(0).fileUrl
            var isTimestamp = false
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
			outputFile = decodeURIComponent(outputFile)

                var page = 1
                propertyCheckLastPage.checked ? page = gapi.getPDFpageCount(loadedFilePath) :
                                            page = propertySpinBoxControl.value
                var reason = ""
                var location = ""

                var isSmallSignature = false

                var coord_x = propertyPDFPreview.propertyCoordX

                //coord_y must be the lower left corner of the signature rectangle

                var coord_y = propertyPDFPreview.propertyCoordY

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y)

                propertyOutputSignedFile = outputFile;

                gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                     reason, location, isTimestamp, isSmallSignature)
        }
    }
    propertyFileDialogCMDOutput {
        onAccepted: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            dialogSignCMD.open()
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            var path = propertyFileDialog.fileUrls[0];

            //  Get the path itself without a regex
            if (Qt.platform.os === "windows") {
                path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
			path = decodeURIComponent(path)
			console.log("Adding file: " + path)
            filesModel.append({
                                  "fileUrl": path
                              })
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    propertyMouseAreaRectMain {
        onClicked: {
            console.log("propertyMouseAreaRectMainRigh clicked")
            propertyFileDialog.visible = true
        }
    }

    propertyButtonRemove {
        onClicked: {
            fileLoaded = false
            filesModel.clear()
        }
    }
    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")

            var outputFile = filesModel.get(0).fileUrl
            //Check if filename has extension and remove it.
            if( outputFile.lastIndexOf('.') > 0)
                var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'));
            propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
            propertyFileDialogOutput.open()
        }
    }
    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD")
            var outputFile = filesModel.get(0).fileUrl
            //Check if filename has extension and remove it.
            if( outputFile.lastIndexOf('.') > 0)
                var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'));
            propertyFileDialogCMDOutput.filename = outputFile + "_signed.pdf";
            propertyFileDialogCMDOutput.open()
        }
    }

    propertyTextSpinBox{
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
    }

    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged count:"
                        + filesModel.count)
            if(filesModel.count === 0){
                fileLoaded = false
                propertyPDFPreview.propertyBackground.source = ""
                propertyPDFPreview.propertyDragSigImg.visible = false
                propertyPDFPreview.propertyDragSigSignedByText.visible = false
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = false
                propertyPDFPreview.propertyDragSigNumIdText.visible = false
                propertyPDFPreview.propertyDragSigDateText.visible = false
                propertyPDFPreview.propertyDragSigImg.visible = false
            }else{
                propertyBusyIndicator.running = true
                var loadedFilePath = filesModel.get(0).fileUrl
                var pageCount = gapi.getPDFpageCount(loadedFilePath)
                if(pageCount > 0){
                    console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount)
                    fileLoaded = true
                    propertyPDFPreview.propertyBackground.cache = false

                    if(propertyCheckLastPage.checked == true){
                        propertySpinBoxControl.value = pageCount
                        propertyPDFPreview.propertyBackground.source =
                                "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
                    }else{
                        if(propertySpinBoxControl.value > pageCount){
                            propertySpinBoxControl.value = 1
                            propertyPDFPreview.propertyBackground.source
                                    = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=1"
                        }else{
                            propertyPDFPreview.propertyBackground.source
                                    = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
                        }
                    }
                    updateIndicators(pageCount)

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
                    propertyPDFPreview.propertyDragSigSignedByText.visible = true
                    propertyPDFPreview.propertyDragSigSignedByNameText.visible = true
                    propertyPDFPreview.propertyDragSigNumIdText.visible = true
                    propertyPDFPreview.propertyDragSigDateText.visible = true
                    propertyPDFPreview.propertyDragSigImg.visible = true
                }else{
                    console.log("Error loading pdf file")
                    propertyBusyIndicator.running = false
                    filesModel.remove(filesModel.count-1)
                    fileLoaded = false
                    propertyPDFPreview.propertyBackground.source = ""
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR_MSG")
                    mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                    mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                }
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            propertyPDFPreview.propertyBackground.source =
                    "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            updateIndicators(pageCount)
        }
    }
    propertyCheckLastPage {
        onCheckedChanged: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            propertySpinBoxControl.value = pageCount
            if(propertyCheckLastPage.checked){
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
            }else{
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            }
        }
    }
    Component.onCompleted: {
        if (gapi.getShortcutFlag() > 0)
            filesModel.append(
                        {
                            "fileUrl": gapi.getShortcutInputPDF()
                        });

        console.log("Page Services Sign Simple mainWindowCompleted")
        propertyBusyIndicator.running = true
        propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
        propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"

        //  CMD load backup mobile data
        textFieldMobileNumber.text = propertyPageLoader.propertyBackupMobileNumber

        gapi.startCardReading()
    }
    function updateIndicators(pageCount){
        propertySpinBoxControl.up.indicator.visible = true
        propertySpinBoxControl.down.indicator.visible = true
        propertySpinBoxControl.up.indicator.enabled = true
        propertySpinBoxControl.down.indicator.enabled = true
        if(propertySpinBoxControl.value >= pageCount){
            propertySpinBoxControl.up.indicator.visible = false
            propertySpinBoxControl.up.indicator.enabled = false
        }
        if (propertySpinBoxControl.value === 1){
            propertySpinBoxControl.down.indicator.visible = false
            propertySpinBoxControl.down.indicator.enabled = false
        }
    }
    function signCMD(){
        var loadedFilePath = filesModel.get(0).fileUrl
        var isTimestamp = false
        var outputFile = propertyFileDialogCMDOutput.fileUrl.toString()
        if (Qt.platform.os === "windows") {
            outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
        }else{
            outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
        }
        outputFile = decodeURIComponent(outputFile)

        var page = 1
        if(propertyCheckLastPage.checked) {
            page = gapi.getPDFpageCount(loadedFilePath)
        }else{
            page = propertySpinBoxControl.value
        }
        var reason = ""
        var location = ""
        var isSmallSignature = false
        var coord_x = propertyPDFPreview.propertyCoordX
        //coord_y must be the lower left corner of the signature rectangle
        var coord_y = propertyPDFPreview.propertyCoordY

        console.log("Output filename: " + outputFile)
        console.log("Signing in position coord_x: " + coord_x
                    + " and coord_y: "+coord_y)

        var countryCode = comboBoxIndicative.currentText.substring(0, comboBoxIndicative.currentText.indexOf(' '));
        var mobileNumber = countryCode + " " + textFieldMobileNumber.text

        propertyOutputSignedFile = outputFile
        rectLabelCMDText.visible = false
        gapi.signOpenCMD(mobileNumber,textFieldPin.text,
                     loadedFilePath,outputFile,page,
                     coord_x,coord_y,
                     reason,location,
                     isTimestamp, isSmallSignature)

        progressBarIndeterminate.visible = true
        progressBar.visible = true
        textFieldPin.text = ""
        textReturnCode.text = ""
        dialogSignCMD.close()
        buttonCMDProgressConfirm.visible = false
        buttonCMDProgressConfirm.text = qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
        dialogCMDProgress.open()
        textFieldReturnCode.focus = true
    }
    function signCMDConfirm(){
        console.log("Send sms_token : " + textFieldReturnCode.text)
        if( progressBar.value < 100) {
            //Empty attributes list, in simple signature view it's not SCAP signature
            gapi.signCloseCMD(textFieldReturnCode.text, [])
            progressBarIndeterminate.visible = true
            rectReturnCode.visible = false
            buttonCMDProgressConfirm.visible = false
            textFieldReturnCode.text = ""
            dialogCMDProgress.open()
        }
        else
        {
            dialogCMDProgress.close()
            if (Qt.platform.os === "windows") {
                propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
            }else{
                propertyOutputSignedFile = "file://" + propertyOutputSignedFile
            }
            console.log("Open Url Externally: " + propertyOutputSignedFile)
            Qt.openUrlExternally(propertyOutputSignedFile)
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
    }
    function signCMDShowSignedFile(){
        if (Qt.platform.os === "windows") {
            propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
        }else{
            propertyOutputSignedFile = "file://" + propertyOutputSignedFile
        }
        console.log("Open Url Externally: " + propertyOutputSignedFile)
        Qt.openUrlExternally(propertyOutputSignedFile)
        signsuccess_dialog.close()
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
    }
}
