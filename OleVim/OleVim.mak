# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **
#
# OleVim.mak
#
# Makefile for OleVim programs
#
# N.B. These comments will vanish if a VC4.n IDE overwrites this file
#
# When
# 2001-08-04 W.Briscoe Original made with 4.0 IDE.
#                      I will increase density when Bram agrees functionality.
#

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=OleVim - Win32 SendToVim Debug
!MESSAGE No configuration specified.  Defaulting to OleVim - Win32 SendToVim\
 Debug.
!ENDIF 

!IF "$(CFG)" != "OleVim - Win32 OpenWithVim Debug" && "$(CFG)" !=\
 "OleVim - Win32 OpenWithVim Release" && "$(CFG)" !=\
 "OleVim - Win32 SendToVim Release" && "$(CFG)" !=\
 "OleVim - Win32 SendToVim Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "OleVim.mak" CFG="OleVim - Win32 SendToVim Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OleVim - Win32 OpenWithVim Debug" (based on\
 "Win32 (x86) Application")
!MESSAGE "OleVim - Win32 OpenWithVim Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "OleVim - Win32 SendToVim Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "OleVim - Win32 SendToVim Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "OleVim - Win32 OpenWithVim Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OleVim - Win32 OpenWithVim Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
OUTDIR=.\.
INTDIR=.\.

ALL : "$(OUTDIR)\OpenWithVim.exe"

CLEAN : 
	-@erase ".\vc40.pdb"
	-@erase ".\vc40.idb"
	-@erase ".\OpenWithVim.exe"
	-@erase ".\OpenWithVim.obj"
	-@erase ".\uvim.obj"
	-@erase ".\OpenWithVim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD CPP /nologo /W4 /WX /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MLd /W4 /WX /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OleVim.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 user32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"OpenWithVim.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=user32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/OpenWithVim.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/OpenWithVim.exe" 
LINK32_OBJS= \
	"$(INTDIR)/OpenWithVim.obj" \
	"$(INTDIR)/uvim.obj"

"$(OUTDIR)\OpenWithVim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OleVim - Win32 OpenWithVim Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\OpenWithVim.exe"

CLEAN : 
	-@erase ".\OpenWithVim.exe"
	-@erase ".\OpenWithVim.obj"
	-@erase ".\uvim.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD CPP /nologo /W4 /WX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /ML /W4 /WX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OleVim.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 user32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"OpenWithVim.exe"
LINK32_FLAGS=user32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /pdb:none /machine:I386 /out:"$(OUTDIR)/OpenWithVim.exe" 
LINK32_OBJS= \
	"$(INTDIR)/OpenWithVim.obj" \
	"$(INTDIR)/uvim.obj"

"$(OUTDIR)\OpenWithVim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\SendToVim.exe"

CLEAN : 
	-@erase ".\SendToVim.exe"
	-@erase ".\SendToVim.obj"
	-@erase ".\uvim.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD CPP /nologo /W4 /WX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /ML /W4 /WX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OleVim.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 user32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"OpenWithVim.exe"
# ADD LINK32 user32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"SendToVim.exe"
LINK32_FLAGS=user32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /pdb:none /machine:I386 /out:"$(OUTDIR)/SendToVim.exe" 
LINK32_OBJS= \
	"$(INTDIR)/SendToVim.obj" \
	"$(INTDIR)/uvim.obj"

"$(OUTDIR)\SendToVim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\SendToVim.exe"

CLEAN : 
	-@erase ".\vc40.pdb"
	-@erase ".\vc40.idb"
	-@erase ".\SendToVim.exe"
	-@erase ".\SendToVim.obj"
	-@erase ".\uvim.obj"
	-@erase ".\SendToVim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD CPP /nologo /W4 /WX /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MLd /W4 /WX /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=./
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OleVim.bsc" 
BSC32_SBRS=
LINK32=link.exe
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"SendToVim.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=user32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/SendToVim.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/SendToVim.exe" 
LINK32_OBJS= \
	"$(INTDIR)/SendToVim.obj" \
	"$(INTDIR)/uvim.obj"

"$(OUTDIR)\SendToVim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "OleVim - Win32 OpenWithVim Debug"
# Name "OleVim - Win32 OpenWithVim Release"
# Name "OleVim - Win32 SendToVim Release"
# Name "OleVim - Win32 SendToVim Debug"

!IF  "$(CFG)" == "OleVim - Win32 OpenWithVim Debug"

!ELSEIF  "$(CFG)" == "OleVim - Win32 OpenWithVim Release"

!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Release"

!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\OpenWithVim.c
DEP_CPP_OPENW=\
	".\uvim.h"\
	

!IF  "$(CFG)" == "OleVim - Win32 OpenWithVim Debug"


"$(INTDIR)\OpenWithVim.obj" : $(SOURCE) $(DEP_CPP_OPENW) "$(INTDIR)"
   $(CPP) /nologo /MLd /W4 /WX /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "OleVim - Win32 OpenWithVim Release"


"$(INTDIR)\OpenWithVim.obj" : $(SOURCE) $(DEP_CPP_OPENW) "$(INTDIR)"
   $(CPP) /nologo /ML /W4 /WX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\uvim.c
DEP_CPP_UVIM_=\
	".\uvim.h"\
	

!IF  "$(CFG)" == "OleVim - Win32 OpenWithVim Debug"


"$(INTDIR)\uvim.obj" : $(SOURCE) $(DEP_CPP_UVIM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OleVim - Win32 OpenWithVim Release"


"$(INTDIR)\uvim.obj" : $(SOURCE) $(DEP_CPP_UVIM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Release"


"$(INTDIR)\uvim.obj" : $(SOURCE) $(DEP_CPP_UVIM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Debug"


"$(INTDIR)\uvim.obj" : $(SOURCE) $(DEP_CPP_UVIM_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SendToVim.c
DEP_CPP_SENDT=\
	".\uvim.h"\
	

!IF  "$(CFG)" == "OleVim - Win32 OpenWithVim Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OleVim - Win32 OpenWithVim Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Release"


"$(INTDIR)\SendToVim.obj" : $(SOURCE) $(DEP_CPP_SENDT) "$(INTDIR)"
   $(CPP) /nologo /ML /W4 /WX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "OleVim - Win32 SendToVim Debug"


"$(INTDIR)\SendToVim.obj" : $(SOURCE) $(DEP_CPP_SENDT) "$(INTDIR)"
   $(CPP) /nologo /MLd /W4 /WX /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
