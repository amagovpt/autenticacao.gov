'EIDLIBCTRLLib

Public Class Form1

    Private Sub btnClose_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnClose.Click
        'Close the application
        Me.Close()
        Application.Exit()

    End Sub

    Private Sub btnRead_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnRead.Click
        'Open the File dialog and save the file name
        Me.dlgopenFile.Filter = "Eid files (*.eid)|*.eid"
        Me.dlgopenFile.FileName = ""

        If Me.dlgopenFile.ShowDialog() <> Windows.Forms.DialogResult.OK Then
            Return
        End If

        Me.txtPath.Text = Me.dlgopenFile.FileName

        'Read the File, parse it, then fill the rawData structure and get the field value
        If Not System.IO.File.Exists(Me.txtPath.Text) Then
            MessageBox.Show("File does not exist!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        Dim FullCOntent As Byte()
        FullCOntent = System.IO.File.ReadAllBytes(Me.txtPath.Text)

        'Parse the file and fill the raw structure
        Dim raw As New EIDLIBCTRLLib.Raw
        ParseFile(FullCOntent, raw)

        Dim lhandle As Long
        Dim retStatus As EIDLIBCTRLLib.RetStatus

        Dim eidlib As New EIDLIBCTRLLib.EIDlib
        retStatus = eidlib.Init("VIRTUAL", 0, 0, lhandle)

        Dim MapColID As New EIDLIBCTRLLib.MapCollection
        Dim MapColAddress As New EIDLIBCTRLLib.MapCollection
        Dim MapColPicture As New EIDLIBCTRLLib.MapCollection
        Dim CertifCheck As New EIDLIBCTRLLib.CertifCheck

        'Set the raw structure into the virtual reader
        retStatus = eidlib.SetRawData(raw)
        If (retStatus.GetGeneral <> 0) Then
            MessageBox.Show("Bad file format.", "Error : " & retStatus.GetGeneral, MessageBoxButtons.OK, MessageBoxIcon.Error)
            eidlib.Exit()
            Return
        End If

        'Get the field Value
        retStatus = eidlib.GetID(MapColID, CertifCheck)
        If (retStatus.GetGeneral = 0) Then
            Me.txtName.Text = ConvertCodePage(MapColID.GetValue("Name"))
            Me.txtFirstName.Text = ConvertCodePage(MapColID.GetValue("FirstName1"))
        Else
            MessageBox.Show("Bad file format.", "Error : " & retStatus.GetGeneral, MessageBoxButtons.OK, MessageBoxIcon.Error)
            eidlib.Exit()
            Return
        End If

        retStatus = eidlib.GetAddress(MapColAddress, CertifCheck)
        If (retStatus.GetGeneral = 0) Then
            Me.txtStreet.Text = ConvertCodePage(MapColAddress.GetValue("Street"))
            Me.txtZip.Text = ConvertCodePage(MapColAddress.GetValue("ZIPCode"))
            Me.txtCity.Text = ConvertCodePage(MapColAddress.GetValue("Municipality"))
        Else
            MessageBox.Show("Bad file format.", "Error : " & retStatus.GetGeneral, MessageBoxButtons.OK, MessageBoxIcon.Error)
            eidlib.Exit()
            Return
        End If

        Dim picture As Byte() = Nothing
        Dim photo As Image

        retStatus = eidlib.GetPicture(MapColPicture, CertifCheck)
        If (retStatus.GetGeneral = 0) Then
            picture = MapColPicture.GetValue("Picture")

            Dim ms As System.IO.MemoryStream
            ms = New System.IO.MemoryStream()
            ms.Write(picture, 0, picture.Length)

            photo = Image.FromStream(ms, True)
            Me.imgPicture.Image = photo
        Else
            MessageBox.Show("Bad file format.", "Error : " & retStatus.GetGeneral, MessageBoxButtons.OK, MessageBoxIcon.Error)
            eidlib.Exit()
            Return
        End If

        eidlib.Exit()

    End Sub

    Private Sub ParseFile(ByVal FullCOntent As Byte(), ByRef raw As EIDLIBCTRLLib.Raw)

        Dim FileContent As Byte() = Nothing

        'File ID
        FileContent = GetTagValue(FullCOntent, 1)
        raw.SetIDData(FileContent)

        'File Sig ID
        FileContent = GetTagValue(FullCOntent, 2)
        raw.SetIDSigData(FileContent)

        'File Address
        FileContent = GetTagValue(FullCOntent, 3)
        raw.SetAddrData(FileContent)

        'File Sig Address
        FileContent = GetTagValue(FullCOntent, 4)
        raw.SetAddrSigData(FileContent)

        'File Picture
        FileContent = GetTagValue(FullCOntent, 5)
        raw.SetPictureData(FileContent)

        'File CardData
        FileContent = GetTagValue(FullCOntent, 6)
        raw.SetCardData(FileContent)

        'File TokenInfo
        FileContent = GetTagValue(FullCOntent, 7)
        raw.SetTokenInfoData(FileContent)

        'File RN
        FileContent = GetTagValue(FullCOntent, 8)
        raw.SetRNData(FileContent)

        'File Challenge
        FileContent = GetTagValue(FullCOntent, 9)
        raw.SetChallengeData(FileContent)

        'File Response
        FileContent = GetTagValue(FullCOntent, 10)
        raw.SetResponseData(FileContent)

    End Sub

    Private Function GetTagValue(ByVal FullContent As Byte(), ByVal TagID As Int32) As Byte()

        Dim result As Byte() = Nothing
        Dim bStop As Boolean = False
        Dim bFound As Boolean = False
        Dim count As Int32
        Dim i As Int32
        Dim CurrentTagID As Int32
        Dim CurrentTagLength As Int32
        count = 0

        Do
            If count >= FullContent.Length() Then
                bStop = True
            Else
                CurrentTagID = FullContent(count)
                CurrentTagLength = 0
                Do
                    count += 1
                    CurrentTagLength = CurrentTagLength * 128 + FullContent(count) Mod 128
                Loop While (FullContent(count) > 128)
            End If

            count += 1

            If TagID = CurrentTagID Then
                If count + CurrentTagLength - 1 < FullContent.Length() Then
                    bFound = True
                End If
                bStop = True
            Else
                count += CurrentTagLength
            End If

        Loop Until (bStop)

        If bFound Then
            ReDim result(CurrentTagLength - 1)
            For i = 0 To CurrentTagLength - 1
                result(i) = FullContent(count + i)
            Next
            Return result
        End If

        MessageBox.Show("Bad file format.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        Return Nothing
    End Function

    Function ConvertCodePage(ByVal strSrc As String) As String
        Const CP_UTF8 = 65001
        Const CP_WIN = 1252

        Dim bytesSrc As Byte() = System.Text.Encoding.UTF8.GetBytes(strSrc)

        'Define the encoding
        Dim encodingSrc As System.Text.Encoding = System.Text.Encoding.GetEncoding(CP_UTF8)
        Dim encodingDest As System.Text.Encoding = System.Text.Encoding.GetEncoding(CP_WIN)

        'Convert the data To destination code page/charset
        Dim bytesDest As Byte() = System.Text.Encoding.Convert(encodingSrc, encodingDest, bytesSrc)

        Dim strDest As String = System.Text.Encoding.UTF8.GetString(bytesDest)
        Return strDest

    End Function

End Class
