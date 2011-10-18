Imports System.Runtime.InteropServices
Imports System.Threading

Imports be.belgium.eid


Public Class frmMain

    <DllImport("Kernel32.dll")> _
    Public Shared Function Beep(ByVal dwFreq As UInteger, ByVal dwDuration As UInteger) As UInteger
    End Function


    Dim m_ReaderSet As BEID_ReaderSet
    Dim m_bShowButton As Boolean
    Dim m_sMessage As String

    Dim MyReadersSet As System.Collections.Hashtable = New System.Collections.Hashtable

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnQuit.Click

        Try
            Me.Close()

            'Never forget to release the sdk before leaving
            BEID_ReaderSet.releaseSDK()

            Application.Exit()

        Catch ex As BEID_Exception
            MessageBox.Show("Crash BEID_Exception : " & ex.GetError().ToString())
            BEID_ReaderSet.releaseSDK()
            Application.Exit()
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
            BEID_ReaderSet.releaseSDK()
            Application.Exit()
        End Try
    End Sub

    Private Sub RefreshMessage()

        'The control properties must be access in the thread that create them
        If (Me.lblMessage.InvokeRequired) Then
            'If the calling thread is not the creator thread,
            'we need to call it using BeginInvoke
            Me.BeginInvoke(New MethodInvoker(AddressOf Me.RefreshMessage))

        Else
            Me.btnTest.Visible = m_bShowButton
            Me.btnQuit.Visible = m_bShowButton
            Me.lblMessage.Text = m_sMessage
        End If
    End Sub

    Private Sub btnTest_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTest.Click

        Try
            'We start the RunThread function in an other thread
            Dim th As Thread = New Thread(New ThreadStart(AddressOf RunThread))
            th.Start()

        Catch ex As BEID_Exception
            MessageBox.Show("Crash BEID_Exception : " & ex.GetError().ToString())
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

    End Sub


    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Try
            m_ReaderSet = BEID_ReaderSet.instance()

            m_bShowButton = True
            m_sMessage = "Click on the 'Start' button and  follow the instruction"


        Catch ex As BEID_Exception
            MessageBox.Show("Crash BEID_Exception : " & ex.GetError().ToString())
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try
    End Sub

    Private Sub RunThread()
        Try
            Dim sName As String = ""

            m_bShowButton = False
            m_sMessage = "Please insert your card"
            RefreshMessage()

            'Get the reader in which a card is inserted (timeout = 5 sec)
            Dim reader As BEID_ReaderContext = WaitForCardPresent(5)
            If (reader Is Nothing) Then
                m_sMessage = "You haven't inserted a card. Click on 'start' to try again."
                RefreshMessage()

            Else
                'Get the card (if it's not an eid card, an exception is thrown)
                Dim card As BEID_EIDCard = reader.getEIDCard()
                card.setAllowTestCard(True)

                'Get data from the card
                sName = card.getID().getFirstName() + " " + card.getID().getSurname()
                m_sMessage = sName + ", please remove your card."
                RefreshMessage()

                'Wait until the card has been remove (timeout = 5 sec)
                If (Not WaitForCardAbsent(5)) Then

                    'Remind the user to remove his card with a beep
                    While (Not WaitForCardAbsent(1))
                        m_sMessage = sName + ", don't forget to remove your card!"
                        RefreshMessage()
                        Beep(150, 150)
                    End While

                End If

                m_sMessage = "Click on the 'Start' button and  follow the instruction"
                RefreshMessage()
            End If
            m_bShowButton = True
            RefreshMessage()

        Catch ex As BEID_Exception
            MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString())
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try
    End Sub

    '*****************************************************************************
    ' WaitForCardPresent function
    ' This function hangs until a card has been inserted into the reader
    ' Or for a maximum of Timeout seconds
    ' It returns the reader context or Nothing if a timeout occured
    '*****************************************************************************
    Private Function WaitForCardPresent(ByVal Timeout As Integer) As BEID_ReaderContext
        Try
            Dim reader As BEID_ReaderContext = Nothing
            Dim bContinue As Boolean = True
            Dim Count As Integer = 0

            While (bContinue)

                'Get a reader. getReader (without parameter) returns the first reader with a card.
                'or the first reader (if no card is present)
                'if no reader is connected, an BEID_ExNoReader exception is thrown
                reader = m_ReaderSet.getReader()

                'If a card is present, we can leave the loop
                If (reader.isCardPresent()) Then
                    bContinue = False

                    'If timeout occured, we leave the loop and prepare the exception
                ElseIf (Count > Timeout) Then
                    bContinue = False
                    reader = Nothing

                    'Else we sleep 1 second and check again
                Else
                    Thread.Sleep(1000)
                    Count += 1
                End If

            End While
            Return reader

        Catch ex As BEID_ExNoReader
            MessageBox.Show("No reader connected")
        Catch ex As BEID_Exception
            MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString())
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

        Return Nothing
    End Function

    '*****************************************************************************
    ' WaitForCardAbsent Function
    ' This function hangs until no more card is pluged in the reader
    ' Or for a maximum of Timeout seconds
    ' It returns true if the no card is inserted and false if timeout occured
    '*****************************************************************************
    Private Function WaitForCardAbsent(ByVal Timeout As Integer) As Boolean
        Try
            Dim reader As BEID_ReaderContext = Nothing
            Dim Count As Integer = 0

            While (True)
                'Get a reader. getReader (without parameter) return the first reader with a card.
                'or the first reader (if no card is present)
                'if no reader is connected, an BEID_ExNoReader exception is thrown
                reader = m_ReaderSet.getReader()

                'If no card is present, we return true
                If (Not reader.isCardPresent()) Then
                    Return True

                    'If timeout occured, we return false
                ElseIf (Count > Timeout) Then
                    Return False

                    'Else we sleep 1 second and check again
                Else
                    Thread.Sleep(1000)
                    Count += 1
                End If
            End While

        Catch ex As BEID_ExNoReader
            MessageBox.Show("No reader connected")
        Catch ex As BEID_Exception
            MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString())
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

        Return False
    End Function

End Class
