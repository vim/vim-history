		    SendToVim & OpenWithVim, version 1.1

* Description

  SendToVim and OpenWithVim are two little tools for the better embedding of
Vim into Windows.

  Both programs look, if there exists an already running instance of vim and
send files or commands to it.  If vim isn't running, it will be started at
first.  Thus you only have one instance of vim running throughout your
session.  The programs are inspired by the emacsclient program used for
emacs.

  SendToVim takes the command line arguments (with or without ") and sends
them to vim.  With an additional switch you can tell vim to change its
working directory to the directory SendToVim was envoked.

Examples:
edit MyFile with vim:
  SendToVim :e MyFile

open new window with MyFile and maximize it:
SendToVim ":split MyFile | resize"

tell vim to insert contents of register a
  SendToVim "\"ap"

search for "set" and highlight it
  SendToVim "/set<c-m>:se hls"

ask vim about its working directory
  SendToVim :pwd
tell vim to change its working directory to current directory and display it
  SendToVim +cd :pwd

  OpenWithVim opens a given file with vim for viewing/editing. Using the
option +<line> you can tell vim to jump to line number <line> of the
specified file.  When opening a file using OpenWithVim, vim always changes
its working directory to the current directory at first unless an absolute
path was specified.

Examples:
edit MyFile with vim:
  OpenWithVim MyFile

edit MyFile with vim and jump to line number 20
  OpenWithVim +20 MyFile

edit c:\MyFile and leave vim's working directory unchanged
  OpenWithVim c:\MyFile

edit MyFile and restore vim's window size (should be done automatic by
SetForeground(), so this is a workaround):
  openWithVim -rr MyFile

Note that the switch -r<char> is language dependend!  The key <char> is used
to build the command :simalt ~<char> (see :h :simalt for details)


Using OpenWithVim with explorer:
(1) Send To menu:
  create a link of OpenWithVim.exe and put it in Windows\SendTo directory
  => right mouse-click on file->SendTo->OpenWithVim opens that file with Vim
(2) file association:
  start explorer, menu view->options, tab types
  select file extension, choose edit
  edit entry "open"
  change application to $PATH_OF_OPENWITHVIM\OpenWithVim -rr "%1"
  => double clicking a file with that extension will be open with Vim

* Requires

  GVim 5.3 with OLE support

* Author

  Christian Schaller (sca@isogmbh.de)

* TODO

  How can I cange the program icon?

* Changes:
  OpenWithVim without parameter starts empty Vim (no message box with help,
  use OpenWithVim -h instead).  Due to an OLE problem, Vim isn't resized, if
  minimized, so you can add -r<char> to use :simalt ~<char> for restoring
  Vim's size (this is language dependent, use -rr for US, -rw for German
  version of Windows), thanks to Rajesh Kallingal.

vim: tw=76
