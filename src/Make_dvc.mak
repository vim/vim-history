# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Vim - Win32 IDE for Make_mvc.mak
!MESSAGE No configuration specified.  Defaulting to Vim - Win32 IDE for\
 Make_mvc.mak.
!ENDIF 

!IF "$(CFG)" != "Vim - Win32 IDE for Make_mvc.mak"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Make_dvc.mak" CFG="Vim - Win32 IDE for Make_mvc.mak"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vim - Win32 IDE for Make_mvc.mak" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Vim - Win32 IDE for Make_mvc.mak"
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\vimrun.exe" "$(OUTDIR)\dummy"

CLEAN : 
	-@erase ".\dummy"
	-@erase ".\vimrun.exe"
	-@erase ".\vimrun.obj"

# ADD BASE CPP /nologo /W3 /D "NDEBUG" /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /D "NDEBUG" /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Make_dvc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:console /machine:I386 /out:"vimrun.exe"
# ADD LINK32 /nologo /subsystem:console /machine:I386 /out:"vimrun.exe"
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/vimrun.pdb" /machine:I386 /out:"$(OUTDIR)/vimrun.exe" 
LINK32_OBJS= \
	"$(INTDIR)/vimrun.obj"

"$(OUTDIR)\vimrun.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
InputPath=.\vimrun.exe
SOURCE=$(InputPath)

"dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   echo Files built with Make_mvc.mak have IDE support in this file.
   echo To use that support:
   echo 1) Build a vim. e.g. nmake -f Make_mvc.mak debug=y gui=yes
   echo 2) Set the built executable for debugging
   echo a) Alt+F7/Debug takes you to the Debug dialog.
   echo b) Fill "Executable for debug session". e.g. gvimd.exe
   echo c) Fill "Program arguments". e.g. -R Make_dvc.mak
   echo d) Complete dialog
   echo 3) You can now debug the gvimd.exe you built with Make_mvc.mak
   

# End Custom Build
CPP_PROJ=/nologo /ML /W3 /D "NDEBUG" /c 

.c.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx.obj:
   $(CPP) $(CPP_PROJ) $<  

.c.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Vim - Win32 IDE for Make_mvc.mak"
################################################################################
# Begin Source File

SOURCE=.\vimrun.c

"$(INTDIR)\vimrun.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
