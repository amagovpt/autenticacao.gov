
Imports be.belgium.eid

Public Class frmMain

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOk.Click

        Me.Close()
        BEID_ReaderSet.releaseSDK()

        Application.Exit()

    End Sub

    Private Sub btnTest_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTest.Click

        Me.txtInfo.Text = ""
        Me.txtInfo.Text &= "eID SDK sample program: get_exception" & vbCrLf
        Me.txtInfo.Text &= "" & vbCrLf

        Dim readerName As String = "test"
        Dim reader As BEID_ReaderContext
        Dim card As BEID_SISCard
        Dim doc As BEID_XMLDoc
        Dim docId As BEID_SisId

        Try
            reader = BEID_ReaderSet.instance().getReaderByName(readerName)

        Catch ex As BEID_ExParamRange
            Me.txtInfo.Text &= "SUCCESS => The reader named " & readerName & " does not exist" & vbCrLf

        Catch ex As BEID_Exception
            Me.txtInfo.Text &= "FAILS => Other BEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            reader = BEID_ReaderSet.instance().getReader()
            Me.txtInfo.Text &= "SUCCESS => Reader found" & vbCrLf

        Catch ex As BEID_ExNoReader
            Me.txtInfo.Text &= "FAILS => No reader found" & vbCrLf
            Exit Sub

        Catch ex As BEID_Exception
            Me.txtInfo.Text &= "FAILS => Other BEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            card = reader.getSISCard()
            Me.txtInfo.Text &= "SUCCESS => SIS card found" & vbCrLf

        Catch ex As BEID_ExNoCardPresent
            Me.txtInfo.Text &= "FAILS => No card found" & vbCrLf
            Exit Sub

        Catch ex As BEID_ExCardBadType
            Me.txtInfo.Text &= "FAILS => This is not a SIS card" & vbCrLf
            Exit Sub

        Catch ex As BEID_Exception
            Me.txtInfo.Text &= "FAILS => Other BEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            doc = card.getDocument(BEID_DocumentType.BEID_DOCTYPE_PICTURE)

        Catch ex As BEID_ExDocTypeUnknown
            Me.txtInfo.Text &= "SUCCESS => No picture on this card" & vbCrLf

        Catch ex As BEID_Exception
            Me.txtInfo.Text &= "FAILS => Other BEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            docId = card.getID()
            Me.txtInfo.Text &= "SUCCESS => Your name is " & docId.getName() & "" & vbCrLf

        Catch ex As BEID_Exception
            Me.txtInfo.Text &= "FAILS => Other BEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

    End Sub


End Class
