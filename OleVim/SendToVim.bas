Attribute VB_Name = "MainModule"
' SendToVim, version 1.1

' Author: Christian Schaller (Christian.Schaller@mchp.siemens.de)

' Description: sends given arguments (with or without ")
' to Vim (if exists, otherwise Vim is started at first), to have only one
' instance of Vim running.

' Requires: GVim 5.3 or greater with OLE support

' Examples:
' edit MyFile with vim:
'   SendToVim :e MyFile
' open new window with MyFile and maximize it:
' SendToVim ":split MyFile | resize"
' tell vim to insert contents of register a
'   SendToVim "\"ap"
' search for "set" and highlight it
'   SendToVim "/set<c-m>:se hls"

' Note that the working directory of Vim isn't changed without the +cd switch
' Example:
' change to a different directory than Vim's
'   SendToVim :pwd
'   => Vim responds with its current working dir
'   SendToVim +cd :pwd
'   => Vim first changes to current directory and then executes :pwd

' Note: Vim is interpreting the sent commands depending on the mode it is in.
'       If you want to make sure, vim is in normal mode, send <c-\><c-n> at
'       first.

'TODO: How can I cange the program icon?

Option Explicit

Sub Main()
    Dim usage As String
    Dim length As Integer
    Dim commandline As Variant
    Dim path As String
    Dim cd As Boolean
    Dim start As Integer
    Dim switch As String
    Dim cmd As String
    
    usage = "SendToVim 1.1 (22. III. 99)" _
        & vbCrLf _
        & vbCrLf & "Usage: SendToVim [+cd] <vim command>" _
        & vbCrLf _
        & vbCrLf & "Options:" _
        & vbCrLf & "+cd" & vbTab & "change to current directory before executing command"
        
    cd = False
    
    'get command line
    commandline = command()
    length = Len(commandline)
    
    'no arguments?
    If length = 0 Then
        MsgBox usage
        Exit Sub
    End If
    
    'leading +?
    If Left(commandline, 1) = "+" Then
        'remove +
        commandline = Mid(commandline, 2)
        start = InStr(commandline, " ")
        'get switch
        switch = Trim(Mid(commandline, 1, start))
        ' get command (without leading and trailing spaces)
        cmd = Trim(Mid(commandline, start + 1))
        If switch = "" Or switch <> "cd" Or cmd = "" Then
            MsgBox usage
            Exit Sub
        End If
        If switch = "cd" Then cd = True
    Else
        'no leading +, just command
        cmd = Trim(commandline)
    End If
    
    'check for leading "
    If Left(cmd, 1) = Chr(34) Then
        'if so, remove the last charakter (")
        cmd = Mid(cmd, 2, length - 2)
    End If
    
    'get current path
    path = CurDir()
    
    Dim vim As Object
    Set vim = CreateObject("Vim.Application")
    If cd Then vim.SendKeys (":cd " & path & "<c-m>")
    vim.SendKeys (cmd & "<c-m>")
    vim.SetForeground
End Sub
