<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.btnOk = New System.Windows.Forms.Button
        Me.btnLoad = New System.Windows.Forms.Button
        Me.txtInfo = New System.Windows.Forms.TextBox
        Me.img = New System.Windows.Forms.PictureBox
        CType(Me.img, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnOk
        '
        Me.btnOk.Location = New System.Drawing.Point(572, 362)
        Me.btnOk.Name = "btnOk"
        Me.btnOk.Size = New System.Drawing.Size(75, 26)
        Me.btnOk.TabIndex = 0
        Me.btnOk.Text = "Quit"
        Me.btnOk.UseVisualStyleBackColor = True
        '
        'btnLoad
        '
        Me.btnLoad.Location = New System.Drawing.Point(44, 362)
        Me.btnLoad.Name = "btnLoad"
        Me.btnLoad.Size = New System.Drawing.Size(96, 26)
        Me.btnLoad.TabIndex = 1
        Me.btnLoad.Text = "Load card"
        Me.btnLoad.UseVisualStyleBackColor = True
        '
        'txtInfo
        '
        Me.txtInfo.Location = New System.Drawing.Point(44, 24)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.txtInfo.Size = New System.Drawing.Size(454, 302)
        Me.txtInfo.TabIndex = 2
        '
        'img
        '
        Me.img.Enabled = False
        Me.img.Location = New System.Drawing.Point(504, 24)
        Me.img.Name = "img"
        Me.img.Size = New System.Drawing.Size(143, 177)
        Me.img.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom
        Me.img.TabIndex = 4
        Me.img.TabStop = False
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(8.0!, 16.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(700, 442)
        Me.Controls.Add(Me.img)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.btnLoad)
        Me.Controls.Add(Me.btnOk)
        Me.Name = "frmMain"
        Me.Text = "Main"
        CType(Me.img, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnOk As System.Windows.Forms.Button
    Friend WithEvents btnLoad As System.Windows.Forms.Button
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents img As System.Windows.Forms.PictureBox

End Class
