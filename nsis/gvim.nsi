# NSIS file to create a self-installing exe for Vim.

Name "Vim 6.0ak"
OutFile gVim60ak.exe
CRCCheck on
ComponentText "This will install Vim 6.0ak on your computer."
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0ak from your system."
UninstallExeName uninst-vim60ak.exe
UninstallIcon icons\vim_uninst_16c.ico

# Types of installs we can perform:
InstType Normal
InstType Full
SilentInstall normal

InstallDir "C:\vim"

Section "Vim executables and runtime files"
SectionIn 1

SetOutPath $INSTDIR\vim60ak
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\README.txt
File ..\README_bindos.txt
File ..\README_ole.txt
File ..\uninstal.txt

SetOutPath $INSTDIR\vim60ak
File ..\*.*

SetOutPath $INSTDIR\vim60ak\colors
File ..\colors\*.*

SetOutPath $INSTDIR\vim60ak\doc
File ..\doc\*.*

#SetOutPath $INSTDIR\vim60ak\doc\howto
#File ..\doc\howto\*.*

#SetOutPath $INSTDIR\vim60ak\doc\html
#File ..\doc\html\*.*

SetOutPath $INSTDIR\vim60ak\ftplugin
File ..\ftplugin\*.*

#SetOutPath $INSTDIR\vim60ak\indent
#File ..\indent\*.*

SetOutPath $INSTDIR\vim60ak\macros
File ..\macros\*.*

SetOutPath $INSTDIR\vim60ak\plugin
File ..\plugin\*.*

SetOutPath $INSTDIR\vim60ak\syntax
File ..\syntax\*.*

SetOutPath $INSTDIR\vim60ak\tools
File ..\tools\*.*

SetOutPath $INSTDIR\vim60ak\tutor
File ..\tutor\*.*

SectionEnd

##########################################################
Section "Extra OLE Vim files"
SectionIn 2

SetOutPath $INSTDIR\vim60ak\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd

##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 2

SetOutPath $INSTDIR\vim60ak\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd

##########################################################
Section "Edit with Vim context menu entry"
SectionIn 3

SetOutPath $INSTDIR\vim60ak
File ..\GvimExt\gvimext.dll

# TODO: setup the context menu entry

SectionEnd

##########################################################
Section Uninstall

Delete $INSTDIR\vim60ak\colors\*.*
RMDir $INSTDIR\vim60ak\colors

Delete $INSTDIR\vim60ak\doc\*.*
RMDir $INSTDIR\vim60ak\doc

Delete $INSTDIR\vim60ak\doc\howto\*.*
RMDir $INSTDIR\vim60ak\doc\howto

Delete $INSTDIR\vim60ak\doc\html\*.*
RMDir $INSTDIR\vim60ak\doc\html

Delete $INSTDIR\vim60ak\ftplugin\*.*
RMDir $INSTDIR\vim60ak\ftplugin

Delete $INSTDIR\vim60ak\indent\*.*
RMDir $INSTDIR\vim60ak\indent

Delete $INSTDIR\vim60ak\macros\*.*
RMDir $INSTDIR\vim60ak\macros

Delete $INSTDIR\vim60ak\plugin\*.*
RMDir $INSTDIR\vim60ak\plugin

Delete $INSTDIR\vim60ak\syntax\*.*
RMDir $INSTDIR\vim60ak\syntax

Delete $INSTDIR\vim60ak\tools\*.*
RMDir $INSTDIR\vim60ak\tools

Delete $INSTDIR\vim60ak\tutor\*.*
RMDir $INSTDIR\vim60ak\tutor

Delete $INSTDIR\vim60ak\OleVim\*.*
RMDir $INSTDIR\vim60ak\OleVim

Delete $INSTDIR\vim60ak\VisVim\*.*
RMDir $INSTDIR\vim60ak\VisVim

Delete $INSTDIR\vim60ak\*.*
RMDir $INSTDIR\vim60ak

Delete $INSTDIR\uninst-vim60ak.exe

SectionEnd
