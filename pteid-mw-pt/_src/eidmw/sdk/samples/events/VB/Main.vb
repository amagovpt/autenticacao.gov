
Imports be.portugal.eid

Public Class frmMain

    Dim ReaderSet As PTEID_ReaderSet

    Private Class ReaderRef
        Public reader As PTEID_ReaderContext
        Public eventHandle As UInteger
        Public ptr As IntPtr
        Public cardId As UInteger
    End Class

    Dim MyReadersSet As System.Collections.Hashtable = New System.Collections.Hashtable

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOk.Click

        Try
            DetachEvents()

            MessageBox.Show("Events have been removed")

            Me.Close()
            PTEID_ReaderSet.releaseSDK()

            Application.Exit()

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
            PTEID_ReaderSet.releaseSDK()
            Application.Exit()
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
            PTEID_ReaderSet.releaseSDK()
            Application.Exit()
        End Try
    End Sub

    Private Sub btnTest_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTest.Click

        Try
            Dim strList As String = ""
            Dim strCard As String = ""

            Dim reader As PTEID_ReaderContext
            Dim ref As ReaderRef

            For Each ref In MyReadersSet.Values
                reader = ref.reader
                If reader.isCardPresent() Then
                    strCard = "Card present"
                Else
                    strCard = "No card"
                End If
                strList &= reader.getName() & " : " & strCard & vbCrLf
            Next

            MessageBox.Show(strList)

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

    End Sub


    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Try
            ReaderSet = PTEID_ReaderSet.instance()

            AttacheEvents()
            MessageBox.Show("An event has been set on each reader")

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try
    End Sub

    Private Sub AttacheEvents()
        Try
            Dim reader As PTEID_ReaderContext
            Dim ref As ReaderRef
            Dim i As Integer

            Dim MyCallback As PTEID_SetEventDelegate
            MyCallback = New PTEID_SetEventDelegate(AddressOf CallBack)

            Dim readerName As String

            For i = 0 To ReaderSet.readerCount() - 1
                reader = ReaderSet.getReaderByNum(i)
                readerName = ReaderSet.getReaderName(i)

                ref = New ReaderRef

                ref.reader = reader
                ref.ptr = System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(readerName)
                ref.cardId = 0
                MyReadersSet.Add(readerName, ref)
                ref.eventHandle = reader.SetEventCallback(MyCallback, ref.ptr)
            Next

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

    End Sub

    Private Sub DetachEvents()
        Try
            Dim reader As PTEID_ReaderContext
            Dim ref As ReaderRef

            For Each ref In MyReadersSet.Values
                reader = ref.reader

                reader.StopEventCallback(ref.eventHandle)
            Next
            MyReadersSet.Clear()

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

    End Sub

    Public Sub CallBack(ByVal lRet As Integer, ByVal lState As UInteger, ByVal p As System.IntPtr)

        Try
            Dim action As String = ""
            Dim readerName As String
            Dim ref As ReaderRef
            Dim bChange As Boolean

            readerName = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(p)
            ref = MyReadersSet(readerName)

            bChange = False

            If ref.reader.isCardPresent() Then
                If ref.reader.isCardChanged(ref.cardId) Then
                    action = "inserted in"
                    bChange = True
                End If
            Else
                If ref.cardId <> 0 Then
                    action = "removed from"
                    bChange = True
                End If
            End If

            If bChange Then
                MessageBox.Show("A card has been " & action & " the reader : " & readerName)
            End If

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try

    End Sub

    Private Sub ctrTimer_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles ctrTimer.Tick
        Try
            Dim count As UInteger

            If ReaderSet.isReadersChanged() Then
                DetachEvents()
                count = ReaderSet.readerCount(True) 'Force the read of reader list
                AttacheEvents()

                MessageBox.Show("Readers has been plugged/unplugged" & vbCrLf & "Number of readers : " & count)
            End If

        Catch ex As PTEID_Exception
            MessageBox.Show("Crash PTEID_Exception!")
        Catch ex As Exception
            MessageBox.Show("Crash System.Exception!")
        End Try
    End Sub
End Class
