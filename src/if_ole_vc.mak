# Microsoft Developer Studio Generated NMAKE File, Based on Gvim_vc.dsp
!IF "$(CFG)" == ""
CFG=Vim - Win32 Release
!MESSAGE No configuration specified. Defaulting to Vim - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Vim - Win32 Release" && "$(CFG)" != "Vim - Win32 Debug" &&\
 "$(CFG)" != "Vim - Win32 Release with Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gvim_vc.mak" CFG="Vim - Win32 Release"
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

!IF  "$(CFG)" == "Vim - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "vim.tlb" "iid_ole.c" "if_ole.h" ".\gvim.exe"

!ELSE 

ALL : "vim.tlb" "iid_ole.c" "if_ole.h" ".\gvim.exe"

!ENDIF 

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
	-@erase "$(INTDIR)\getchar.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui_w32.obj"
	-@erase "$(INTDIR)\if_ole.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mark.obj"
	-@erase "$(INTDIR)\memfile.obj"
	-@erase "$(INTDIR)\memline.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\misc1.obj"
	-@erase "$(INTDIR)\misc2.obj"
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
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vim.res"
	-@erase "$(INTDIR)\window.obj"
	-@erase ".\gvim.exe"
	-@erase "if_ole.h"
	-@erase "iid_ole.c"
	-@erase "vim.tlb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32" /D\
 "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\vim.res" /d "NDEBUG" /d "HAVE_OLE" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gvim_vc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\gvim.pdb"\
 /machine:I386 /nodefaultlib /out:".\gvim.exe" 
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
	"$(INTDIR)\getchar.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\gui_w32.obj" \
	"$(INTDIR)\if_ole.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mark.obj" \
	"$(INTDIR)\memfile.obj" \
	"$(INTDIR)\memline.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\misc1.obj" \
	"$(INTDIR)\misc2.obj" \
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

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\gvimd.exe" "$(OUTDIR)\Gvim_vc.bsc"

!ELSE 

ALL : ".\gvimd.exe" "$(OUTDIR)\Gvim_vc.bsc"

!ENDIF 

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
	-@erase "$(INTDIR)\getchar.obj"
	-@erase "$(INTDIR)\getchar.sbr"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui.sbr"
	-@erase "$(INTDIR)\gui_w32.obj"
	-@erase "$(INTDIR)\gui_w32.sbr"
	-@erase "$(INTDIR)\if_ole.obj"
	-@erase "$(INTDIR)\if_ole.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\mark.obj"
	-@erase "$(INTDIR)\mark.sbr"
	-@erase "$(INTDIR)\memfile.obj"
	-@erase "$(INTDIR)\memfile.sbr"
	-@erase "$(INTDIR)\memline.obj"
	-@erase "$(INTDIR)\memline.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\misc1.obj"
	-@erase "$(INTDIR)\misc1.sbr"
	-@erase "$(INTDIR)\misc2.obj"
	-@erase "$(INTDIR)\misc2.sbr"
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
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(INTDIR)\vim.res"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(OUTDIR)\Gvim_vc.bsc"
	-@erase ".\gvimd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\vim.res" /d "_DEBUG" /d "HAVE_OLE" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gvim_vc.bsc" 
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
	"$(INTDIR)\getchar.sbr" \
	"$(INTDIR)\gui.sbr" \
	"$(INTDIR)\gui_w32.sbr" \
	"$(INTDIR)\if_ole.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\mark.sbr" \
	"$(INTDIR)\memfile.sbr" \
	"$(INTDIR)\memline.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\misc1.sbr" \
	"$(INTDIR)\misc2.sbr" \
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
LINK32_FLAGS=libcmtd.lib oldnames.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows\
 /profile /debug /machine:I386 /nodefaultlib /out:".\gvimd.exe" 
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
	"$(INTDIR)\getchar.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\gui_w32.obj" \
	"$(INTDIR)\if_ole.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mark.obj" \
	"$(INTDIR)\memfile.obj" \
	"$(INTDIR)\memline.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\misc1.obj" \
	"$(INTDIR)\misc2.obj" \
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

OUTDIR=.\RelDebug
INTDIR=.\RelDebug

!IF "$(RECURSE)" == "0" 

ALL : ".\gvimrwd.exe"

!ELSE 

ALL : ".\gvimrwd.exe"

!ENDIF 

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
	-@erase "$(INTDIR)\getchar.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui_w32.obj"
	-@erase "$(INTDIR)\if_ole.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mark.obj"
	-@erase "$(INTDIR)\memfile.obj"
	-@erase "$(INTDIR)\memline.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\misc1.obj"
	-@erase "$(INTDIR)\misc2.obj"
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
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vim.res"
	-@erase "$(INTDIR)\window.obj"
	-@erase ".\gvimrwd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\RelDebug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\vim.res" /d "NDEBUG" /d "HAVE_OLE" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gvim_vc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmtd.lib oldnames.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows\
 /profile /debug /machine:I386 /nodefaultlib /out:".\gvimrwd.exe" 
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
	"$(INTDIR)\getchar.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\gui_w32.obj" \
	"$(INTDIR)\if_ole.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mark.obj" \
	"$(INTDIR)\memfile.obj" \
	"$(INTDIR)\memline.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\misc1.obj" \
	"$(INTDIR)\misc2.obj" \
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


!IF "$(CFG)" == "Vim - Win32 Release" || "$(CFG)" == "Vim - Win32 Debug" ||\
 "$(CFG)" == "Vim - Win32 Release with Debug"
SOURCE=.\buffer.c
DEP_CPP_BUFFE=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BUFFE=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj"	"$(INTDIR)\buffer.sbr" : $(SOURCE) $(DEP_CPP_BUFFE)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\charset.c
DEP_CPP_CHARS=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CHARS=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\charset.obj"	"$(INTDIR)\charset.sbr" : $(SOURCE) $(DEP_CPP_CHARS)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\digraph.c
DEP_CPP_DIGRA=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DIGRA=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digraph.obj" : $(SOURCE) $(DEP_CPP_DIGRA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digraph.obj"	"$(INTDIR)\digraph.sbr" : $(SOURCE) $(DEP_CPP_DIGRA)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digraph.obj" : $(SOURCE) $(DEP_CPP_DIGRA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\edit.c
DEP_CPP_EDIT_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EDIT_=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\edit.obj"	"$(INTDIR)\edit.sbr" : $(SOURCE) $(DEP_CPP_EDIT_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\eval.c
DEP_CPP_EVAL_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EVAL_=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\eval.obj" : $(SOURCE) $(DEP_CPP_EVAL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\eval.obj"	"$(INTDIR)\eval.sbr" : $(SOURCE) $(DEP_CPP_EVAL_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\eval.obj" : $(SOURCE) $(DEP_CPP_EVAL_) "$(INTDIR)"


!ENDIF 

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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EX_CM=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ex_cmds.obj" : $(SOURCE) $(DEP_CPP_EX_CM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ex_cmds.obj"	"$(INTDIR)\ex_cmds.sbr" : $(SOURCE) $(DEP_CPP_EX_CM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ex_cmds.obj" : $(SOURCE) $(DEP_CPP_EX_CM) "$(INTDIR)"


!ENDIF 

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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EX_DO=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ex_docmd.obj" : $(SOURCE) $(DEP_CPP_EX_DO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ex_docmd.obj"	"$(INTDIR)\ex_docmd.sbr" : $(SOURCE) $(DEP_CPP_EX_DO)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ex_docmd.obj" : $(SOURCE) $(DEP_CPP_EX_DO) "$(INTDIR)"


!ENDIF 

SOURCE=.\ex_getln.c
DEP_CPP_EX_GE=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EX_GE=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ex_getln.obj" : $(SOURCE) $(DEP_CPP_EX_GE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ex_getln.obj"	"$(INTDIR)\ex_getln.sbr" : $(SOURCE) $(DEP_CPP_EX_GE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ex_getln.obj" : $(SOURCE) $(DEP_CPP_EX_GE) "$(INTDIR)"


!ENDIF 

SOURCE=.\fileio.c
DEP_CPP_FILEI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FILEI=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj"	"$(INTDIR)\fileio.sbr" : $(SOURCE) $(DEP_CPP_FILEI)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\getchar.c
DEP_CPP_GETCH=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GETCH=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\getchar.obj" : $(SOURCE) $(DEP_CPP_GETCH) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\getchar.obj"	"$(INTDIR)\getchar.sbr" : $(SOURCE) $(DEP_CPP_GETCH)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\getchar.obj" : $(SOURCE) $(DEP_CPP_GETCH) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\gui.c
DEP_CPP_GUI_C=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GUI_C=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\gui.obj" : $(SOURCE) $(DEP_CPP_GUI_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\gui.obj"	"$(INTDIR)\gui.sbr" : $(SOURCE) $(DEP_CPP_GUI_C)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\gui.obj" : $(SOURCE) $(DEP_CPP_GUI_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\gui_w32.c

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_GUI_W=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	

"$(INTDIR)\gui_w32.obj" : $(SOURCE) $(DEP_CPP_GUI_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_GUI_W=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GUI_W=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

"$(INTDIR)\gui_w32.obj"	"$(INTDIR)\gui_w32.sbr" : $(SOURCE) $(DEP_CPP_GUI_W)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_GUI_W=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GUI_W=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

"$(INTDIR)\gui_w32.obj" : $(SOURCE) $(DEP_CPP_GUI_W) "$(INTDIR)"


!ENDIF 

SOURCE=.\if_ole.cpp

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_IF_OL=\
	".\ascii.h"\
	".\ex_cmds.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\if_ole.h"\
	".\iid_ole.c"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	

"$(INTDIR)\if_ole.obj" : $(SOURCE) $(DEP_CPP_IF_OL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_IF_OL=\
	".\ascii.h"\
	".\ex_cmds.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\if_ole.h"\
	".\iid_ole.c"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_IF_OL=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

"$(INTDIR)\if_ole.obj"	"$(INTDIR)\if_ole.sbr" : $(SOURCE) $(DEP_CPP_IF_OL)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_IF_OL=\
	".\ascii.h"\
	".\ex_cmds.h"\
	".\feature.h"\
	".\globals.h"\
	".\gui.h"\
	".\if_ole.h"\
	".\iid_ole.c"\
	".\keymap.h"\
	".\macros.h"\
	".\option.h"\
	".\os_win32.h"\
	".\proto.h"\
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_IF_OL=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

"$(INTDIR)\if_ole.obj" : $(SOURCE) $(DEP_CPP_IF_OL) "$(INTDIR)"


!ENDIF 

SOURCE=.\main.c

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_MAIN_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	
CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_MAIN_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MAIN_=\
	".\config.h"\
	".\farsi.c"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	
CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_MAIN_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MAIN_=\
	".\config.h"\
	".\farsi.c"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	
CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mark.c
DEP_CPP_MARK_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MARK_=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mark.obj" : $(SOURCE) $(DEP_CPP_MARK_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mark.obj"	"$(INTDIR)\mark.sbr" : $(SOURCE) $(DEP_CPP_MARK_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mark.obj" : $(SOURCE) $(DEP_CPP_MARK_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\memfile.c
DEP_CPP_MEMFI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MEMFI=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\memfile.obj" : $(SOURCE) $(DEP_CPP_MEMFI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\memfile.obj"	"$(INTDIR)\memfile.sbr" : $(SOURCE) $(DEP_CPP_MEMFI)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\memfile.obj" : $(SOURCE) $(DEP_CPP_MEMFI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\memline.c
DEP_CPP_MEMLI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MEMLI=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\memline.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\memline.obj"	"$(INTDIR)\memline.sbr" : $(SOURCE) $(DEP_CPP_MEMLI)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\memline.obj" : $(SOURCE) $(DEP_CPP_MEMLI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\message.c
DEP_CPP_MESSA=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MESSA=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\message.obj"	"$(INTDIR)\message.sbr" : $(SOURCE) $(DEP_CPP_MESSA)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\misc1.c
DEP_CPP_MISC1=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MISC1=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\misc1.obj" : $(SOURCE) $(DEP_CPP_MISC1) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\misc1.obj"	"$(INTDIR)\misc1.sbr" : $(SOURCE) $(DEP_CPP_MISC1)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\misc1.obj" : $(SOURCE) $(DEP_CPP_MISC1) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\misc2.c
DEP_CPP_MISC2=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MISC2=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\misc2.obj" : $(SOURCE) $(DEP_CPP_MISC2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\misc2.obj"	"$(INTDIR)\misc2.sbr" : $(SOURCE) $(DEP_CPP_MISC2)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\misc2.obj" : $(SOURCE) $(DEP_CPP_MISC2) "$(INTDIR)"


!ENDIF 

SOURCE=.\normal.c
DEP_CPP_NORMA=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_NORMA=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\normal.obj" : $(SOURCE) $(DEP_CPP_NORMA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\normal.obj"	"$(INTDIR)\normal.sbr" : $(SOURCE) $(DEP_CPP_NORMA)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\normal.obj" : $(SOURCE) $(DEP_CPP_NORMA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ops.c
DEP_CPP_OPS_C=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OPS_C=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ops.obj" : $(SOURCE) $(DEP_CPP_OPS_C) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ops.obj"	"$(INTDIR)\ops.sbr" : $(SOURCE) $(DEP_CPP_OPS_C)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ops.obj" : $(SOURCE) $(DEP_CPP_OPS_C) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\option.c
DEP_CPP_OPTIO=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OPTIO=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\option.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\option.obj"	"$(INTDIR)\option.sbr" : $(SOURCE) $(DEP_CPP_OPTIO)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\option.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\os_w32exe.c
DEP_CPP_OS_W3=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OS_W3=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\os_w32exe.obj" : $(SOURCE) $(DEP_CPP_OS_W3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\os_w32exe.obj"	"$(INTDIR)\os_w32exe.sbr" : $(SOURCE)\
 $(DEP_CPP_OS_W3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\os_w32exe.obj" : $(SOURCE) $(DEP_CPP_OS_W3) "$(INTDIR)"


!ENDIF 

SOURCE=.\os_win32.c

!IF  "$(CFG)" == "Vim - Win32 Release"

DEP_CPP_OS_WI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	

"$(INTDIR)\os_win32.obj" : $(SOURCE) $(DEP_CPP_OS_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

DEP_CPP_OS_WI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OS_WI=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

"$(INTDIR)\os_win32.obj"	"$(INTDIR)\os_win32.sbr" : $(SOURCE) $(DEP_CPP_OS_WI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

DEP_CPP_OS_WI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_OS_WI=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

"$(INTDIR)\os_win32.obj" : $(SOURCE) $(DEP_CPP_OS_WI) "$(INTDIR)"


!ENDIF 

SOURCE=.\quickfix.c
DEP_CPP_QUICK=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_QUICK=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\quickfix.obj" : $(SOURCE) $(DEP_CPP_QUICK) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\quickfix.obj"	"$(INTDIR)\quickfix.sbr" : $(SOURCE) $(DEP_CPP_QUICK)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\quickfix.obj" : $(SOURCE) $(DEP_CPP_QUICK) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\regexp.c
DEP_CPP_REGEX=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_REGEX=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\regexp.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\regexp.obj"	"$(INTDIR)\regexp.sbr" : $(SOURCE) $(DEP_CPP_REGEX)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\regexp.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\screen.c
DEP_CPP_SCREE=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SCREE=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\screen.obj"	"$(INTDIR)\screen.sbr" : $(SOURCE) $(DEP_CPP_SCREE)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\search.c
DEP_CPP_SEARC=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SEARC=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj"	"$(INTDIR)\search.sbr" : $(SOURCE) $(DEP_CPP_SEARC)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\syntax.c
DEP_CPP_SYNTA=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SYNTA=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\syntax.obj"	"$(INTDIR)\syntax.sbr" : $(SOURCE) $(DEP_CPP_SYNTA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"


!ENDIF 

SOURCE=.\tag.c
DEP_CPP_TAG_C=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TAG_C=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tag.obj" : $(SOURCE) $(DEP_CPP_TAG_C) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tag.obj"	"$(INTDIR)\tag.sbr" : $(SOURCE) $(DEP_CPP_TAG_C)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tag.obj" : $(SOURCE) $(DEP_CPP_TAG_C) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\term.c
DEP_CPP_TERM_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_TERM_=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\term.obj" : $(SOURCE) $(DEP_CPP_TERM_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\term.obj"	"$(INTDIR)\term.sbr" : $(SOURCE) $(DEP_CPP_TERM_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\term.obj" : $(SOURCE) $(DEP_CPP_TERM_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ui.c
DEP_CPP_UI_C40=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UI_C40=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"


"$(INTDIR)\ui.obj" : $(SOURCE) $(DEP_CPP_UI_C40) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"


"$(INTDIR)\ui.obj"	"$(INTDIR)\ui.sbr" : $(SOURCE) $(DEP_CPP_UI_C40) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"


"$(INTDIR)\ui.obj" : $(SOURCE) $(DEP_CPP_UI_C40) "$(INTDIR)"


!ENDIF 

SOURCE=.\undo.c
DEP_CPP_UNDO_=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UNDO_=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\undo.obj"	"$(INTDIR)\undo.sbr" : $(SOURCE) $(DEP_CPP_UNDO_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\version.c
DEP_CPP_VERSI=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\version.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VERSI=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\version.obj"	"$(INTDIR)\version.sbr" : $(SOURCE) $(DEP_CPP_VERSI)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\version.obj" : $(SOURCE) $(DEP_CPP_VERSI) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\vim.rc
DEP_RSC_VIM_R=\
	".\version.h"\
	".\VIM.TLB"\
	".\vim.ico"\
	

"$(INTDIR)\vim.res" : $(SOURCE) $(DEP_RSC_VIM_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\window.c
DEP_CPP_WINDO=\
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
	".\proto\buffer.pro"\
	".\proto\charset.pro"\
	".\proto\digraph.pro"\
	".\proto\edit.pro"\
	".\proto\eval.pro"\
	".\proto\ex_cmds.pro"\
	".\proto\ex_docmd.pro"\
	".\proto\ex_getln.pro"\
	".\proto\fileio.pro"\
	".\proto\getchar.pro"\
	".\proto\gui.pro"\
	".\proto\gui_w32.pro"\
	".\proto\if_ole.pro"\
	".\proto\if_python.pro"\
	".\proto\main.pro"\
	".\proto\mark.pro"\
	".\proto\memfile.pro"\
	".\proto\memline.pro"\
	".\proto\message.pro"\
	".\proto\misc1.pro"\
	".\proto\misc2.pro"\
	".\proto\normal.pro"\
	".\proto\ops.pro"\
	".\proto\option.pro"\
	".\proto\os_win32.pro"\
	".\proto\quickfix.pro"\
	".\proto\regexp.pro"\
	".\proto\screen.pro"\
	".\proto\search.pro"\
	".\proto\syntax.pro"\
	".\proto\tag.pro"\
	".\proto\term.pro"\
	".\proto\ui.pro"\
	".\proto\undo.pro"\
	".\proto\version.pro"\
	".\proto\window.pro"\
	".\regexp.h"\
	".\structs.h"\
	".\term.h"\
	".\vim.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WINDO=\
	".\config.h"\
	".\farsi.h"\
	".\osdef.h"\
	".\proto\os_vms.pro"\
	".\SniffVim.h"\
	".\vms.h"\
	

!IF  "$(CFG)" == "Vim - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "USE_GUI_WIN32"\
 /D "HAVE_OLE" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj"	"$(INTDIR)\window.sbr" : $(SOURCE) $(DEP_CPP_WINDO)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D\
 "USE_GUI_WIN32" /Iproto /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\if_ole.idl

!IF  "$(CFG)" == "Vim - Win32 Release"

InputPath=.\if_ole.idl

"iid_ole.c"	"if_ole.h"	"vim.tlb"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	midl /tlb vim.tlb /iid iid_ole.c /proxy nul /header if_ole.h $(InputPath)

!ELSEIF  "$(CFG)" == "Vim - Win32 Debug"

!ELSEIF  "$(CFG)" == "Vim - Win32 Release with Debug"

!ENDIF 


!ENDIF 

