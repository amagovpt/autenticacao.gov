'************************************************
'
' DOC2PDF.VBS Microsoft Scripting Host Script (Requires Version 5.6 or newer)
' --------------------------------------------------------------------------------
'
' Author: Michael Suodenjoki
' Created: 2007.07.07
'
' This script can create a PDF file from a Word document provided you're using
' Word 2007 and have the 'Office Add-in: Save As PDF' installed.
'

' Constants
Const WdDoNotSaveChanges = 0

' see WdSaveFormat enumeration constants: 
' http://msdn2.microsoft.com/en-us/library/bb238158.aspx
Const wdFormatPDF = 17  ' PDF format. 
Const wdFormatXPS = 18  ' XPS format. 


' Global variables
Dim arguments
Set arguments = WScript.Arguments

' ***********************************************
' ECHOLOGO
'
' Outputs the logo information.
'
Function EchoLogo()
  If Not (arguments.Named.Exists("nologo") Or arguments.Named.Exists("n")) Then
    WScript.Echo "doc2pdf Version 2.0, Michael Suodenjoki 2007"
    WScript.Echo "=================================================="
    WScript.Echo ""
  End If
End Function

' ***********************************************
' ECHOUSAGE
'
' Outputs the usage information.
'
Function EchoUsage()
  If arguments.Count=0 Or arguments.Named.Exists("help") Or _
    arguments.Named.Exists("h") _
  Then
    WScript.Echo "Generates a PDF from a Word document file using Word 2007."
    WScript.Echo ""
    WScript.Echo "Usage: doc2pdf.vbs <options> <doc-file> [/o:<pdf-file>]"
    WScript.Echo ""
    WScript.Echo "Available Options:"
    WScript.Echo ""
    WScript.Echo " /nologo - Specifies that the logo shouldn't be displayed"
    WScript.Echo " /help   - Specifies that this usage/help information " + _
                 "should be displayed."
    WScript.Echo " /debug  - Specifies that debug output should be displayed."
    WScript.Echo ""
    WScript.Echo "Parameters:"
    WScript.Echo ""
    WScript.Echo " /o:<pdf-file> Optionally specification of output file (PDF)."
    WScript.Echo ""
  End If 
End Function

' ***********************************************
' CHECKARGS
'
' Makes some preliminary checks of the arguments.
' Quits the application is any problem is found.
'
Function CheckArgs()
  ' Check that <doc-file> is specified
  If arguments.Unnamed.Count <> 1 Then
    WScript.Echo "Error: Obligatory <doc-file> parameter missing!"
    WScript.Quit 1
  End If

  bShowDebug = arguments.Named.Exists("debug") Or arguments.Named.Exists("d")

End Function


' ***********************************************
' DOC2PDF
'
' Converts a Word document to PDF using Word 2007.
'
' Input:
' sDocFile - Full path to Word document.
' sPDFFile - Optional full path to output file.
'
' If not specified the output PDF file
' will be the same as the sDocFile except
' file extension will be .pdf.
'
Function DOC2PDF( sDocFile, sPDFFile )

  Dim fso ' As FileSystemObject
  Dim wdo ' As Word.Application
  Dim wdoc ' As Word.Document
  Dim wdocs ' As Word.Documents
  Dim sPrevPrinter ' As String

  Set fso = CreateObject("Scripting.FileSystemObject")
  Set wdo = CreateObject("Word.Application")
  Set wdocs = wdo.Documents

  sDocFile = fso.GetAbsolutePathName(sDocFile)

  ' Debug outputs...
  If bShowDebug Then
    WScript.Echo "Doc file = '" + sDocFile + "'"
    WScript.Echo "PDF file = '" + sPDFFile + "'"
  End If

  sFolder = fso.GetParentFolderName(sDocFile)

  If Len(sPDFFile)=0 Then
    sPDFFile = fso.GetBaseName(sDocFile) + ".pdf"
  End If

  If Len(fso.GetParentFolderName(sPDFFile))=0 Then
    sPDFFile = sFolder + "\" + sPDFFile
  End If

  ' Enable this line if you want to disable autoexecute macros
  ' wdo.WordBasic.DisableAutoMacros

  ' Open the Word document
  Set wdoc = wdocs.Open(sDocFile)

  ' Let Word document save as PDF
  ' - for documentation of SaveAs() method,
  '   see http://msdn2.microsoft.com/en-us/library/bb221597.aspx 
  wdoc.SaveAs sPDFFile, wdFormatPDF

  wdoc.Close WdDoNotSaveChanges
  wdo.Quit WdDoNotSaveChanges
  Set wdo = Nothing

  Set fso = Nothing

End Function

' *** MAIN **************************************

Call EchoLogo()
Call EchoUsage()
Call CheckArgs()
Call DOC2PDF( arguments.Unnamed.Item(0), arguments.Named.Item("o") )

Set arguments = Nothing