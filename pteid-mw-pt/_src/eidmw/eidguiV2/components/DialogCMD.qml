import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../scripts/Constants.js" as Constants

Item {       
    y: parent.height * 0.5 - dialogSignCMD.height * 0.5
    
    Connections {
        target: gapi
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
        onSignalUpdateProgressBar: {
            console.log("CMD sign change --> update progress bar with value = " + value)
            progressBar.value = value
            if(value === 100) {
                progressBarIndeterminate.visible = false
            }
        }
        onSignalUpdateProgressStatus: {
            console.log("CMD sign change --> update progress status with text = " + statusMessage)
            textMessageTop.text = statusMessage
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
        y: parent.height 

        header: Label {
            id: labelTextTitle
            text: qsTranslate("PageServicesSign","STR_SIGN_CMD")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
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
            // Reject CMD Popup's only with ESC key
            dialogSignCMD.open()
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
        y: parent.height
        focus: true

        header: Label {
            id: labelConfirmOfAddressProgressTextTitle
            text: qsTranslate("PageServicesSign","STR_SIGN_CMD")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
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
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
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
            // Reject CMD Popup's only with ESC key
            dialogCMDProgress.open()
        }
    }

    function open() {
        dialogSignCMD.open()
    }
    function signCMD(){
        var loadedFilePath = filesModel.get(0).fileUrl
        var isTimestamp = false
        var outputFile = propertyFileDialogCMDOutput.fileUrl.toString()

        outputFile = decodeURIComponent(stripFilePrefix(outputFile))

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
        textFieldReturnCode.text = ""
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
}