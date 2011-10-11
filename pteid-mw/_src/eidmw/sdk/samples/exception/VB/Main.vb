
Imports be.portugal.eid

Public Class frmMain

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOk.Click

        Me.Close()
        PTEID_ReaderSet.releaseSDK()

        Application.Exit()

    End Sub

    Private Sub btnTest_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTest.Click

        Me.txtInfo.Text = ""
        Me.txtInfo.Text &= "eID SDK sample program: get_exception" & vbCrLf
        Me.txtInfo.Text &= "" & vbCrLf

        Dim readerName As String = "test"
        Dim reader As PTEID_ReaderContext
        Dim card As PTEID_SISCard
        Dim doc As PTEID_XMLDoc
        Dim docId As PTEID_SisId

        Try
            reader = PTEID_ReaderSet.instance().getReaderByName(readerName)

        Catch ex As PTEID_ExParamRange
            Me.txtInfo.Text &= "SUCCESS => The reader named " & readerName & " does not exist" & vbCrLf

        Catch ex As PTEID_Exception
            Me.txtInfo.Text &= "FAILS => Other PTEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            reader = PTEID_ReaderSet.instance().getReader()
            Me.txtInfo.Text &= "SUCCESS => Reader found" & vbCrLf

        Catch ex As PTEID_ExNoReader
            Me.txtInfo.Text &= "FAILS => No reader found" & vbCrLf
            Exit Sub

        Catch ex As PTEID_Exception
            Me.txtInfo.Text &= "FAILS => Other PTEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            card = reader.getSISCard()
            Me.txtInfo.Text &= "SUCCESS => SIS card found" & vbCrLf

        Catch ex As PTEID_ExNoCardPresent
            Me.txtInfo.Text &= "FAILS => No card found" & vbCrLf
            Exit Sub

        Catch ex As PTEID_ExCardBadType
            Me.txtInfo.Text &= "FAILS => This is not a SIS card" & vbCrLf
            Exit Sub

        Catch ex As PTEID_Exception
            Me.txtInfo.Text &= "FAILS => Other PTEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            doc = card.getDocument(PTEID_DocumentType.PTEID_DOCTYPE_PICTURE)

        Catch ex As PTEID_ExDocTypeUnknown
            Me.txtInfo.Text &= "SUCCESS => No picture on this card" & vbCrLf

        Catch ex As PTEID_Exception
            Me.txtInfo.Text &= "FAILS => Other PTEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

        Try
            docId = card.getID()
            Me.txtInfo.Text &= "SUCCESS => Your name is " & docId.getName() & "" & vbCrLf

        Catch ex As PTEID_Exception
            Me.txtInfo.Text &= "FAILS => Other PTEID_Exception (code = " & ex.GetError().ToString("x") & ")" & vbCrLf
            Exit Sub

        Catch
            Me.txtInfo.Text &= "FAILS => Other exception" & vbCrLf
            Exit Sub

        End Try

    End Sub


End Class
