# NSIS file to create a self-installing exe for Vim.
# Last modification:	2001 Jul 9

# WARNING: if you make changes to this script I advise you to comment out line
# 66:		RMDir /r $0    (it is extremely dangerous)
# and to disable the uninstall feature until you are sure that all the things go
# to the right place. (Eduardo)

# comment next line if you don't have UPX.
# Get it at http://upx.sourceforge.net
!define HAVE_UPX

# comment the next line if you do not want to add Native Language Support 
!define HAVE_NLS

Name "Vim 6.0an"
OutFile gVim60an.exe
CRCCheck on
ComponentText "This will install Vim 6.0an on your computer."
DirText "Choose a directory to install Vim"
SetDatablockOptimize on
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0an from your system."
UninstallExeName uninst-vim60an.exe
UninstallIcon icons\vim_uninst_16c.ico
BGGradient 005000 008200 ffffff
LicenseText "You should read the following before installing:"
LicenseData ..\doc\uganda.txt

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
	"This will install Vim 6.0an on your computer.$\n Continue?" IDYES NoAbort
  
  Abort ; causes installer to quit.
  NoAbort:
  StrCpy $INSTDIR "C:\vim"
# User variables:
# $0 - holds the directory the executables are installed to
# $1 - holds the parameters to be passed to install.exe.  Starts empty.
  StrCpy $0 "$INSTDIR\vim60an"
  StrCpy $1 "-register-OLE"

FunctionEnd

Function .onVerifyInstDir

  StrCpy $0 "$INSTDIR\vim60an"

  IfFileExists $0 0 RemoveOldVim
  	MessageBox MB_YESNO|MB_ICONQUESTION \
	"A previous installation of Vim exists: $0. \
	$\nDo you want to remove it?" IDNO NoRemoveOldVim

	RemoveOldVim:
		RMDir /r $0
		Goto Fin

	NoRemoveOldVim:
  		MessageBox MB_OK|MB_ICONEXCLAMATION \
		"You must choose another directory to install Vim \
		$\n if you want to keep the old installation."
        
	Fin:

FunctionEnd

Function .onUserAbort
  MessageBox MB_YESNO|MB_ICONQUESTION "Abort install?" IDYES NoCancelAbort
    Abort ; causes installer to not quit.
  NoCancelAbort:
FunctionEnd

Function .onInstSuccess
!ifdef HAVE_NLS
IfFileExists $0\lang 0 NoNLS
  MessageBox MB_OK|MB_ICONINFORMATION \
	"To complete the installation you must add the following line \
	$\n$\nset LANG=<your language>, e.g. es_es.iso_8859-1 \
	$\n$\n to your autoexec.bat file and reboot the system."
  NoNLS:
!endif
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
  MessageBox MB_OK|MB_ICONINFORMATION "Vim 6.0an has been (partly) removed from your system"  
FunctionEnd

##########################################################
Section "Vim executables and runtime files"
SectionIn 1,2,3

# we need also this here if the user changes the instdir
StrCpy $0 "$INSTDIR\vim60an"

SetOutPath $0
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\vimtutor.bat
File ..\README.txt
File ..\uninstal.txt


SetOutPath $0
File ..\*.vim
File ..\rgb.txt

SetOutPath $0\colors
File ..\colors\*.*

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

SetOutPath $0
  CreateShortCut "$DESKTOP\gvim 6.0.lnk" "$0\gvim.exe"
  CreateShortCut "$DESKTOP\gview 6.0.lnk" "$0\gvim.exe" "-R"
  CreateShortCut "$DESKTOP\evim 6.0.lnk" "$0\gvim.exe" "-y"

SectionEnd
##########################################################
Section "Add Vim to the Start Menu"
SectionIn 1,3

SetOutPath $0
  CreateDirectory "$SMPROGRAMS\Vim 6.0"
  CreateShortCut "$SMPROGRAMS\Vim 6.0\Readme.lnk" \
                 "$0\gvim.exe" '-R "$0\README.txt"' \
                 "$0\gvim.exe" 
  CreateShortCut "$SMPROGRAMS\Vim 6.0\Vim tutor.lnk" \
                 "$0\vimtutor.bat" "" "$0\gvim.exe" 
  CreateShortCut "$SMPROGRAMS\Vim 6.0\Help.lnk" "$0\gvim.exe" "-c h"
  CreateShortCut "$SMPROGRAMS\Vim 6.0\gVim.lnk" "$0\gvim.exe"
  CreateShortCut "$SMPROGRAMS\Vim 6.0\gView.lnk" "$0\gvim.exe" "-R"
  CreateShortCut "$SMPROGRAMS\Vim 6.0\eVim.lnk" "$0\gvim.exe" "-y"
  CreateShortCut "$SMPROGRAMS\Vim 6.0\GVimdiff.lnk" "$0\gvim.exe" "-d"
  CreateShortCut "$SMPROGRAMS\Vim 6.0\Uninstall.lnk" \
                 "$INSTDIR\uninst-vim60an.exe"

  WriteINIStr "$SMPROGRAMS\Vim 6.0\Vim online.url" InternetShortcut URL http://vim.sf.net/

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
Section "Extra OLE Vim files"
SectionIn 3

SetOutPath $0\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd
##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 3

SetOutPath $0\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd
##########################################################
!ifdef HAVE_NLS
Section "Native Language Support"
SectionIn 3

SetOutPath $0\lang
File /r ..\lang\*.*
SetOutPath $0\keymap
File ..\keymap\README.txt
File ..\keymap\*.vim

SectionEnd
!endif
##########################################################
Section -call_install_exe

SetOutPath $0
  StrCmp $1 "" NoExecInstall
	ExecWait "$0\install.exe $1"

  NoExecInstall:

SectionEnd
##########################################################
Section -post

  ExecShell open '$INSTDIR'
  Sleep 500
  BringToFront

SectionEnd

##########################################################
Section Uninstall

StrCpy $0 "$INSTDIR\vim60an"

; delete uninstaller
Delete $INSTDIR\uninst-vim60an.exe

; delete the context menu entry and batch files
ExecWait "$0\uninstal.exe -nsis"

; remove start menu entries
Delete "$SMPROGRAMS\Vim 6.0\*.*"
RMDir "$SMPROGRAMS\Vim 6.0"

; remove icons on the desktop
Delete "$DESKTOP\gvim 6.0.lnk"
Delete "$DESKTOP\gview 6.0.lnk"
Delete "$DESKTOP\evim 6.0.lnk"

# ask the user if the Vim version dir must be removed
MessageBox MB_YESNO|MB_ICONQUESTION \
  "Would you like to delete $0?$\n \
   $\nIt contains the Vim executables and runtime files." IDNO Fin

ClearErrors
RMDir /r $0

IfErrors ErrorMess NoErrorMess
  ErrorMess:
    MessageBox MB_OK|MB_ICONEXCLAMATION \
      "Some files in $0 have not been deleted! You must do it manually."
  NoErrorMess:

# if a plugin dir was created at installation ask the user to remove it
IfFileExists $INSTDIR\vimfiles 0 NoRemove 
    MessageBox MB_YESNO|MB_ICONQUESTION \
      "Remove all files in your $INSTDIR\vimfiles directory? \
      $\nIf you have created something there that you want to keep, click No" IDNO Fin
    RMDir /r $INSTDIR\vimfiles
  NoRemove:

# ask the user if the Vim root dir must be removed
MessageBox MB_YESNO|MB_ICONQUESTION \
  "Would you like to remove $INSTDIR?$\n \
   $\nIt contains your Vim configuration files!" IDNO NoDelete
   RMDir /r $INSTDIR ; skipped if no
NoDelete:

Fin:

Call un.onUnInstSuccess

SectionEnd
