
Imports be.portugal.eid

Public Class frmMain

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOk.Click

        Me.Close()
        PTEID_ReaderSet.releaseSDK()

        Application.Exit()

    End Sub

    Private Sub btnLoad_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLoad.Click

        Try

            Dim ReaderSet As PTEID_ReaderSet
            ReaderSet = PTEID_ReaderSet.instance()

            Dim Reader As PTEID_ReaderContext
            Reader = ReaderSet.getReader()

            Me.img.Image = Nothing

            Dim sText As String
            sText = "Reader = " & Reader.getName() & Constants.vbCrLf & Constants.vbCrLf
            If Reader.isCardPresent() Then
                If Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_EID _
                    Or Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_FOREIGNER _
                    Or Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_KIDS Then

                    sText &= Load_eid(Reader)

                ElseIf Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_SIS Then

                    sText &= Load_sis(Reader)

                Else
                    sText &= "CARD TYPE UNKNOWN"
                End If
            End If

            Me.txtInfo.Text = sText

            PTEID_ReaderSet.releaseSDK()

        Catch ex As PTEID_Exception
            Debug.Print("Crash PTEID_Exception!")
            PTEID_ReaderSet.releaseSDK()
        Catch ex As Exception
            Debug.Print("Crash System.Exception!")
            PTEID_ReaderSet.releaseSDK()
        End Try

    End Sub

    Private Function Load_eid(ByVal Reader As PTEID_ReaderContext) As String
        Dim sText As String
        Dim card As PTEID_EIDCard
        card = Reader.getEIDCard()
        If card.isTestCard() Then
            card.setAllowTestCard(True)
        End If

        Dim doc As PTEID_EId
        doc = card.getID()

        sText = ""
        sText &= "PERSONAL DATA" & Constants.vbCrLf
        sText &= Constants.vbCrLf
        sText &= "First Name = " & doc.getFirstName() & Constants.vbCrLf
        sText &= "Last Name = " & doc.getSurname() & Constants.vbCrLf
        sText &= "Gender = " & doc.getGender() & Constants.vbCrLf
        sText &= "DateOfBirth = " & doc.getDateOfBirth() & Constants.vbCrLf
        sText &= "LocationOfBirth = " & doc.getLocationOfBirth() & Constants.vbCrLf
        sText &= "Nobility = " & doc.getNobility() & Constants.vbCrLf
        sText &= "Nationality = " & doc.getNationality() & Constants.vbCrLf
        sText &= "NationalNumber = " & doc.getNationalNumber() & Constants.vbCrLf
        sText &= "SpecialOrganization = " & doc.getSpecialOrganization() & Constants.vbCrLf
        sText &= "MemberOfFamily = " & doc.getMemberOfFamily() & Constants.vbCrLf
        sText &= "AddressVersion = " & doc.getAddressVersion() & Constants.vbCrLf
        sText &= "Street = " & doc.getStreet() & Constants.vbCrLf
        sText &= "ZipCode = " & doc.getZipCode() & Constants.vbCrLf
        sText &= "Municipality = " & doc.getMunicipality() & Constants.vbCrLf
        sText &= "Country = " & doc.getCountry() & Constants.vbCrLf
        sText &= "SpecialStatus = " & doc.getSpecialStatus() & Constants.vbCrLf

        sText &= Constants.vbCrLf
        sText &= Constants.vbCrLf

        sText &= "CARD DATA" & Constants.vbCrLf
        sText &= Constants.vbCrLf
        sText &= "LogicalNumber = " & doc.getLogicalNumber() & Constants.vbCrLf
        sText &= "ChipNumber = " & doc.getChipNumber() & Constants.vbCrLf
        sText &= "ValidityBeginDate = " & doc.getValidityBeginDate() & Constants.vbCrLf
        sText &= "ValidityEndDate = " & doc.getValidityEndDate() & Constants.vbCrLf
        sText &= "IssuingMunicipality = " & doc.getIssuingMunicipality() & Constants.vbCrLf

        Dim photo As Image
        Dim picture As PTEID_Picture
        picture = card.getPicture()

        Dim bytearray As Byte()
        bytearray = picture.getData().GetBytes()

        Dim ms As MemoryStream
        ms = New MemoryStream()
        ms.Write(bytearray, 0, bytearray.Length)

        photo = Image.FromStream(ms, True)
        Me.img.Image = photo

        Return sText

    End Function

    Private Function Load_sis(ByVal Reader As PTEID_ReaderContext) As String
        Dim sText As String
        Dim card As PTEID_SISCard
        card = Reader.getSISCard()

        Dim doc As PTEID_SisId
        doc = card.getID()

        sText = ""
        sText &= "PERSONNAL DATA" & Constants.vbCrLf
        sText &= Constants.vbCrLf
        sText &= "Name = " & doc.getName() & Constants.vbCrLf
        sText &= "Surname = " & doc.getSurname() & Constants.vbCrLf
        sText &= "Initials = " & doc.getInitials() & Constants.vbCrLf
        sText &= "Gender = " & doc.getGender() & Constants.vbCrLf
        sText &= "DateOfBirth = " & doc.getDateOfBirth() & Constants.vbCrLf
        sText &= "SocialSecurityNumber = " & doc.getSocialSecurityNumber() & Constants.vbCrLf

        sText &= Constants.vbCrLf
        sText &= Constants.vbCrLf

        sText &= "CARD DATA" & Constants.vbCrLf
        sText &= Constants.vbCrLf
        sText &= "LogicalNumber = " & doc.getLogicalNumber() & Constants.vbCrLf
        sText &= "DateOfIssue = " & doc.getDateOfIssue() & Constants.vbCrLf
        sText &= "ValidityBeginDate = " & doc.getValidityBeginDate() & Constants.vbCrLf
        sText &= "ValidityEndDate = " & doc.getValidityEndDate() & Constants.vbCrLf

        Return sText

    End Function

End Class
