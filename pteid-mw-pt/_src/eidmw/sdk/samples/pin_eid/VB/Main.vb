
Imports be.portugal.eid

Public Class frmMain

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOk.Click

        Me.Close()
        PTEID_ReaderSet.releaseSDK()

        Application.Exit()

    End Sub

    Private Sub btnVerify_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnVerify.Click

        Try
            Dim ReaderSet As PTEID_ReaderSet
            ReaderSet = PTEID_ReaderSet.instance()

            Dim Reader As PTEID_ReaderContext
            Reader = ReaderSet.getReader()

            If Reader.isCardPresent() Then
                If Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_EID _
                    Or Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_FOREIGNER _
                    Or Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_KIDS Then

                    Dim lRemaining As Int32
                    If (Reader.getEIDCard().getPins().getPinByNumber(0).verifyPin("", lRemaining)) Then
                        MessageBox.Show("Verification succeeded", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information)
                    Else
                        MessageBox.Show("Verification failed" & vbCrLf & "Tries left = " & lRemaining, "Information", MessageBoxButtons.OK, MessageBoxIcon.Information)
                    End If

                Else
                    MessageBox.Show("The card doesn't support this feature", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                End If
            Else
                MessageBox.Show("Please insert a card in the reader", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            End If

            PTEID_ReaderSet.releaseSDK()

        Catch ex As PTEID_Exception
            Debug.Print("Crash PTEID_Exception!")
            PTEID_ReaderSet.releaseSDK()
        Catch ex As Exception
            Debug.Print("Crash System.Exception!")
            PTEID_ReaderSet.releaseSDK()
        End Try

    End Sub


    Private Sub btnChange_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnChange.Click
        Try
            Dim ReaderSet As PTEID_ReaderSet
            ReaderSet = PTEID_ReaderSet.instance()

            Dim Reader As PTEID_ReaderContext
            Reader = ReaderSet.getReader()

            If Reader.isCardPresent() Then
                If Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_EID _
                    Or Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_FOREIGNER _
                    Or Reader.getCardType() = PTEID_CardType.PTEID_CARDTYPE_KIDS Then

                    Dim lRemaining As Int32
                    If (Reader.getEIDCard().getPins().getPinByNumber(0).changePin("", "", lRemaining)) Then
                        MessageBox.Show("Change succeeded", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information)
                    Else
                        MessageBox.Show("Change failed" & vbCrLf & "Tries left = " & lRemaining, "Information", MessageBoxButtons.OK, MessageBoxIcon.Information)
                    End If

                Else
                    MessageBox.Show("The card doesn't support this feature", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                End If
            Else
                MessageBox.Show("Please insert a card in the reader", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning)
            End If

            PTEID_ReaderSet.releaseSDK()

        Catch ex As PTEID_Exception
            Debug.Print("Crash PTEID_Exception!")
            PTEID_ReaderSet.releaseSDK()
        Catch ex As Exception
            Debug.Print("Crash System.Exception!")
            PTEID_ReaderSet.releaseSDK()
        End Try

    End Sub
End Class
