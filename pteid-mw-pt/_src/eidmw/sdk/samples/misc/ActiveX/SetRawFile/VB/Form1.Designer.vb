<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
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
        Me.btnRead = New System.Windows.Forms.Button
        Me.btnClose = New System.Windows.Forms.Button
        Me.dlgopenFile = New System.Windows.Forms.OpenFileDialog
        Me.txtPath = New System.Windows.Forms.TextBox
        Me.lblName = New System.Windows.Forms.Label
        Me.txtName = New System.Windows.Forms.TextBox
        Me.txtFirstName = New System.Windows.Forms.TextBox
        Me.lblFirstName = New System.Windows.Forms.Label
        Me.imgPicture = New System.Windows.Forms.PictureBox
        Me.txtStreet = New System.Windows.Forms.TextBox
        Me.lblAddress = New System.Windows.Forms.Label
        Me.txtCity = New System.Windows.Forms.TextBox
        Me.txtZip = New System.Windows.Forms.TextBox
        CType(Me.imgPicture, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(32, 8)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(80, 36)
        Me.btnRead.TabIndex = 0
        Me.btnRead.Text = "Read file"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'btnClose
        '
        Me.btnClose.Location = New System.Drawing.Point(524, 284)
        Me.btnClose.Name = "btnClose"
        Me.btnClose.Size = New System.Drawing.Size(80, 35)
        Me.btnClose.TabIndex = 1
        Me.btnClose.Text = "Close"
        Me.btnClose.UseVisualStyleBackColor = True
        '
        'dlgopenFile
        '
        Me.dlgopenFile.FileName = "OpenFileDialog1"
        '
        'txtPath
        '
        Me.txtPath.BackColor = System.Drawing.SystemColors.Window
        Me.txtPath.ForeColor = System.Drawing.SystemColors.WindowText
        Me.txtPath.Location = New System.Drawing.Point(32, 50)
        Me.txtPath.Name = "txtPath"
        Me.txtPath.ReadOnly = True
        Me.txtPath.Size = New System.Drawing.Size(572, 22)
        Me.txtPath.TabIndex = 2
        '
        'lblName
        '
        Me.lblName.AutoSize = True
        Me.lblName.Location = New System.Drawing.Point(29, 95)
        Me.lblName.Name = "lblName"
        Me.lblName.Size = New System.Drawing.Size(45, 17)
        Me.lblName.TabIndex = 4
        Me.lblName.Text = "Name"
        '
        'txtName
        '
        Me.txtName.BackColor = System.Drawing.SystemColors.Window
        Me.txtName.Location = New System.Drawing.Point(125, 92)
        Me.txtName.Name = "txtName"
        Me.txtName.ReadOnly = True
        Me.txtName.Size = New System.Drawing.Size(314, 22)
        Me.txtName.TabIndex = 5
        '
        'txtFirstName
        '
        Me.txtFirstName.BackColor = System.Drawing.SystemColors.Window
        Me.txtFirstName.Location = New System.Drawing.Point(125, 120)
        Me.txtFirstName.Name = "txtFirstName"
        Me.txtFirstName.ReadOnly = True
        Me.txtFirstName.Size = New System.Drawing.Size(314, 22)
        Me.txtFirstName.TabIndex = 7
        '
        'lblFirstName
        '
        Me.lblFirstName.AutoSize = True
        Me.lblFirstName.Location = New System.Drawing.Point(29, 123)
        Me.lblFirstName.Name = "lblFirstName"
        Me.lblFirstName.Size = New System.Drawing.Size(74, 17)
        Me.lblFirstName.TabIndex = 6
        Me.lblFirstName.Text = "First name"
        '
        'imgPicture
        '
        Me.imgPicture.Enabled = False
        Me.imgPicture.Location = New System.Drawing.Point(461, 92)
        Me.imgPicture.Name = "imgPicture"
        Me.imgPicture.Size = New System.Drawing.Size(143, 170)
        Me.imgPicture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom
        Me.imgPicture.TabIndex = 8
        Me.imgPicture.TabStop = False
        '
        'txtStreet
        '
        Me.txtStreet.BackColor = System.Drawing.SystemColors.Window
        Me.txtStreet.Location = New System.Drawing.Point(125, 148)
        Me.txtStreet.Name = "txtStreet"
        Me.txtStreet.ReadOnly = True
        Me.txtStreet.Size = New System.Drawing.Size(314, 22)
        Me.txtStreet.TabIndex = 10
        '
        'lblAddress
        '
        Me.lblAddress.AutoSize = True
        Me.lblAddress.Location = New System.Drawing.Point(29, 151)
        Me.lblAddress.Name = "lblAddress"
        Me.lblAddress.Size = New System.Drawing.Size(60, 17)
        Me.lblAddress.TabIndex = 9
        Me.lblAddress.Text = "Address"
        '
        'txtCity
        '
        Me.txtCity.BackColor = System.Drawing.SystemColors.Window
        Me.txtCity.Location = New System.Drawing.Point(216, 176)
        Me.txtCity.Name = "txtCity"
        Me.txtCity.ReadOnly = True
        Me.txtCity.Size = New System.Drawing.Size(223, 22)
        Me.txtCity.TabIndex = 12
        '
        'txtZip
        '
        Me.txtZip.AllowDrop = True
        Me.txtZip.BackColor = System.Drawing.SystemColors.Window
        Me.txtZip.Location = New System.Drawing.Point(125, 176)
        Me.txtZip.Name = "txtZip"
        Me.txtZip.ReadOnly = True
        Me.txtZip.Size = New System.Drawing.Size(85, 22)
        Me.txtZip.TabIndex = 13
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(8.0!, 16.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(643, 345)
        Me.Controls.Add(Me.txtZip)
        Me.Controls.Add(Me.txtCity)
        Me.Controls.Add(Me.txtStreet)
        Me.Controls.Add(Me.lblAddress)
        Me.Controls.Add(Me.imgPicture)
        Me.Controls.Add(Me.txtFirstName)
        Me.Controls.Add(Me.lblFirstName)
        Me.Controls.Add(Me.txtName)
        Me.Controls.Add(Me.lblName)
        Me.Controls.Add(Me.txtPath)
        Me.Controls.Add(Me.btnClose)
        Me.Controls.Add(Me.btnRead)
        Me.Name = "Form1"
        Me.Text = "Form1"
        CType(Me.imgPicture, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents btnClose As System.Windows.Forms.Button
    Friend WithEvents dlgopenFile As System.Windows.Forms.OpenFileDialog
    Friend WithEvents lblName As System.Windows.Forms.Label
    Friend WithEvents txtName As System.Windows.Forms.TextBox
    Friend WithEvents txtFirstName As System.Windows.Forms.TextBox
    Friend WithEvents lblFirstName As System.Windows.Forms.Label
    Friend WithEvents imgPicture As System.Windows.Forms.PictureBox
    Friend WithEvents txtStreet As System.Windows.Forms.TextBox
    Friend WithEvents lblAddress As System.Windows.Forms.Label
    Friend WithEvents txtCity As System.Windows.Forms.TextBox
    Friend WithEvents txtZip As System.Windows.Forms.TextBox
    Friend WithEvents txtPath As System.Windows.Forms.TextBox

End Class
