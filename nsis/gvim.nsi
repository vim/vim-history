# NSIS file to create a self-installing exe for Vim.
# It needs NSIS version 1.80 or later.
# Last modification:	2001 Oct 31

# WARNING: if you make changes to this script, look out for $0 to be valid,
# because this line is very dangerous:		RMDir /r $0

# comment next line if you don't have UPX.
# Get it at http://upx.sourceforge.net
!define HAVE_UPX

# comment the next line if you do not want to add Native Language Support
!define HAVE_NLS

!define VER_MINOR 1
!define VER_MAJOR 6

Name "Vim ${VER_MAJOR}.${VER_MINOR}"
OutFile gvim${VER_MAJOR}${VER_MINOR}.exe
CRCCheck on
ComponentText "This will install Vim ${VER_MAJOR}.${VER_MINOR} on your computer."
DirText "Choose a directory to install Vim (must end in 'vim')"
SetDatablockOptimize on
Icon icons\vim_16c.ico
EnabledBitmap icons\enabled.bmp
DisabledBitmap icons\disabled.bmp
UninstallText "This will uninstall Vim ${VER_MAJOR}.${VER_MINOR} from your system."
UninstallIcon icons\vim_uninst_16c.ico
BGGradient 004000 008200 ffffff
LicenseText "You should read the following before installing:"
LicenseData ..\doc\uganda.nsis.txt

!ifdef HAVE_UPX
  !packhdr temp.dat "upx --best --compress-icons=1 temp.dat"
!endif
# This add '\vim' to the user choice automagically.
InstallDir "C:\vim"

# Types of installs we can perform:
InstType Typical
InstType Minimal
InstType Full

SilentInstall normal

##########################################################
# Functions

Function .onInit
  MessageBox MB_YESNO|MB_ICONQUESTION \
	"This will install Vim ${VER_MAJOR}.${VER_MINOR} on your computer.$\n Continue?" \
	IDYES NoAbort

  Abort ; causes installer to quit.
  NoAbort:

  # run the install program to check for already installed versions
  SetOutPath $TEMP
  File ..\src\install.exe
  ExecWait "$TEMP\install.exe -uninstall-check"
  Delete $TEMP\install.exe

  # We may have been put to the background when uninstall did something.
  BringToFront

  # Install will have created a file for us that contains the directory where
  # we should install.  This is $VIM if it's set.  This appears to be the only
  # way to get the value of $VIM here!?
  ReadINIStr $INSTDIR $TEMP\vimini.ini vimini dir
  Delete $TEMP\vimini.ini

  # If ReadINIStr failed for some reason, use default dir.
  StrCmp $INSTDIR "" 0 IniOK
    StrCpy $INSTDIR "C:\vim"
  IniOK:

  # Should check for the value of $VIM and use it.  Unfortunately I don't know
  # how to obtain the value of $VIM
  # IfFileExists "$VIM" 0 No_Vim
  #   StrCpy $INSTDIR "$VIM"
  # No_Vim:

# User variables:
# $0 - holds the directory the executables are installed to
# $1 - holds the parameters to be passed to install.exe.  Starts with OLE
#      registration (since a non-OLE gvim will not complain, and we want to
#      always register an OLE gvim).
  StrCpy $0 "$INSTDIR\vim${VER_MAJOR}${VER_MINOR}"
  StrCpy $1 "-register-OLE"

FunctionEnd

Function .onUserAbort
  MessageBox MB_YESNO|MB_ICONQUESTION "Abort install?" IDYES NoCancelAbort
    Abort ; causes installer to not quit.
  NoCancelAbort:
FunctionEnd

# Only enable the "Install" button if the install directory ends in "vim".
Function .onVerifyInstDir
  StrCpy $0 $INSTDIR 3 -3
  StrCmp $0 "vim" PathGood
    Abort

  PathGood:
FunctionEnd

Function .onInstSuccess
  WriteUninstaller vim${VER_MAJOR}${VER_MINOR}\uninstall-gui.exe
  MessageBox MB_YESNO|MB_ICONQUESTION \
	"The installation process has been successfull. Happy Vimming! \
	$\n$\n Do you want to see the README file now?" IDNO NoReadme
	Exec '$0\gvim.exe -R "$0\README.txt"'
  NoReadme:
FunctionEnd

Function .onInstFailed
  MessageBox MB_OK|MB_ICONEXCLAMATION "Installation failed. Better luck next time."
FunctionEnd

Function un.onUnInstSuccess
  MessageBox MB_OK|MB_ICONINFORMATION \
  "Vim ${VER_MAJOR}.${VER_MINOR} has been (partly) removed from your system"
FunctionEnd

Function un.GetParent
  Exch $0 ; old $0 is on top of stack
  Push $1
  Push $2
  StrCpy $1 -1
  loop:
    StrCpy $2 $0 1 $1
    StrCmp $2 "" exit
    StrCmp $2 "\" exit
    IntOp $1 $1 - 1
  Goto loop
  exit:
    StrCpy $0 $0 $1
    Pop $2
    Pop $1
    Exch $0 ; put $0 on top of stack, restore $0 to original value
FunctionEnd

##########################################################
Section "Vim executables and runtime files"
SectionIn 1,2,3

# we need also this here if the user changes the instdir
StrCpy $0 "$INSTDIR\vim${VER_MAJOR}${VER_MINOR}"

SetOutPath $0
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\..\diff.exe
File ..\vimtutor.bat
File ..\README.txt
File ..\uninstal.txt
File ..\*.vim
File ..\rgb.txt

SetOutPath $0\colors
File ..\colors\*.*

SetOutPath $0\compiler
File ..\compiler\*.*

SetOutPath $0\doc
File ..\doc\*.txt
File ..\doc\tags

SetOutPath $0\ftplugin
File ..\ftplugin\*.*

SetOutPath $0\indent
File ..\indent\*.*

SetOutPath $0\macros
File ..\macros\*.*

SetOutPath $0\plugin
File ..\plugin\*.*

SetOutPath $0\syntax
File ..\syntax\*.*

SetOutPath $0\tools
File ..\tools\*.*

SetOutPath $0\tutor
File ..\tutor\*.*

SectionEnd
##########################################################
SectionDivider
##########################################################
Section "Create icons on the Desktop"
SectionIn 1,3

StrCpy $1 "$1 -install-icons"

SectionEnd
##########################################################
Section "Add Vim to the Start Menu"
SectionIn 1,3

StrCpy $1 "$1 -add-start-menu"

SectionEnd
##########################################################
Section "Add an Edit-with-Vim context menu entry"
SectionIn 1,3

SetOutPath $0
File ..\GvimExt\gvimext.dll

StrCpy $1 "$1 -install-popup"

SectionEnd
##########################################################
SectionDivider
##########################################################
Section "Create a _vimrc if it doesn't exist"
SectionIn 1,3

StrCpy $1 "$1 -create-vimrc"

SectionEnd
##########################################################
Section "Create .bat files for command line use"
SectionIn 3

StrCpy $1 "$1 -create-batfiles gvim evim gview gvimdiff"

SectionEnd
##########################################################
Section "Create plugin directories in HOME or VIM"
SectionIn 1,3

StrCpy $1 "$1 -create-directories home"

SectionEnd
##########################################################
Section "Create plugin directories in VIM"
SectionIn 3

StrCpy $1 "$1 -create-directories vim"

SectionEnd

##########################################################
SectionDivider
##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 3

SetOutPath $0\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\README.txt
ExecWait "regsvr32.exe /s $0\VisVim.dll"

SectionEnd
##########################################################
!ifdef HAVE_NLS
Section "Native Language Support"
SectionIn 1,3

SetOutPath $0\lang
File /r ..\lang\*.*
SetOutPath $0\keymap
File ..\keymap\README.txt
File ..\keymap\*.vim
SetOutPath $0
File ..\libintl.dll

SectionEnd
!endif
##########################################################
Section -call_install_exe

SetOutPath $0
  ExecWait "$0\install.exe $1"

SectionEnd
##########################################################
Section -post

  BringToFront

SectionEnd

##########################################################
Section Uninstall

# Apparently $INSTDIR is set to the directory where the uninstaller is created.
# Thus the "vim61" directory is included in it.
StrCpy $0 "$INSTDIR"

; If VisVim was installed, unregister the DLL
IfFileExists "$0\VisVim.dll" Has_VisVim No_VisVim
Has_VisVim:
   ExecWait "regsvr32.exe /u /s $0\VisVim.dll"

No_VisVim:

; delete the context menu entry and batch files
ExecWait "$0\uninstal.exe -nsis"

# We may have been put to the background when uninstall did something.
BringToFront

# ask the user if the Vim version dir must be removed
MessageBox MB_YESNO|MB_ICONQUESTION \
  "Would you like to delete $0?$\n \
   $\nIt contains the Vim executables and runtime files." IDNO NoRemoveExes

Delete /REBOOTOK $0\gvimext.dll
ClearErrors
RMDir /r $0

IfErrors ErrorMess NoErrorMess
  ErrorMess:
    MessageBox MB_OK|MB_ICONEXCLAMATION \
      "Some files in $0 have not been deleted!$\nYou must do it manually."
  NoErrorMess:

NoRemoveExes:

# get the parent dir of the installation
Push $INSTDIR
Call un.GetParent
Pop $0

StrCpy $1 $0

# if a plugin dir was created at installation ask the user to remove it
# first look in the root of the installation then in HOME
IfFileExists $1\vimfiles AskRemove 0
    ReadEnvStr $1 "HOME"
    StrCmp $1 "" NoRemove 0

    IfFileExists $1\vimfiles 0 NoRemove

  AskRemove:
    MessageBox MB_YESNO|MB_ICONQUESTION \
      "Remove all files in your $1\vimfiles directory? \
      $\nIf you have created something there that you want to keep, click No" IDNO Fin
    RMDir /r $1\vimfiles
  NoRemove:

# ask the user if the Vim root dir must be removed
MessageBox MB_YESNO|MB_ICONQUESTION \
  "Would you like to remove $0?$\n \
   $\nIt contains your Vim configuration files!" IDNO NoDelete
   RMDir /r $0 ; skipped if no
NoDelete:

Fin:

Call un.onUnInstSuccess

SectionEnd
