#
# Makefile for Vim.
# Compiler: Borland C++ 5.0 and later 32-bit compiler
#  Targets: Dos16 or Win32 (Windows NT and Windows 95) (with/without GUI)
#
# Contributed by Ben Singer.
# Updated 4/1997 by Ron Aaron
#	6/1997 - added support for 16 bit DOS
#	Note: this has been tested, and works, for BC5.  Your mileage may vary.
#	Has been reported NOT to work with BC 4.52.  Maybe it can be fixed?
#	10/1997 - ron - fixed bugs w/ BC 5.02
#	8/1998 - ron - updated with new targets, fixed some stuff
#	3/2000 - Bram: Made it work with BC 5.5 free command line compiler,
#			cleaned up variables.
#	6/2001 - Dan - Added support for compiling Python and TCL
#
# It builds on Windows 95 and NT-Intel, producing the same binary in either
# case.  To build using Microsoft Visual C++, use Make_mvc.mak.
#
# This should work with the free Borland command line compiler, version 5.5.
# You need at least sp1 (service pack 1).  With sp2 it compiles faster.
# Use a command like this:
# <path>\bin\make /f Make_bc5.mak BOR=<path>
#

# let the make utility do the hard work:
.AUTODEPEND
.CACHEAUTODEPEND
#
# VARIABLES:
# name		value (default)
#
# BOR		path to root of Borland C install (c:\bc5)
# LINK		name of the linker ($(BOR)\bin\ilink if OSTYPE is DOS16,
#		$(BOR)\bin\ilink32 otherwise)
# GUI		0 or 1: set to 1 if you want the GUI version (1)
# PERL		define to path to Perl dir to get Perl support (not defined)
#   PERL_VER	  define to version of Perl being used (56)
#   DYNAMIC_PERL  no or yes: set to yes to load the Perl DLL dynamically (no)
# PYTHON	define to path to Python dir to get PYTHON support (not defined)
#   PYTHON_VER	    define to version of Python being used (15)
#   DYNAMIC_PYTHON  no or yes: use yes to load the Python DLL dynamically (no)
# TCL		define to path to TCL dir to get TCL support (not defined)
#   TCL_VER	define to version oc TCL being used (83)
# OLE		0 or 1: set to 1 to make OLE gvim (0)
# OSTYPE	DOS16 or WIN32 (WIN32)
# DEBUG		set to "-v" if you wish a DEBUGging build (not defined)
# CODEGUARD	set to "-vG" if you want to use CODEGUARD (not defined)
# CPU		1 through 6: select CPU to compile for (3)
# USEDLL	0 or 1: set to 1 to use the Runtime library DLL (1)
#		For USEDLL=1 the cc3250.dll is required to run Vim.
# VIMDLL	0 or 1: create vim32.dll, and stub (g)vim.exe (0)
# ALIGN		1, 2 or 4: Alignment to use (4 for Win32, 2 for DOS16)
# FASTCALL	0 or 1: set to 1 to use register-based function protocol (1)
# OPTIMIZE	SPEED or SPACE: type of optimization (SPEED)
#
### BOR: root of the BC installation
!if ("$(BOR)"=="")
BOR = c:\bc5
!endif
#
### LINK: Name of the linker: tlink or ilink32 (this is below, depends on
# $(OSTYPE)
#
### GUI: 1 for GUI version, 0 for console version
!if ("$(GUI)"=="")
GUI = 1
!endif
#
### PERL: uncommented this line if you want perl support in vim
#	'USEDLL' doesn't seem to work with perl, don't know why.
# PERL=c:\perl
#
### PYTHON: uncommented this line if you want python support in vim
# PYTHON=c:\python20
#
### TCL: uncommented this line if you want tcl support in vim
# TCL=c:\tcl
#
### OLE: 0 for normal gvim, 1 for OLE-capable gvim (only works with GUI)
#OLE = 1
#
### OSTYPE: DOS16 for Windows 3.1 version, WIN32 for Windows 95/98/NT/2000
#   version
!if ("$(OSTYPE)"=="")
OSTYPE = WIN32
!endif
#
### DEBUG: Uncomment to make an executable for debugging
#DEBUG = -v
#
### CODEGUARD: Uncomment to use the CODEGUARD stuff (BC 5.0 or later):
#CODEGUARD = -vG
#
### CPU: set your target processor (3 to 6)
!if ("$(CPU)"=="")
CPU = 3
!endif
#
### Comment out to use precompiled headers (faster, but uses lots of disk!)
HEADERS = -H -H=vim.csm -Hc
#
### USEDLL: 0 for statically linked version of run-time, 1 for DLL runtime
!if ("$(USEDLL)"=="")
USEDLL = 1
!endif
#
### VIMDLL: 1 for a DLL version of VIM (NOT RECOMMENDED), 0 otherwise
#VIMDLL = 1
#
### ALIGN: alignment you desire: (1,2 or 4: s/b 4 for Win32, 2 for DOS)
!if ("$(ALIGN)"=="")
!if ($(OSTYPE)==DOS16)
ALIGN = 2
!else
ALIGN = 4
!endif
!endif
#
### FASTCALL: 1 to use FASTCALL calling convention (RECOMMENDED!), 0 otherwise
#   Incompatible when calling external functions (like MSVC-compiled DLLs), so
#   don't use FASTCALL when linking with external libs.
!if ("$(FASTCALL)"=="") && ("$(PYTHON)"=="") && ("$(PERL)"=="") && ("$(TCL)"=="")
FASTCALL = 1
!endif
#
### OPTIMIZE: SPEED to optimize for speed, SPACE otherwise (SPEED RECOMMENDED)
!if ("$(OPTIMIZE)"=="")
OPTIMIZE = SPEED
!endif
#
### Set the default $(WINVER) to make it work with Bcc 5.5.
!ifndef WINVER
WINVER = -DWINVER=0x0400 -D_WIN32_WINNT=0x0400
!endif
#
# Sanity checks for the above options:
#
!if ($(OSTYPE)==DOS16)
!if (($(CPU)+0)>4)
!error CPU Must be less than or equal to 4 for DOS16
!endif
#
!if (($(ALIGN)+0)>2)
!error ALIGN Must be less than or equal to 2 for DOS16
!endif
#
!else	# not DOS16
!if (($(CPU)+0)<3)
!error CPU Must be greater or equal to 3 for WIN32
!endif
!endif
#
!if ($(OSTYPE)!=WIN32) && ($(OSTYPE)!=DOS16)
!error Check the OSTYPE variable again: $(OSTYPE) is not supported!
!endif
#
# Optimizations: change as desired (RECOMMENDATION: Don't change!):
#
!ifdef DEBUG
OPT = -Od -N
!else
!if ($(OPTIMIZE)==SPACE)
OPT = -O1 -f- -N- -d
!else
OPT = -O2 -f- -d -N- -Oca -O
!endif
!if ($(FASTCALL)==1)
OPT = $(OPT) -pr
!endif
!ifndef CODEGUARD
OPT = $(OPT) -vi-
!endif
!endif
!if ($(OSTYPE)==DOS16)
!undef GUI
!undef VIMDLL
!undef USEDLL
!endif
# shouldn't have to change:
LIB = $(BOR)\lib
INCLUDE = $(BOR)\include;.;proto
DEFINES = -DWIN32 -DPC $(WINVER)
#
!ifdef PERL
DEFINES = $(DEFINES) -DFEAT_PERL
INCLUDE = $(PERL)\lib\core;$(INCLUDE)
!  ifndef PERL_VER
PERL_VER = 56
!  endif
!  if ("$(DYNAMIC_PERL)" == "yes")
!    if ($(PERL_VER) > 55)
DEFINES = $(DEFINES) -DDYNAMIC_PERL -DDYNAMIC_PERL_DLL=\"perl$(PERL_VER).dll\"
PERL_LIB_FLAG = /nodefaultlib:
!    else
!      message "Cannot dynamically load Perl versions less than 5.6.  Loading statically..."
!    endif
!  endif
!endif
#
!ifdef PYTHON
DEFINES = $(DEFINES) -DFEAT_PYTHON
INCLUDE = $(PYTHON)\include;$(INCLUDE)
!ifndef PYTHON_VER
PYTHON_VER = 15
!endif
!if "$(DYNAMIC_PYTHON)" == "yes"
DEFINES = $(DEFINES) -DDYNAMIC_PYTHON -DDYNAMIC_PYTHON_DLL=\"python$(PYTHON_VER).dll\"
PYTHON_LIB_FLAG = /nodefaultlib:
!endif
!endif
#
!ifdef TCL
DEFINES = $(DEFINES) -DFEAT_TCL
INCLUDE = $(TCL)\include;$(INCLUDE)
!ifndef TCL_VER
TCL_VER = 83
!endif
!endif
# 
# DO NOT change below:
#
CPU = -$(CPU)
ALIGN = -a$(ALIGN)
#
!ifdef DEBUG
DEFINES=$(DEFINES) -DDEBUG
!endif
#
!if ($(OLE)==1)
DEFINES = $(DEFINES) -DFEAT_OLE
!endif
#
!if ($(GUI)==1)
DEFINES = $(DEFINES) -DFEAT_GUI_W32 -DFEAT_CLIPBOARD
!ifdef DEBUG
TARGET = gvimd.exe
!else
TARGET = gvim.exe
!endif
!if ($(VIMDLL)==1)
EXETYPE=-WD
DEFINES = $(DEFINES) -DVIMDLL
!else
EXETYPE=-WE
!endif
STARTUPOBJ = c0w32.obj
LINK2 = -aa
RESFILE = vim.res
!else
!ifdef DEBUG
TARGET = vimd.exe
!else
# for now, anyway: VIMDLL is only for the GUI version
!undef VIMDLL
TARGET = vim.exe
!endif
!if ($(OSTYPE)==DOS16)
DEFINES=-DMSDOS
EXETYPE=-ml
STARTUPOBJ = c0l.obj
LINK2 =
!else
EXETYPE=-WC
STARTUPOBJ = c0x32.obj
LINK2 = -ap -OS -o -P
!endif
RESFILE =
!endif

!if ($(USEDLL)==1)
DEFINES = $(DEFINES) -D_RTLDLL
!endif

!ifdef DEBUG
OBJDIR	= $(OSTYPE)\objdbg
!else
!if ($(GUI)==1)
!if ($(OLE)==1)
OBJDIR	= $(OSTYPE)\oleobj
!else
OBJDIR	= $(OSTYPE)\gobj
!endif
!else
OBJDIR	= $(OSTYPE)\obj
!endif
!endif

##### BASE COMPILER/TOOLS RULES #####
MAKE = $(BOR)\bin\make
!if ($(OSTYPE)==DOS16)
BRC =
!if ("$(LINK)"=="")
LINK	= $(BOR)\BIN\TLink
!endif
CC   = $(BOR)\BIN\Bcc
LFLAGS	= -Tde -c -m -L$(LIB) $(DEBUG) $(LINK2)
LFLAGSDLL  =
CFLAGS = -w- -w-aus -w-par -I$(INCLUDE) -H- -P- $(HEADERS)
!else
BRC = $(BOR)\BIN\brc32
!if ("$(LINK)"=="")
LINK	= $(BOR)\BIN\ILink32
!endif
CC   = $(BOR)\BIN\Bcc32
LFLAGS	= -OS -r -Tpe -c -m -L$(LIB) $(DEBUG) $(LINK2)
LFLAGSDLL  = -Tpd -c -m -L$(LIB) $(DEBUG) $(LINK2)
CFLAGS = -w- -w-aus -w-par -I$(INCLUDE) -P- -d -x- -RT- -k- -Oi $(HEADERS) -N- -f-
!endif

CC1 = -c
CC2 = -o
CCARG = +$(OBJDIR)\bcc.cfg

# implicit rules:
.c.obj:
	$(CC) $(CCARG) $(CC1) $(CC2)$@ $*.c

.cpp.obj:
	$(CC) $(CCARG) $(CC1) $(CC2)$@ $*.cpp

!if ($(OSTYPE)==DOS16)
!else # win32:
vimmain = \
	$(OBJDIR)\os_w32exe.obj
!if ($(VIMDLL)==1)
vimwinmain = \
	$(OBJDIR)\os_w32dll.obj
!else
vimwinmain = \
	$(OBJDIR)\os_w32exe.obj
!endif
!endif

vimobj = $(vimwinmain) \
	$(OBJDIR)\buffer.obj \
	$(OBJDIR)\charset.obj \
	$(OBJDIR)\diff.obj \
	$(OBJDIR)\digraph.obj \
	$(OBJDIR)\edit.obj \
	$(OBJDIR)\eval.obj \
	$(OBJDIR)\ex_cmds.obj \
	$(OBJDIR)\ex_cmds2.obj \
	$(OBJDIR)\ex_docmd.obj \
	$(OBJDIR)\ex_getln.obj \
	$(OBJDIR)\fileio.obj \
	$(OBJDIR)\fold.obj \
	$(OBJDIR)\getchar.obj \
	$(OBJDIR)\main.obj \
	$(OBJDIR)\mark.obj \
	$(OBJDIR)\memfile.obj \
	$(OBJDIR)\memline.obj \
	$(OBJDIR)\menu.obj \
	$(OBJDIR)\message.obj \
	$(OBJDIR)\misc1.obj \
	$(OBJDIR)\misc2.obj \
	$(OBJDIR)\move.obj \
	$(OBJDIR)\mbyte.obj \
	$(OBJDIR)\normal.obj \
	$(OBJDIR)\ops.obj \
	$(OBJDIR)\option.obj \
	$(OBJDIR)\quickfix.obj \
	$(OBJDIR)\regexp.obj \
	$(OBJDIR)\screen.obj \
	$(OBJDIR)\search.obj \
	$(OBJDIR)\syntax.obj \
	$(OBJDIR)\tag.obj \
	$(OBJDIR)\term.obj \
	$(OBJDIR)\ui.obj \
	$(OBJDIR)\undo.obj \
	$(OBJDIR)\version.obj \
	$(OBJDIR)\window.obj

!if ($(OLE)==1)
vimobj = $(vimobj) \
	$(OBJDIR)\if_ole.obj
!endif

!ifdef PERL
vimobj = $(vimobj) \
    $(OBJDIR)\if_perl.obj
!endif

!ifdef PYTHON
vimobj = $(vimobj) \
    $(OBJDIR)\if_python.obj
!endif

!ifdef TCL
vimobj = $(vimobj) \
    $(OBJDIR)\if_tcl.obj
!endif

!if ($(VIMDLL)==1)
vimdllobj = $(vimobj)
!ifdef DEBUG
DLLTARGET = vim32d.dll
!else
DLLTARGET = vim32.dll
!endif
!else
DLLTARGET = joebob
!endif

!if ($(GUI)==1)
vimobj = $(vimobj) \
	$(OBJDIR)\gui.obj \
	$(OBJDIR)\gui_w32.obj
!endif

!if ($(OSTYPE)==WIN32)
vimobj = $(vimobj) \
	$(OBJDIR)\os_win32.obj $(OBJDIR)\os_mswin.obj
!elif ($(OSTYPE)==DOS16)
vimobj = $(vimobj) \
	$(OBJDIR)\os_msdos.obj
!endif
# Blab what we are going to do:
MSG = Compiling $(OSTYPE) $(TARGET) $(OLETARGET), with:
!if ($(GUI)==1)
MSG = $(MSG) GUI
!endif
!if ($(OLE)==1)
MSG = $(MSG) OLE
!endif
!if ($(USEDLL)==1)
MSG = $(MSG) USEDLL
!endif
!if ($(VIMDLL)==1)
MSG = $(MSG) VIMDLL
!endif
!if ($(FASTCALL)==1)
MSG = $(MSG) FASTCALL
!endif
!ifdef DEBUG
MSG = $(MSG) DEBUG
!endif
!ifdef CODEGUARD
MSG = $(MSG) CODEGUARD
!endif
!ifdef PERL
MSG = $(MSG) PERL
! ifdef DYNAMIC_PERL
MSG = $(MSG)(dynamic)
! endif
!endif
!ifdef PYTHON
MSG = $(MSG) PYTHON
! ifdef DYNAMIC_PYTHON
MSG = $(MSG)(dynamic)
! endif
!endif
!ifdef TCL
MSG = $(MSG) TCL
!endif
MSG = $(MSG) cpu=$(CPU)
MSG = $(MSG) Align=$(ALIGN)

!message $(MSG)

!if ($(OSTYPE)==DOS16)
TARGETS = $(TARGET)
!else
!if ($(VIMDLL)==1)
TARGETS = $(DLLTARGET)
!endif
TARGETS = $(TARGETS) $(TARGET)
!endif

# Targets:
all: vim vimrun.exe install.exe xxd uninstal.exe

vim: $(OSTYPE) $(OBJDIR) $(OBJDIR)\bcc.cfg $(TARGETS)
	@del $(OBJDIR)\version.obj

$(OSTYPE):
	-@md $(OSTYPE)

$(OBJDIR):
	-@md $(OBJDIR)

xxd:
	@cd xxd
	$(MAKE) /f Make_bc5.mak BOR="$(BOR)" BCC="$(CC)"
	@cd ..

install.exe: dosinst.c $(OBJDIR)\bcc.cfg
!if ($(OSTYPE)==WIN32)
	$(CC) $(CCARG) -WC -DWIN32 -einstall dosinst.c
!else
	$(CC) $(CCARG) -WC -einstall dosinst.c
!endif

uninstal.exe: uninstal.c $(OBJDIR)\bcc.cfg
!if ($(OSTYPE)==WIN32)
	$(CC) $(CCARG) -WC -DWIN32 -O2 -euninstal uninstal.c
!else
	$(CC) $(CCARG) -WC -O2 -euninstal uninstal.c
!endif

clean:
#	For Windows NT/2000, doesn't work on Windows 95/98...
	-@rmdir /s /q $(OBJDIR)
#	For Windows 95/98, doesn't work on Windows NT/2000...
	-@deltree /y $(OBJDIR)
	-@del *.res
	-@del *.dll
	-@del *vim*.exe
	-@del *install*.exe
	-@del *.csm
	-@del *.map
	-@del *.tds
	@cd xxd
	$(MAKE) /f Make_bc5.mak BOR="$(BOR)" clean
	@cd ..

$(DLLTARGET): $(OBJDIR) $(vimdllobj)
  $(LINK) @&&|
	$(LFLAGSDLL) +
	c0d32.obj +
	$(vimdllobj)
	$<,$*
!ifdef CODEGUARD
	cg32.lib+
!endif
!if ($(OLE)==1)
	ole2w32.lib +
!endif
!if ($(OSTYPE)==WIN32)
	import32.lib+
!ifdef PERL
	$(PERL_LIB_FLAG)perl.lib+
!endif
!ifdef PYTHON
	$(PYTHON_LIB_FLAG)python.lib+
!endif
!ifdef TCL
	tcl.lib+
!endif
!if ($(USEDLL)==1)
	cw32i.lib
!else
	cw32.lib
!endif
	vim.def
!else
	cl.lib
!endif
|

!if ($(VIMDLL)==1)
$(TARGET): $(OBJDIR) $(DLLTARGET) $(vimmain) $(RESFILE)
!else
$(TARGET): $(OBJDIR) $(vimobj) $(RESFILE)
!endif
  $(LINK) @&&|
	$(LFLAGS) +
	$(STARTUPOBJ) +
!if ($(VIMDLL)==1)
	$(vimmain)
!else
	$(vimobj)
!endif
	$<,$*
!if ($(OSTYPE)==WIN32)
!ifdef CODEGUARD
	cg32.lib+
!endif
!if ($(OLE)==1)
	ole2w32.lib +
!endif
	import32.lib+
!ifdef PERL
	$(PERL_LIB_FLAG)perl.lib+
!endif
!ifdef PYTHON
	$(PYTHON_LIB_FLAG)python.lib+
!endif
!ifdef TCL
	tcl.lib+
!endif
!if ($(USEDLL)==1)
	cw32i.lib
!else
	cw32.lib
!endif
!if ($(GUI)==1)

	$(RESFILE)
!endif
!else
	emu.lib + cl.lib
!endif
|

$(OBJDIR)\buffer.obj:  buffer.c

$(OBJDIR)\charset.obj:	charset.c

$(OBJDIR)\diff.obj:	diff.c

$(OBJDIR)\digraph.obj:	digraph.c

$(OBJDIR)\edit.obj:  edit.c

$(OBJDIR)\eval.obj:  eval.c

$(OBJDIR)\ex_cmds.obj:	ex_cmds.c

$(OBJDIR)\ex_cmds2.obj:	ex_cmds2.c

$(OBJDIR)\ex_docmd.obj:  ex_docmd.c ex_cmds.h

$(OBJDIR)\ex_getln.obj:  ex_getln.c

$(OBJDIR)\fileio.obj:  fileio.c

$(OBJDIR)\fold.obj:  fold.c

$(OBJDIR)\getchar.obj:	getchar.c

$(OBJDIR)\main.obj:  main.c

$(OBJDIR)\mark.obj:  mark.c

$(OBJDIR)\memfile.obj:	memfile.c

$(OBJDIR)\memline.obj:	memline.c

$(OBJDIR)\menu.obj:	menu.c

$(OBJDIR)\message.obj:	message.c

$(OBJDIR)\misc1.obj:  misc1.c

$(OBJDIR)\misc2.obj:  misc2.c

$(OBJDIR)\move.obj:  move.c

$(OBJDIR)\mbyte.obj:  mbyte.c

$(OBJDIR)\normal.obj:  normal.c

$(OBJDIR)\ops.obj:  ops.c

$(OBJDIR)\option.obj:  option.c

$(OBJDIR)\quickfix.obj:  quickfix.c

$(OBJDIR)\regexp.obj:  regexp.c

$(OBJDIR)\screen.obj:  screen.c

$(OBJDIR)\search.obj:  search.c

$(OBJDIR)\syntax.obj:  syntax.c

$(OBJDIR)\tag.obj:  tag.c

$(OBJDIR)\term.obj:  term.c

$(OBJDIR)\ui.obj:  ui.c

$(OBJDIR)\undo.obj:  undo.c

$(OBJDIR)\version.obj:	version.c

$(OBJDIR)\os_win32.obj:  os_win32.c

$(OBJDIR)\os_mswin.obj:  os_mswin.c

$(OBJDIR)\os_msdos.obj:  os_msdos.c

$(OBJDIR)\window.obj:  window.c

$(OBJDIR)\gui.obj: gui.c

$(OBJDIR)\gui_w32.obj: gui_w32.c

$(OBJDIR)\os_w32dll.obj: os_w32dll.c

$(OBJDIR)\if_ole.obj: if_ole.cpp

$(OBJDIR)\os_w32exe.obj: os_w32exe.c
	$(CC) $(CCARG) $(CC1) -I$(INCLUDE) -WE $(CC2)$@ os_w32exe.c

$(OBJDIR)\if_perl.obj: if_perl.c perl.lib
	$(CC) $(CCARG) $(CC1) $(CC2)$@ -pc if_perl.c

if_perl.c: if_perl.xs typemap
	$(PERL)\bin\perl.exe $(PERL)\lib\ExtUtils\xsubpp -prototypes -typemap \
	    $(PERL)\lib\ExtUtils\typemap if_perl.xs > $@

$(OBJDIR)\if_python.obj: if_python.c python.lib
	$(CC) $(CCARG) $(CC1) $(CC2)$@ -pc if_python.c

$(OBJDIR)\if_tcl.obj: if_tcl.c tcl.lib
	$(CC) $(CCARG) $(CC1) $(CC2)$@ -pc if_tcl.c

vim.res: vim.rc version.h tools.bmp tearoff.bmp vim.ico vim_error.ico vim_alert.ico vim_info.ico vim_quest.ico
    $(BRC) $(DEFINES) -i $(BOR)\include -w32 -r $*.rc

perl.lib: $(PERL)\lib\CORE\perl$(PERL_VER).lib
	coff2omf $(PERL)\lib\CORE\perl$(PERL_VER).lib $@

python.lib: $(PYTHON)\libs\python$(PYTHON_VER).lib
	coff2omf $(PYTHON)\libs\python$(PYTHON_VER).lib $@

tcl.lib: $(TCL)\lib\tcl$(TCL_VER).lib
	coff2omf $(TCL)\lib\tcl$(TCL_VER).lib $@

# vimrun.exe:
vimrun.exe: vimrun.c
!if ($(USEDLL)==1)
	$(CC) -WC -O1 -I$(INCLUDE) -L$(LIB) -D_RTLDLL vimrun.c cw32mti.lib
!else
	$(CC) -WC -O1 -I$(INCLUDE) -L$(LIB) vimrun.c
!endif


$(OBJDIR)\bcc.cfg: Make_bc5.mak
  copy &&|
	$(CFLAGS)
	-L$(LIB)
	$(DEFINES)
	$(EXETYPE)
	$(DEBUG)
	$(OPT)
	$(CODEGUARD)
	$(CPU)
	$(ALIGN)
| $@

# vi:set sts=4 sw=4:
