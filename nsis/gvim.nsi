# NSIS file to create a self-installing exe for Vim.

Name "Vim 6.0ah"
OutFile gVim60ah.exe
CRCCheck on
ComponentText "This will install Vim 6.0ah on your computer."
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0ah from your system."
UninstallExeName uninst-vim60ah.exe
UninstallIcon icons\vim_uninst_16c.ico

# Types of installs we can perform:
InstType Normal
InstType Full
SilentInstall normal

InstallDir "C:\vim"

Section "Vim executables and runtime files"
SectionIn 1

SetOutPath $INSTDIR\vim60ah
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\README.txt
File ..\README_bindos.txt
File ..\README_ole.txt
File ..\uninstal.txt

SetOutPath $INSTDIR\vim60ah
File ..\*.*

SetOutPath $INSTDIR\vim60ah\colors
File ..\colors\*.*

SetOutPath $INSTDIR\vim60ah\doc
File ..\doc\*.*

#SetOutPath $INSTDIR\vim60ah\doc\howto
#File ..\doc\howto\*.*

#SetOutPath $INSTDIR\vim60ah\doc\html
#File ..\doc\html\*.*

SetOutPath $INSTDIR\vim60ah\ftplugin
File ..\ftplugin\*.*

#SetOutPath $INSTDIR\vim60ah\indent
#File ..\indent\*.*

SetOutPath $INSTDIR\vim60ah\macros
File ..\macros\*.*

SetOutPath $INSTDIR\vim60ah\plugin
File ..\plugin\*.*

SetOutPath $INSTDIR\vim60ah\syntax
File ..\syntax\*.*

SetOutPath $INSTDIR\vim60ah\tools
File ..\tools\*.*

SetOutPath $INSTDIR\vim60ah\tutor
File ..\tutor\*.*

SectionEnd

##########################################################
Section "Extra OLE Vim files"
SectionIn 2

SetOutPath $INSTDIR\vim60ah\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd

##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 2

SetOutPath $INSTDIR\vim60ah\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd

##########################################################
Section "Edit with Vim context menu entry"
SectionIn 3

SetOutPath $INSTDIR\vim60ah
File ..\GvimExt\gvimext.dll

# TODO: setup the context menu entry

SectionEnd

##########################################################
Section Uninstall

Delete $INSTDIR\vim60ah\colors\*.*
RMDir $INSTDIR\vim60ah\colors

Delete $INSTDIR\vim60ah\doc\*.*
RMDir $INSTDIR\vim60ah\doc

Delete $INSTDIR\vim60ah\doc\howto\*.*
RMDir $INSTDIR\vim60ah\doc\howto

Delete $INSTDIR\vim60ah\doc\html\*.*
RMDir $INSTDIR\vim60ah\doc\html

Delete $INSTDIR\vim60ah\ftplugin\*.*
RMDir $INSTDIR\vim60ah\ftplugin

Delete $INSTDIR\vim60ah\indent\*.*
RMDir $INSTDIR\vim60ah\indent

Delete $INSTDIR\vim60ah\macros\*.*
RMDir $INSTDIR\vim60ah\macros

Delete $INSTDIR\vim60ah\plugin\*.*
RMDir $INSTDIR\vim60ah\plugin

Delete $INSTDIR\vim60ah\syntax\*.*
RMDir $INSTDIR\vim60ah\syntax

Delete $INSTDIR\vim60ah\tools\*.*
RMDir $INSTDIR\vim60ah\tools

Delete $INSTDIR\vim60ah\tutor\*.*
RMDir $INSTDIR\vim60ah\tutor

Delete $INSTDIR\vim60ah\OleVim\*.*
RMDir $INSTDIR\vim60ah\OleVim

Delete $INSTDIR\vim60ah\VisVim\*.*
RMDir $INSTDIR\vim60ah\VisVim

Delete $INSTDIR\vim60ah\*.*
RMDir $INSTDIR\vim60ah

Delete $INSTDIR\uninst-vim60ah.exe

SectionEnd
