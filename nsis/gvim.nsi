# NSIS file to create a self-installing exe for Vim.
# Last modification:	2001 Jul 21

# WARNING: if you make changes to this script, look out for $0 to be valid,
# because this line is very dangerous:		RMDir /r $0

# comment next line if you don't have UPX.
# Get it at http://upx.sourceforge.net
!define HAVE_UPX

# comment the next line if you do not want to add Native Language Support
!define HAVE_NLS

Name "Vim 6.0av"
OutFile gVim60av.exe
CRCCheck on
ComponentText "This will install Vim 6.0av on your computer."
DirText "Choose a directory to install Vim (should end in 'vim')"
SetDatablockOptimize on
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0av from your system."
UninstallExeName vim60av\uninstall-gui.exe
UninstallIcon icons\vim_uninst_16c.ico
BGGradient 004000 008200 ffffff
LicenseText "You should read the following before installing:"
LicenseData ..\doc\uganda.nsis.txt

!ifdef HAVE_UPX
  !packhdr temp.dat "upx --best --compress-icons=1 temp.dat"
!endif

# Types of installs we can perform:
InstType Typical
InstType Minimal
InstType Full

SilentInstall normal

##########################################################
# Functions

Function .onInit
  MessageBox MB_YESNO|MB_ICONQUESTION \
	"This will install Vim 6.0av on your computer.$\n Continue?" IDYES NoAbort

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
  StrCpy $0 "$INSTDIR\vim60av"
  StrCpy $1 "-register-OLE"

FunctionEnd

Function .onUserAbort
  MessageBox MB_YESNO|MB_ICONQUESTION "Abort install?" IDYES NoCancelAbort
    Abort ; causes installer to not quit.
  NoCancelAbort:
FunctionEnd

# Only enable the "Next" button if the install directory is OK.
Function .onVerifyInstDir
  StrCmp $INSTDIR $PROGRAMFILES PathBad 0
  StrCmp $INSTDIR $WINDIR PathBad PathGood
    PathBad:
    Abort

  PathGood:
FunctionEnd

Function .onInstSuccess
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
  MessageBox MB_OK|MB_ICONINFORMATION "Vim 6.0av has been (partly) removed from your system"
FunctionEnd

##########################################################
Section "Vim executables and runtime files"
SectionIn 1,2,3

# we need also this here if the user changes the instdir
StrCpy $0 "$INSTDIR\vim60av"

SetOutPath $0
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
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
# Thus the "vim60av" directory is included in it.
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
      "Some files in $0 have not been deleted! You must do it manually."
  NoErrorMess:

NoRemoveExes:

GetParentDir $0 $INSTDIR

# if a plugin dir was created at installation ask the user to remove it
IfFileExists $0\vimfiles 0 NoRemove
    MessageBox MB_YESNO|MB_ICONQUESTION \
      "Remove all files in your $0\vimfiles directory? \
      $\nIf you have created something there that you want to keep, click No" IDNO Fin
    RMDir /r $0\vimfiles
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
