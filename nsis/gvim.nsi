# NSIS file to create a self-installing exe for Vim.

Name "Vim 6.0ai"
OutFile gVim60ai.exe
CRCCheck on
ComponentText "This will install Vim 6.0ai on your computer."
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0ai from your system."
UninstallExeName uninst-vim60ai.exe
UninstallIcon icons\vim_uninst_16c.ico

# Types of installs we can perform:
InstType Normal
InstType Full
SilentInstall normal

InstallDir "C:\vim"

Section "Vim executables and runtime files"
SectionIn 1

SetOutPath $INSTDIR\vim60ai
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\README.txt
File ..\README_bindos.txt
File ..\README_ole.txt
File ..\uninstal.txt

SetOutPath $INSTDIR\vim60ai
File ..\*.*

SetOutPath $INSTDIR\vim60ai\colors
File ..\colors\*.*

SetOutPath $INSTDIR\vim60ai\doc
File ..\doc\*.*

#SetOutPath $INSTDIR\vim60ai\doc\howto
#File ..\doc\howto\*.*

#SetOutPath $INSTDIR\vim60ai\doc\html
#File ..\doc\html\*.*

SetOutPath $INSTDIR\vim60ai\ftplugin
File ..\ftplugin\*.*

#SetOutPath $INSTDIR\vim60ai\indent
#File ..\indent\*.*

SetOutPath $INSTDIR\vim60ai\macros
File ..\macros\*.*

SetOutPath $INSTDIR\vim60ai\plugin
File ..\plugin\*.*

SetOutPath $INSTDIR\vim60ai\syntax
File ..\syntax\*.*

SetOutPath $INSTDIR\vim60ai\tools
File ..\tools\*.*

SetOutPath $INSTDIR\vim60ai\tutor
File ..\tutor\*.*

SectionEnd

##########################################################
Section "Extra OLE Vim files"
SectionIn 2

SetOutPath $INSTDIR\vim60ai\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd

##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 2

SetOutPath $INSTDIR\vim60ai\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd

##########################################################
Section "Edit with Vim context menu entry"
SectionIn 3

SetOutPath $INSTDIR\vim60ai
File ..\GvimExt\gvimext.dll

# TODO: setup the context menu entry

SectionEnd

##########################################################
Section Uninstall

Delete $INSTDIR\vim60ai\colors\*.*
RMDir $INSTDIR\vim60ai\colors

Delete $INSTDIR\vim60ai\doc\*.*
RMDir $INSTDIR\vim60ai\doc

Delete $INSTDIR\vim60ai\doc\howto\*.*
RMDir $INSTDIR\vim60ai\doc\howto

Delete $INSTDIR\vim60ai\doc\html\*.*
RMDir $INSTDIR\vim60ai\doc\html

Delete $INSTDIR\vim60ai\ftplugin\*.*
RMDir $INSTDIR\vim60ai\ftplugin

Delete $INSTDIR\vim60ai\indent\*.*
RMDir $INSTDIR\vim60ai\indent

Delete $INSTDIR\vim60ai\macros\*.*
RMDir $INSTDIR\vim60ai\macros

Delete $INSTDIR\vim60ai\plugin\*.*
RMDir $INSTDIR\vim60ai\plugin

Delete $INSTDIR\vim60ai\syntax\*.*
RMDir $INSTDIR\vim60ai\syntax

Delete $INSTDIR\vim60ai\tools\*.*
RMDir $INSTDIR\vim60ai\tools

Delete $INSTDIR\vim60ai\tutor\*.*
RMDir $INSTDIR\vim60ai\tutor

Delete $INSTDIR\vim60ai\OleVim\*.*
RMDir $INSTDIR\vim60ai\OleVim

Delete $INSTDIR\vim60ai\VisVim\*.*
RMDir $INSTDIR\vim60ai\VisVim

Delete $INSTDIR\vim60ai\*.*
RMDir $INSTDIR\vim60ai

Delete $INSTDIR\uninst-vim60ai.exe

SectionEnd
