# NSIS file to create a self-installing exe for Vim.

Name "Vim 6.0aj"
OutFile gVim60aj.exe
CRCCheck on
ComponentText "This will install Vim 6.0aj on your computer."
Icon icons\vim_16c.ico
UninstallText "This will uninstall Vim 6.0aj from your system."
UninstallExeName uninst-vim60aj.exe
UninstallIcon icons\vim_uninst_16c.ico

# Types of installs we can perform:
InstType Normal
InstType Full
SilentInstall normal

InstallDir "C:\vim"

Section "Vim executables and runtime files"
SectionIn 1

SetOutPath $INSTDIR\vim60aj
File ..\src\gvim.exe
File ..\src\install.exe
File ..\src\uninstal.exe
File ..\src\vimrun.exe
File ..\src\xxd\xxd.exe
File ..\README.txt
File ..\README_bindos.txt
File ..\README_ole.txt
File ..\uninstal.txt

SetOutPath $INSTDIR\vim60aj
File ..\*.*

SetOutPath $INSTDIR\vim60aj\colors
File ..\colors\*.*

SetOutPath $INSTDIR\vim60aj\doc
File ..\doc\*.*

#SetOutPath $INSTDIR\vim60aj\doc\howto
#File ..\doc\howto\*.*

#SetOutPath $INSTDIR\vim60aj\doc\html
#File ..\doc\html\*.*

SetOutPath $INSTDIR\vim60aj\ftplugin
File ..\ftplugin\*.*

#SetOutPath $INSTDIR\vim60aj\indent
#File ..\indent\*.*

SetOutPath $INSTDIR\vim60aj\macros
File ..\macros\*.*

SetOutPath $INSTDIR\vim60aj\plugin
File ..\plugin\*.*

SetOutPath $INSTDIR\vim60aj\syntax
File ..\syntax\*.*

SetOutPath $INSTDIR\vim60aj\tools
File ..\tools\*.*

SetOutPath $INSTDIR\vim60aj\tutor
File ..\tutor\*.*

SectionEnd

##########################################################
Section "Extra OLE Vim files"
SectionIn 2

SetOutPath $INSTDIR\vim60aj\OleVim
File ..\OleVim\OpenWithVim.exe
File ..\OleVim\SendToVim.exe
File ..\OleVim\README.txt

SectionEnd

##########################################################
Section "VisVim Extension for MS Visual Studio"
SectionIn 2

SetOutPath $INSTDIR\vim60aj\VisVim
File ..\VisVim\VisVim.dll
File ..\VisVim\VsReadMe.txt
File ..\VisVim\README.txt

SectionEnd

##########################################################
Section "Edit with Vim context menu entry"
SectionIn 3

SetOutPath $INSTDIR\vim60aj
File ..\GvimExt\gvimext.dll

# TODO: setup the context menu entry

SectionEnd

##########################################################
Section Uninstall

Delete $INSTDIR\vim60aj\colors\*.*
RMDir $INSTDIR\vim60aj\colors

Delete $INSTDIR\vim60aj\doc\*.*
RMDir $INSTDIR\vim60aj\doc

Delete $INSTDIR\vim60aj\doc\howto\*.*
RMDir $INSTDIR\vim60aj\doc\howto

Delete $INSTDIR\vim60aj\doc\html\*.*
RMDir $INSTDIR\vim60aj\doc\html

Delete $INSTDIR\vim60aj\ftplugin\*.*
RMDir $INSTDIR\vim60aj\ftplugin

Delete $INSTDIR\vim60aj\indent\*.*
RMDir $INSTDIR\vim60aj\indent

Delete $INSTDIR\vim60aj\macros\*.*
RMDir $INSTDIR\vim60aj\macros

Delete $INSTDIR\vim60aj\plugin\*.*
RMDir $INSTDIR\vim60aj\plugin

Delete $INSTDIR\vim60aj\syntax\*.*
RMDir $INSTDIR\vim60aj\syntax

Delete $INSTDIR\vim60aj\tools\*.*
RMDir $INSTDIR\vim60aj\tools

Delete $INSTDIR\vim60aj\tutor\*.*
RMDir $INSTDIR\vim60aj\tutor

Delete $INSTDIR\vim60aj\OleVim\*.*
RMDir $INSTDIR\vim60aj\OleVim

Delete $INSTDIR\vim60aj\VisVim\*.*
RMDir $INSTDIR\vim60aj\VisVim

Delete $INSTDIR\vim60aj\*.*
RMDir $INSTDIR\vim60aj

Delete $INSTDIR\uninst-vim60aj.exe

SectionEnd
