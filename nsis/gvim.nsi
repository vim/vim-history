# NSIS file to create a self-installing exe for Vim.

Name "Vim 6.0am"
OutFile gVim60am.exe
CRCCheck on
ComponentText "This will install Vim 6.0am on your computer."
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0am from your system."
UninstallExeName uninst-vim60am.exe
UninstallIcon icons\vim_uninst_16c.ico

# Types of installs we can perform:
InstType Normal
InstType Full
SilentInstall normal

InstallDir "C:\vim"

Section "Vim executables and runtime files"
SectionIn 1

SetOutPath $INSTDIR\vim60am
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\README.txt
File ..\README_bindos.txt
File ..\README_ole.txt
File ..\uninstal.txt

SetOutPath $INSTDIR\vim60am
File ..\*.*

SetOutPath $INSTDIR\vim60am\colors
File ..\colors\*.*

SetOutPath $INSTDIR\vim60am\doc
File ..\doc\*.*

#SetOutPath $INSTDIR\vim60am\doc\howto
#File ..\doc\howto\*.*

#SetOutPath $INSTDIR\vim60am\doc\html
#File ..\doc\html\*.*

SetOutPath $INSTDIR\vim60am\ftplugin
File ..\ftplugin\*.*

#SetOutPath $INSTDIR\vim60am\indent
#File ..\indent\*.*

SetOutPath $INSTDIR\vim60am\macros
File ..\macros\*.*

SetOutPath $INSTDIR\vim60am\plugin
File ..\plugin\*.*

SetOutPath $INSTDIR\vim60am\syntax
File ..\syntax\*.*

SetOutPath $INSTDIR\vim60am\tools
File ..\tools\*.*

SetOutPath $INSTDIR\vim60am\tutor
File ..\tutor\*.*

SectionEnd

##########################################################
Section "Extra OLE Vim files"
SectionIn 2

SetOutPath $INSTDIR\vim60am\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd

##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 2

SetOutPath $INSTDIR\vim60am\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd

##########################################################
Section "Edit with Vim context menu entry"
SectionIn 3

SetOutPath $INSTDIR\vim60am
File ..\GvimExt\gvimext.dll

# TODO: setup the context menu entry

SectionEnd

##########################################################
Section Uninstall

Delete $INSTDIR\vim60am\colors\*.*
RMDir $INSTDIR\vim60am\colors

Delete $INSTDIR\vim60am\doc\*.*
RMDir $INSTDIR\vim60am\doc

Delete $INSTDIR\vim60am\doc\howto\*.*
RMDir $INSTDIR\vim60am\doc\howto

Delete $INSTDIR\vim60am\doc\html\*.*
RMDir $INSTDIR\vim60am\doc\html

Delete $INSTDIR\vim60am\ftplugin\*.*
RMDir $INSTDIR\vim60am\ftplugin

Delete $INSTDIR\vim60am\indent\*.*
RMDir $INSTDIR\vim60am\indent

Delete $INSTDIR\vim60am\macros\*.*
RMDir $INSTDIR\vim60am\macros

Delete $INSTDIR\vim60am\plugin\*.*
RMDir $INSTDIR\vim60am\plugin

Delete $INSTDIR\vim60am\syntax\*.*
RMDir $INSTDIR\vim60am\syntax

Delete $INSTDIR\vim60am\tools\*.*
RMDir $INSTDIR\vim60am\tools

Delete $INSTDIR\vim60am\tutor\*.*
RMDir $INSTDIR\vim60am\tutor

Delete $INSTDIR\vim60am\OleVim\*.*
RMDir $INSTDIR\vim60am\OleVim

Delete $INSTDIR\vim60am\VisVim\*.*
RMDir $INSTDIR\vim60am\VisVim

Delete $INSTDIR\vim60am\*.*
RMDir $INSTDIR\vim60am

Delete $INSTDIR\uninst-vim60am.exe

SectionEnd
