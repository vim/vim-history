# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Vim - Win32 Release
!MESSAGE No configuration specified.  Defaulting to Vim - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Vim - Win32 Release" && "$(CFG)" != "Vim - Win32 Debug" &&\
 "$(CFG)" != "Vim - Win32 Release with Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gvim_vc.mak" CFG="Vim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vim - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Vim - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Vim - Win32 Release with Debug" (based on\
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
# PROP Target_Last_Scanned "Vim - Win32 Release with Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Vim - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Rel"
# PROP BASE Intermediate_Dir "Rel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Rel"
# PROP Intermediate_Dir "Rel"
OUTDIR=.\Rel
INTDIR=.\Rel

ALL : ".\gvim.exe"

CLEAN : 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\digraph.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\ex_cmds.obj"
	-@erase "$(INTDIR)\ex_docmd.obj"
	-@erase "$(INTDIR)\ex_getln.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fold.obj"
	-@erase "$(INTDIR)\getchar.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui_w32.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mark.obj"
	-@erase "$(INTDIR)\memfile.obj"
	-@erase "$(INTDIR)\memline.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\misc1.obj"
	-@erase "$(INTDIR)\misc2.obj"
	-@erase "$(INTDIR)\multibyte.obj"
	-@erase "$(INTDIR)\normal.obj"
	-@erase "$(INTDIR)\ops.obj"
	-@erase "$(INTDIR)\option.obj"
	-@erase "$(INTDIR)\os_w32exe.obj"
	-@erase "$(INTDIR)\os_win32.obj"
	-@erase "$(INTDIR)\quickfix.obj"
	-@erase "$(INTDIR)\regexp.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\term.obj"
	-@erase "$(INTDIR)\ui.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vim.res"
	-@erase "$(INTDIR)\window.obj"
	-@erase ".\gvim.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/vim.bsc" : $(OUTDIR)  $(BSC32_SBRS)
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /Iproto /FR /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32" /Iproto /c
# SUBTRACT CPP /Fr /YX
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Rel/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "FEAT_GUI_W32"
# ADD RSC /l 0x409 /d "NDEBUG" /d "FEAT_GUI_W32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vim.res" /d "NDEBUG" /d "FEAT_GUI_W32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Gvim_vc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib imm32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libc.lib oldnames.lib kernel32.lib user32.lib gdi32.lib imm32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"gvim.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=libc.lib oldnames.lib kernel32.lib user32.lib gdi32.lib imm32.lib\
 winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/gvim.pdb" /machine:I386 /nodefaultlib\
 /out:"gvim.exe" 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\digraph.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\ex_cmds.obj" \
	"$(INTDIR)\ex_docmd.obj" \
	"$(INTDIR)\ex_getln.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\fold.obj" \
	"$(INTDIR)\getchar.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\gui_w32.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mark.obj" \
	"$(INTDIR)\memfile.obj" \
	"$(INTDIR)\memline.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\misc1.obj" \
	"$(INTDIR)\misc2.obj" \
	"$(INTDIR)\multibyte.obj" \
	"$(INTDIR)\normal.obj" \
	"$(INTDIR)\ops.obj" \
	"$(INTDIR)\option.obj" \
	"$(INTDIR)\os_w32exe.obj" \
	"$(INTDIR)\os_win32.obj" \
	"$(INTDIR)\quickfix.obj" \
	"$(INTDIR)\regexp.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\term.obj" \
	"$(INTDIR)\ui.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vim.res" \
	"$(INTDIR)\window.obj"

".\gvim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Dbg"
# PROP BASE Intermediate_Dir "Dbg"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Dbg"
# PROP Intermediate_Dir "Dbg"
OUTDIR=.\Dbg
INTDIR=.\Dbg

ALL : ".\gvimd.exe" "$(OUTDIR)\Gvim_vc.bsc"

CLEAN : 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\buffer.sbr"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\charset.sbr"
	-@erase "$(INTDIR)\digraph.obj"
	-@erase "$(INTDIR)\digraph.sbr"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\edit.sbr"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\eval.sbr"
	-@erase "$(INTDIR)\ex_cmds.obj"
	-@erase "$(INTDIR)\ex_cmds.sbr"
	-@erase "$(INTDIR)\ex_docmd.obj"
	-@erase "$(INTDIR)\ex_docmd.sbr"
	-@erase "$(INTDIR)\ex_getln.obj"
	-@erase "$(INTDIR)\ex_getln.sbr"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fileio.sbr"
	-@erase "$(INTDIR)\fold.obj"
	-@erase "$(INTDIR)\fold.sbr"
	-@erase "$(INTDIR)\getchar.obj"
	-@erase "$(INTDIR)\getchar.sbr"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui.sbr"
	-@erase "$(INTDIR)\gui_w32.obj"
	-@erase "$(INTDIR)\gui_w32.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\mark.obj"
	-@erase "$(INTDIR)\mark.sbr"
	-@erase "$(INTDIR)\memfile.obj"
	-@erase "$(INTDIR)\memfile.sbr"
	-@erase "$(INTDIR)\memline.obj"
	-@erase "$(INTDIR)\memline.sbr"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menu.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\misc1.obj"
	-@erase "$(INTDIR)\misc1.sbr"
	-@erase "$(INTDIR)\misc2.obj"
	-@erase "$(INTDIR)\misc2.sbr"
	-@erase "$(INTDIR)\multibyte.obj"
	-@erase "$(INTDIR)\multibyte.sbr"
	-@erase "$(INTDIR)\normal.obj"
	-@erase "$(INTDIR)\normal.sbr"
	-@erase "$(INTDIR)\ops.obj"
	-@erase "$(INTDIR)\ops.sbr"
	-@erase "$(INTDIR)\option.obj"
	-@erase "$(INTDIR)\option.sbr"
	-@erase "$(INTDIR)\os_w32exe.obj"
	-@erase "$(INTDIR)\os_w32exe.sbr"
	-@erase "$(INTDIR)\os_win32.obj"
	-@erase "$(INTDIR)\os_win32.sbr"
	-@erase "$(INTDIR)\quickfix.obj"
	-@erase "$(INTDIR)\quickfix.sbr"
	-@erase "$(INTDIR)\regexp.obj"
	-@erase "$(INTDIR)\regexp.sbr"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\screen.sbr"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\search.sbr"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\syntax.sbr"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\tag.sbr"
	-@erase "$(INTDIR)\term.obj"
	-@erase "$(INTDIR)\term.sbr"
	-@erase "$(INTDIR)\ui.obj"
	-@erase "$(INTDIR)\ui.sbr"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undo.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(INTDIR)\vim.res"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(OUTDIR)\Gvim_vc.bsc"
	-@erase ".\gvimd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/vim.bsc" : $(OUTDIR)  $(BSC32_SBRS)
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /Iproto /FR /YX /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "FEAT_GUI_W32" /Iproto /FR /c
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Dbg/
CPP_SBRS=.\Dbg/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "FEAT_GUI_W32"
# ADD RSC /l 0x409 /d "_DEBUG" /d "FEAT_GUI_W32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vim.res" /d "_DEBUG" /d "FEAT_GUI_W32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Gvim_vc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\buffer.sbr" \
	"$(INTDIR)\charset.sbr" \
	"$(INTDIR)\digraph.sbr" \
	"$(INTDIR)\edit.sbr" \
	"$(INTDIR)\eval.sbr" \
	"$(INTDIR)\ex_cmds.sbr" \
	"$(INTDIR)\ex_docmd.sbr" \
	"$(INTDIR)\ex_getln.sbr" \
	"$(INTDIR)\fileio.sbr" \
	"$(INTDIR)\fold.sbr" \
	"$(INTDIR)\getchar.sbr" \
	"$(INTDIR)\gui.sbr" \
	"$(INTDIR)\gui_w32.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\mark.sbr" \
	"$(INTDIR)\memfile.sbr" \
	"$(INTDIR)\memline.sbr" \
	"$(INTDIR)\menu.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\misc1.sbr" \
	"$(INTDIR)\misc2.sbr" \
	"$(INTDIR)\multibyte.sbr" \
	"$(INTDIR)\normal.sbr" \
	"$(INTDIR)\ops.sbr" \
	"$(INTDIR)\option.sbr" \
	"$(INTDIR)\os_w32exe.sbr" \
	"$(INTDIR)\os_win32.sbr" \
	"$(INTDIR)\quickfix.sbr" \
	"$(INTDIR)\regexp.sbr" \
	"$(INTDIR)\screen.sbr" \
	"$(INTDIR)\search.sbr" \
	"$(INTDIR)\syntax.sbr" \
	"$(INTDIR)\tag.sbr" \
	"$(INTDIR)\term.sbr" \
	"$(INTDIR)\ui.sbr" \
	"$(INTDIR)\undo.sbr" \
	"$(INTDIR)\version.sbr" \
	"$(INTDIR)\window.sbr"

"$(OUTDIR)\Gvim_vc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib imm32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 libcd.lib oldnames.lib kernel32.lib user32.lib gdi32.lib imm32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib /out:"gvimd.exe"
LINK32_FLAGS=libcd.lib oldnames.lib kernel32.lib user32.lib gdi32.lib imm32.lib\
 winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows\
 /profile /map:"$(INTDIR)\gvimd.map" /debug /machine:I386 /nodefaultlib /out:"gvimd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\digraph.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\ex_cmds.obj" \
	"$(INTDIR)\ex_docmd.obj" \
	"$(INTDIR)\ex_getln.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\fold.obj" \
	"$(INTDIR)\getchar.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\gui_w32.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mark.obj" \
	"$(INTDIR)\memfile.obj" \
	"$(INTDIR)\memline.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\misc1.obj" \
	"$(INTDIR)\misc2.obj" \
	"$(INTDIR)\multibyte.obj" \
	"$(INTDIR)\normal.obj" \
	"$(INTDIR)\ops.obj" \
	"$(INTDIR)\option.obj" \
	"$(INTDIR)\os_w32exe.obj" \
	"$(INTDIR)\os_win32.obj" \
	"$(INTDIR)\quickfix.obj" \
	"$(INTDIR)\regexp.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\term.obj" \
	"$(INTDIR)\ui.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vim.res" \
	"$(INTDIR)\window.obj"

".\gvimd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_Re"
# PROP BASE Intermediate_Dir "Win32_Re"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelDebug"
# PROP Intermediate_Dir "RelDebug"
OUTDIR=.\RelDebug
INTDIR=.\RelDebug

ALL : ".\gvimrwd.exe"

CLEAN : 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\digraph.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\ex_cmds.obj"
	-@erase "$(INTDIR)\ex_docmd.obj"
	-@erase "$(INTDIR)\ex_getln.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fold.obj"
	-@erase "$(INTDIR)\getchar.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui_w32.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mark.obj"
	-@erase "$(INTDIR)\memfile.obj"
	-@erase "$(INTDIR)\memline.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\misc1.obj"
	-@erase "$(INTDIR)\misc2.obj"
	-@erase "$(INTDIR)\multibyte.obj"
	-@erase "$(INTDIR)\normal.obj"
	-@erase "$(INTDIR)\ops.obj"
	-@erase "$(INTDIR)\option.obj"
	-@erase "$(INTDIR)\os_w32exe.obj"
	-@erase "$(INTDIR)\os_win32.obj"
	-@erase "$(INTDIR)\quickfix.obj"
	-@erase "$(INTDIR)\regexp.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\term.obj"
	-@erase "$(INTDIR)\ui.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vim.res"
	-@erase "$(INTDIR)\window.obj"
	-@erase ".\gvimrwd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/vim.bsc" : $(OUTDIR)  $(BSC32_SBRS)
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /Iproto /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32" /Iproto /c
# SUBTRACT CPP /Fr /YX
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\RelDebug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "FEAT_GUI_W32"
# ADD RSC /l 0x409 /d "NDEBUG" /d "FEAT_GUI_W32"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/vim.res" /d "NDEBUG" /d "FEAT_GUI_W32" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Gvim_vc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib imm32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"vim.exe"
# ADD LINK32 libc.lib oldnames.lib kernel32.lib user32.lib gdi32.lib imm32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib /out:"gvimrwd.exe"
LINK32_FLAGS=libc.lib oldnames.lib kernel32.lib user32.lib gdi32.lib imm32.lib\
 winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows\
 /profile /debug /machine:I386 /nodefaultlib /out:"gvimrwd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\digraph.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\ex_cmds.obj" \
	"$(INTDIR)\ex_docmd.obj" \
	"$(INTDIR)\ex_getln.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\fold.obj" \
	"$(INTDIR)\getchar.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\gui_w32.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mark.obj" \
	"$(INTDIR)\memfile.obj" \
	"$(INTDIR)\memline.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\misc1.obj" \
	"$(INTDIR)\misc2.obj" \
	"$(INTDIR)\multibyte.obj" \
	"$(INTDIR)\normal.obj" \
	"$(INTDIR)\ops.obj" \
	"$(INTDIR)\option.obj" \
	"$(INTDIR)\os_w32exe.obj" \
	"$(INTDIR)\os_win32.obj" \
	"$(INTDIR)\quickfix.obj" \
	"$(INTDIR)\regexp.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\term.obj" \
	"$(INTDIR)\ui.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vim.res" \
	"$(INTDIR)\window.obj"

".\gvimrwd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Vim - Win32 Release"
# Name "Vim - Win32 Debug"
# Name "Vim - Win32 Release with Debug"

!IF  "$(CFG)" == "Vim - Win32 Release"

!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\edit.c
DEP_CPP_EDIT_=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_EDIT_=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\edit.sbr" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\buffer.c
DEP_CPP_BUFFE=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_BUFFE=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\buffer.sbr" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fileio.c
DEP_CPP_FILEI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_FILEI=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fileio.sbr" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fold.c
DEP_CPP_FOLD=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_FOLD=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\fold.obj" : $(SOURCE) $(DEP_CPP_FOLD) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\fold.obj" : $(SOURCE) $(DEP_CPP_FOLD) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fold.sbr" : $(SOURCE) $(DEP_CPP_FOLD) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\fold.obj" : $(SOURCE) $(DEP_CPP_FOLD) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\quickfix.c
DEP_CPP_QUICK=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_QUICK=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\quickfix.obj" : $(SOURCE) $(DEP_CPP_QUICK) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\quickfix.obj" : $(SOURCE) $(DEP_CPP_QUICK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\quickfix.sbr" : $(SOURCE) $(DEP_CPP_QUICK) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\quickfix.obj" : $(SOURCE) $(DEP_CPP_QUICK) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memline.c
DEP_CPP_MEMLI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MEMLI=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\memline.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\memline.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\memline.sbr" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\memline.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\menu.c
DEP_CPP_MEMLI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MEMLI=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\menu.sbr" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\menu.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getchar.c
DEP_CPP_GETCH=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_GETCH=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\getchar.obj" : $(SOURCE) $(DEP_CPP_GETCH) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\getchar.obj" : $(SOURCE) $(DEP_CPP_GETCH) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\getchar.sbr" : $(SOURCE) $(DEP_CPP_GETCH) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\getchar.obj" : $(SOURCE) $(DEP_CPP_GETCH) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\charset.c
DEP_CPP_CHARS=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_CHARS=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\charset.sbr" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\version.c
DEP_CPP_VERSI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\version.h"\
	".\vim.h"\
	
NODEP_CPP_VERSI=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\version.sbr" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memfile.c
DEP_CPP_MEMFI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MEMFI=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\memfile.obj" : $(SOURCE) $(DEP_CPP_MEMFI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\memfile.obj" : $(SOURCE) $(DEP_CPP_MEMFI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\memfile.sbr" : $(SOURCE) $(DEP_CPP_MEMFI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\memfile.obj" : $(SOURCE) $(DEP_CPP_MEMFI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\message.c
DEP_CPP_MESSA=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MESSA=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\message.sbr" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\normal.c
DEP_CPP_NORMA=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_NORMA=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\normal.obj" : $(SOURCE) $(DEP_CPP_NORMA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\normal.obj" : $(SOURCE) $(DEP_CPP_NORMA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\normal.sbr" : $(SOURCE) $(DEP_CPP_NORMA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\normal.obj" : $(SOURCE) $(DEP_CPP_NORMA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\search.c
DEP_CPP_SEARC=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_SEARC=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mark.c
DEP_CPP_MARK_=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MARK_=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\mark.obj" : $(SOURCE) $(DEP_CPP_MARK_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\mark.obj" : $(SOURCE) $(DEP_CPP_MARK_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mark.sbr" : $(SOURCE) $(DEP_CPP_MARK_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\mark.obj" : $(SOURCE) $(DEP_CPP_MARK_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc1.c
DEP_CPP_MISC1=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MISC1=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\misc1.obj" : $(SOURCE) $(DEP_CPP_MISC1) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\misc1.obj" : $(SOURCE) $(DEP_CPP_MISC1) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\misc1.sbr" : $(SOURCE) $(DEP_CPP_MISC1) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\misc1.obj" : $(SOURCE) $(DEP_CPP_MISC1) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\term.c
DEP_CPP_TERM_=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_TERM_=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\term.obj" : $(SOURCE) $(DEP_CPP_TERM_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\term.obj" : $(SOURCE) $(DEP_CPP_TERM_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\term.sbr" : $(SOURCE) $(DEP_CPP_TERM_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\term.obj" : $(SOURCE) $(DEP_CPP_TERM_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tag.c
DEP_CPP_TAG_C=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_TAG_C=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\tag.obj" : $(SOURCE) $(DEP_CPP_TAG_C) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\tag.obj" : $(SOURCE) $(DEP_CPP_TAG_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tag.sbr" : $(SOURCE) $(DEP_CPP_TAG_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\tag.obj" : $(SOURCE) $(DEP_CPP_TAG_C) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\screen.c
DEP_CPP_SCREE=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_SCREE=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\screen.sbr" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ops.c
DEP_CPP_OPS_C=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OPS_C=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\ops.obj" : $(SOURCE) $(DEP_CPP_OPS_C) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\ops.obj" : $(SOURCE) $(DEP_CPP_OPS_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ops.sbr" : $(SOURCE) $(DEP_CPP_OPS_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\ops.obj" : $(SOURCE) $(DEP_CPP_OPS_C) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regexp.c
DEP_CPP_REGEX=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_REGEX=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\regexp.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\regexp.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\regexp.sbr" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\regexp.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\window.c
DEP_CPP_WINDO=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_WINDO=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\window.sbr" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\undo.c
DEP_CPP_UNDO_=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_UNDO_=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\undo.sbr" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\digraph.c
DEP_CPP_DIGRA=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_DIGRA=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\digraph.obj" : $(SOURCE) $(DEP_CPP_DIGRA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\digraph.obj" : $(SOURCE) $(DEP_CPP_DIGRA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\digraph.sbr" : $(SOURCE) $(DEP_CPP_DIGRA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\digraph.obj" : $(SOURCE) $(DEP_CPP_DIGRA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\main.c
DEP_CPP_MAIN_=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MAIN_=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\option.c
DEP_CPP_OPTIO=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OPTIO=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

# SUBTRACT CPP /Fr /YX

"$(INTDIR)\option.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "FEAT_GUI_W32"\
 /Iproto /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /FR"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\option.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\option.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

# SUBTRACT BASE CPP /Fr /YX
# SUBTRACT CPP /Fr /YX

"$(INTDIR)\option.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "FEAT_GUI_W32" /Iproto /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vim.rc
DEP_RSC_VIM_R=\
	".\version.h"\
	".\tearoff.bmp"\
	".\tools.bmp"\
	".\vim.ico"\
	".\vim_error.ico"\
	".\vim_alert.ico"\
	".\vim_info.ico"\
	".\vim_quest.ico"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\vim.res" : $(SOURCE) $(DEP_RSC_VIM_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\vim.res" : $(SOURCE) $(DEP_RSC_VIM_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\vim.res" : $(SOURCE) $(DEP_RSC_VIM_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ui.c
DEP_CPP_UI_C32=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_UI_C32=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ui.obj" : $(SOURCE) $(DEP_CPP_UI_C32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ui.obj" : $(SOURCE) $(DEP_CPP_UI_C32) "$(INTDIR)"

"$(INTDIR)\ui.sbr" : $(SOURCE) $(DEP_CPP_UI_C32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ui.obj" : $(SOURCE) $(DEP_CPP_UI_C32) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ex_cmds.c
DEP_CPP_EX_CM=\
	".\ascii.h"\
	".\ex_cmds.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_EX_CM=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ex_cmds.obj" : $(SOURCE) $(DEP_CPP_EX_CM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ex_cmds.obj" : $(SOURCE) $(DEP_CPP_EX_CM) "$(INTDIR)"

"$(INTDIR)\ex_cmds.sbr" : $(SOURCE) $(DEP_CPP_EX_CM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ex_cmds.obj" : $(SOURCE) $(DEP_CPP_EX_CM) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ex_docmd.c
DEP_CPP_EX_DO=\
	".\ascii.h"\
	".\ex_cmds.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_EX_DO=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ex_docmd.obj" : $(SOURCE) $(DEP_CPP_EX_DO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ex_docmd.obj" : $(SOURCE) $(DEP_CPP_EX_DO) "$(INTDIR)"

"$(INTDIR)\ex_docmd.sbr" : $(SOURCE) $(DEP_CPP_EX_DO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ex_docmd.obj" : $(SOURCE) $(DEP_CPP_EX_DO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ex_getln.c
DEP_CPP_EX_GE=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_EX_GE=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ex_getln.obj" : $(SOURCE) $(DEP_CPP_EX_GE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ex_getln.obj" : $(SOURCE) $(DEP_CPP_EX_GE) "$(INTDIR)"

"$(INTDIR)\ex_getln.sbr" : $(SOURCE) $(DEP_CPP_EX_GE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ex_getln.obj" : $(SOURCE) $(DEP_CPP_EX_GE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\syntax.c
DEP_CPP_SYNTA=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_SYNTA=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"

"$(INTDIR)\syntax.sbr" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gui.c
DEP_CPP_GUI_C=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_GUI_C=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\gui.obj" : $(SOURCE) $(DEP_CPP_GUI_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\gui.obj" : $(SOURCE) $(DEP_CPP_GUI_C) "$(INTDIR)"

"$(INTDIR)\gui.sbr" : $(SOURCE) $(DEP_CPP_GUI_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\gui.obj" : $(SOURCE) $(DEP_CPP_GUI_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\eval.c
DEP_CPP_EVAL_=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_EVAL_=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\eval.obj" : $(SOURCE) $(DEP_CPP_EVAL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\eval.obj" : $(SOURCE) $(DEP_CPP_EVAL_) "$(INTDIR)"

"$(INTDIR)\eval.sbr" : $(SOURCE) $(DEP_CPP_EVAL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\eval.obj" : $(SOURCE) $(DEP_CPP_EVAL_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\os_w32exe.c

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_OS_W3=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OS_W3=\
	

"$(INTDIR)\os_w32exe.obj" : $(SOURCE) $(DEP_CPP_OS_W3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_OS_W3=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OS_W3=\
	

"$(INTDIR)\os_w32exe.obj" : $(SOURCE) $(DEP_CPP_OS_W3) "$(INTDIR)"

"$(INTDIR)\os_w32exe.sbr" : $(SOURCE) $(DEP_CPP_OS_W3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_OS_W3=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OS_W3=\
	

"$(INTDIR)\os_w32exe.obj" : $(SOURCE) $(DEP_CPP_OS_W3) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\os_win32.c

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_OS_WI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OS_WI=\
	

"$(INTDIR)\os_win32.obj" : $(SOURCE) $(DEP_CPP_OS_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_OS_WI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OS_WI=\
	

"$(INTDIR)\os_win32.obj" : $(SOURCE) $(DEP_CPP_OS_WI) "$(INTDIR)"

"$(INTDIR)\os_win32.sbr" : $(SOURCE) $(DEP_CPP_OS_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_OS_WI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_OS_WI=\
	

"$(INTDIR)\os_win32.obj" : $(SOURCE) $(DEP_CPP_OS_WI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc2.c
DEP_CPP_MISC2=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MISC2=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\misc2.obj" : $(SOURCE) $(DEP_CPP_MISC2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\misc2.obj" : $(SOURCE) $(DEP_CPP_MISC2) "$(INTDIR)"

"$(INTDIR)\misc2.sbr" : $(SOURCE) $(DEP_CPP_MISC2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\misc2.obj" : $(SOURCE) $(DEP_CPP_MISC2) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multibyte.c
DEP_CPP_MULTI=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_MISC2=\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\multibyte.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\multibyte.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"

"$(INTDIR)\multibyte.sbr" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\multibyte.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gui_w32.c

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_GUI_W=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_GUI_W=\
	

"$(INTDIR)\gui_w32.obj" : $(SOURCE) $(DEP_CPP_GUI_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_GUI_W=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_GUI_W=\
	

"$(INTDIR)\gui_w32.obj" : $(SOURCE) $(DEP_CPP_GUI_W) "$(INTDIR)"

"$(INTDIR)\gui_w32.sbr" : $(SOURCE) $(DEP_CPP_GUI_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_GUI_W=\
	".\ascii.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
NODEP_CPP_GUI_W=\
	

"$(INTDIR)\gui_w32.obj" : $(SOURCE) $(DEP_CPP_GUI_W) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
