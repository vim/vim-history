# NSIS file to create a self-installing exe for Vim.

Name "Vim 6.0al"
OutFile gVim60al.exe
CRCCheck on
ComponentText "This will install Vim 6.0al on your computer."
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0al from your system."
UninstallExeName uninst-vim60al.exe
UninstallIcon icons\vim_uninst_16c.ico

# Types of installs we can perform:
InstType Normal
InstType Full
SilentInstall normal

InstallDir "C:\vim"

Section "Vim executables and runtime files"
SectionIn 1

SetOutPath $INSTDIR\vim60al
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\README.txt
File ..\README_bindos.txt
File ..\README_ole.txt
File ..\uninstal.txt

SetOutPath $INSTDIR\vim60al
File ..\*.*

SetOutPath $INSTDIR\vim60al\colors
File ..\colors\*.*

SetOutPath $INSTDIR\vim60al\doc
File ..\doc\*.*

#SetOutPath $INSTDIR\vim60al\doc\howto
#File ..\doc\howto\*.*

#SetOutPath $INSTDIR\vim60al\doc\html
#File ..\doc\html\*.*

SetOutPath $INSTDIR\vim60al\ftplugin
File ..\ftplugin\*.*

#SetOutPath $INSTDIR\vim60al\indent
#File ..\indent\*.*

SetOutPath $INSTDIR\vim60al\macros
File ..\macros\*.*

SetOutPath $INSTDIR\vim60al\plugin
File ..\plugin\*.*

SetOutPath $INSTDIR\vim60al\syntax
File ..\syntax\*.*

SetOutPath $INSTDIR\vim60al\tools
File ..\tools\*.*

SetOutPath $INSTDIR\vim60al\tutor
File ..\tutor\*.*

SectionEnd

##########################################################
Section "Extra OLE Vim files"
SectionIn 2

SetOutPath $INSTDIR\vim60al\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd

##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 2

SetOutPath $INSTDIR\vim60al\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd

##########################################################
Section "Edit with Vim context menu entry"
SectionIn 3

SetOutPath $INSTDIR\vim60al
File ..\GvimExt\gvimext.dll

# TODO: setup the context menu entry

SectionEnd

##########################################################
Section Uninstall

Delete $INSTDIR\vim60al\colors\*.*
RMDir $INSTDIR\vim60al\colors

Delete $INSTDIR\vim60al\doc\*.*
RMDir $INSTDIR\vim60al\doc

Delete $INSTDIR\vim60al\doc\howto\*.*
RMDir $INSTDIR\vim60al\doc\howto

Delete $INSTDIR\vim60al\doc\html\*.*
RMDir $INSTDIR\vim60al\doc\html

Delete $INSTDIR\vim60al\ftplugin\*.*
RMDir $INSTDIR\vim60al\ftplugin

Delete $INSTDIR\vim60al\indent\*.*
RMDir $INSTDIR\vim60al\indent

Delete $INSTDIR\vim60al\macros\*.*
RMDir $INSTDIR\vim60al\macros

Delete $INSTDIR\vim60al\plugin\*.*
RMDir $INSTDIR\vim60al\plugin

Delete $INSTDIR\vim60al\syntax\*.*
RMDir $INSTDIR\vim60al\syntax

Delete $INSTDIR\vim60al\tools\*.*
RMDir $INSTDIR\vim60al\tools

Delete $INSTDIR\vim60al\tutor\*.*
RMDir $INSTDIR\vim60al\tutor

Delete $INSTDIR\vim60al\OleVim\*.*
RMDir $INSTDIR\vim60al\OleVim

Delete $INSTDIR\vim60al\VisVim\*.*
RMDir $INSTDIR\vim60al\VisVim

Delete $INSTDIR\vim60al\*.*
RMDir $INSTDIR\vim60al

Delete $INSTDIR\uninst-vim60al.exe

SectionEnd
