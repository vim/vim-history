Attribute VB_Name = "MainModule"
' OpenWithVim, version 1.3

' Author: Christian Schaller (Christian.Schaller@mchp.siemens.de)

' Description: opens given file with Vim (if exists, otherwise Vim is
' started first) and jumps to given line number, if supplied.
' Vim opens that file regardless of the mode vim is currently in.

' Requires: GVim 5.3 or greater with OLE support

' Examples:
' edit MyFile with vim:
'   OpenWithVim MyFile
' edit MyFile with vim and jump to line number 20
'   OpenWithVim +20 MyFile
' edit MyFile and restore vim's window size (should be done automatic by
' SetForeground(), so this is a workaround):
'   openWithVim -rr MyFile
' Note that the switch -r<char> is language dependend!  The key <char> is used
' to build the command :simalt ~<char> (see :h :simalt for details)

' Using OpenWithVim with explorer:
' (1) Send To:
'   create a link of OpenWithVim.exe and put it in Windows\SendTo directory
'   => right mouse-click on file->SendTo->OpenWithVim opens that file with Vim
' (2) file association:
'   start explorer, menu view->options, tab types
'   select file extension, choose edit
'   edit entry "open"
'   change application to $PATH_OF_OPENWITHVIM\OpenWithVim -rr "%1"
'   => double clicking a file with that extension will be open with Vim

'TODO: How can I cange the program icon?

Option Explicit

Sub Main()
    Dim usage As String
    Dim commandline As String
    Dim path As String
    Dim vim As Object
    Dim line As String
    Dim file As String
    Dim start As Integer
    Dim param As String
    Dim restoreCmd As String
    
    line = ""
    file = ""
    start = 0
    
    usage = "OpenWithVim 1.3 (13.XII.99)" _
        & vbCrLf _
        & vbCrLf & "Usage: OpenWithVim [-r<char>] [+<line>] [<file>]" _
        & vbCrLf _
        & vbCrLf & "Options:" _
        & vbCrLf & "+<line>" & vbTab & "goto line number <line>" _
        & vbCrLf & "-r<char>" & vbTab & "send the language dependent shortcut for" _
        & vbCrLf & vbTab & "restoring vim's window (US: -rr, German: -rw)" _
        & vbCrLf & "-h" & vbTab & "display this dialog"
        
    'get command line
    commandline = Command()
    
    'switches starting with -
    If Left(commandline, 1) = "-" Then
        start = InStr(commandline, " ")
        If start = 0 Then
            start = Len(commandline)
        End If
        param = Trim(Mid(commandline, 2, start - 1))
        If Left(param, 1) = "r" Then
            restoreCmd = Mid(param, 2)
            If restoreCmd = "" Then
                MsgBox usage
                Exit Sub
            End If
        Else
            MsgBox usage
            Exit Sub
        End If
        'remove switch from commandline
        commandline = Trim(Mid(commandline, start + 1))
    End If
    
    'is next parameter starting with +?
    If Left(commandline, 1) = "+" Then
        start = InStr(commandline, " ")
        If start > 0 Then
            line = Trim(Mid(commandline, 2, start - 1))
            file = Trim(Mid(commandline, start + 1))
        End If
        'no line number?
        If line = "" Or file = "" Then
            MsgBox usage
            Exit Sub
        End If
    Else
        file = Trim(commandline)
    End If
    
    'remove "s
    If Left(file, 1) = Chr(34) Then file = Mid(file, 2, Len(file) - 2)
    
    'get current path
    path = CurDir()
 
    'Dim dbgMsg As String
 
    Set vim = CreateObject("Vim.Application")
    'first, make sure, vim is in normal mode
    vim.SendKeys("<c-\><c-n>")

    'if file name is relative, then first change to that directory
    If Left(file, 1) <> "\" And Left(file, 1) <> "/" _
        And Mid(file, 2, 1) <> ":" Then
        'dbgMsg = "Sending :cd " & path & "<c-m>"
        vim.SendKeys (":cd " & path & "<c-m> ")
    End If
    If file <> "" Then
        'dbgMsg = dbgMsg & ":e " & file & "<c-m>"
        vim.SendKeys (":e " & file & "<c-m>")
    End If
    If line <> "" Then vim.SendKeys (":" & line & "<c-m>")

    vim.SetForeground
    If restoreCmd <> "" Then
        vim.SendKeys (":simalt ~" & restoreCmd & "<c-m>")
    End If
    'MsgBox dbgMsg
End Sub
