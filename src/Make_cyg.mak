#
# Makefile for VIM on Win32, using Cygnus gcc
#
# This compiles Vim as a Windows application.  If you want Vim to run as a
# Cygwin application use the Makefile (just like on Unix).
#
# Last updated by Dan Sharp.  Last Change: 2003 Apr 21
#
# GUI		no or yes: set to yes if you want the GUI version (yes)
# PERL		define to path to Perl dir to get Perl support (not defined)
#   PERL_VER	  define to version of Perl being used (56)
#   DYNAMIC_PERL  no or yes: set to yes to load the Perl DLL dynamically (yes)
# PYTHON	define to path to Python dir to get PYTHON support (not defined)
#   PYTHON_VER	    define to version of Python being used (22)
#   DYNAMIC_PYTHON  no or yes: use yes to load the Python DLL dynamically (yes)
# TCL		define to path to TCL dir to get TCL support (not defined)
#   TCL_VER	define to version of TCL being used (83)
#   DYNAMIC_TCL no or yes: use yes to load the TCL DLL dynamically (yes)
# RUBY		define to path to Ruby dir to get Ruby support (not defined)
#   RUBY_VER	define to version of Ruby being used (16)
#   DYNAMIC_RUBY no or yes: use yes to load the Ruby DLL dynamically (yes)
# GETTEXT	no or yes: set to yes for dynamic gettext support (yes)
# ICONV		no or yes: set to yes for dynamic iconv support (yes)
# IME		no or yes: set to yes to include IME support (no)
# OLE		no or yes: set to yes to make OLE gvim (no)
# DEBUG		no or yes: set to yes if you wish a DEBUGging build (no)
# CPUNR		i386 through pentium4: select -mcpu argument to compile with (i386)
# ARCH		i386 through pentium4: select -march argument to compile with (i386)
# USEDLL	no or yes: set to yes to use the Runtime library DLL (no)
#		For USEDLL=yes the cygwin1.dll is required to run Vim.
# POSTSCRIPT	no or yes: set to yes for PostScript printing (no)
# FEATURES	TINY, SMALL, NORMAL, BIG or HUGE (BIG)
#>>>>> choose options:
ifndef GUI
GUI=yes
endif

ifndef FEATURES
FEATURES = BIG
endif

ifndef GETTEXT
GETTEXT = yes
endif

ifndef ICONV
ICONV = yes
endif

ifndef IME
IME = no
endif

ifndef USEDLL
USEDLL = no
endif

ifndef CPUNR
CPUNR = i386
endif

ifndef ARCH
ARCH = i386
endif

### See feature.h for a list of optionals.
### Any other defines can be included here.

DEFINES = -DHAVE_PATHDEF -DFEAT_$(FEATURES)
INCLUDES = -mcpu=$(CPUNR) -march=$(ARCH)

#>>>>> name of the compiler and linker, name of lib directory
CC = gcc
RC = windres

##############################
# DYNAMIC_PERL=yes and no both work
##############################
ifdef PERL
PERL_OBJ = $(OUTDIR)/if_perl.o
DEFINES += -DFEAT_PERL
ifndef PERL_VER
PERL_VER = 56
endif
ifeq (yes, $(DYNAMIC_PERL))
DEFINES += -DDYNAMIC_PERL -DDYNAMIC_PERL_DLL=\"perl$(PERL_VER).dll\"
endif
INCLUDES += -I$(PERL)/lib/CORE
EXTRA_LIBS += $(PERL)/lib/CORE/perl$(PERL_VER).lib
endif

##############################
# DYNAMIC_PYTHON=yes works.
# DYNAMIC_PYTHON=no does not (unresolved externals on link).
##############################
ifdef PYTHON
PYTHON_OBJ = $(OUTDIR)/if_python.o
DEFINES += -DFEAT_PYTHON
ifndef PYTHON_VER
PYTHON_VER = 22
endif
ifeq (yes, $(DYNAMIC_PYTHON))
DEFINES += -DDYNAMIC_PYTHON -DDYNAMIC_PYTHON_DLL=\"python$(PYTHON_VER).dll\"
endif
INCLUDES += -I$(PYTHON)/include
EXTRA_LIBS += $(PYTHON)/libs/python$(PYTHON_VER).lib
endif

##############################
# DYNAMIC_RUBY=yes works.
# DYNAMIC_RUBY=no does not (process exits).
##############################
ifdef RUBY
ifndef RUBY_VER_LONG
RUBY_VER_LONG=1.6
endif
ifndef RUBY_VER
RUBY_VER=16
endif
RUBY_OBJ = $(OUTDIR)/if_ruby.o
DEFINES += -DFEAT_RUBY
INCLUDES += -I$(RUBY)/lib/ruby/$(RUBY_VER_LONG)/i586-mswin32
EXTRA_LIBS += $(RUBY)/lib/mswin32-ruby$(RUBY_VER).lib
ifeq (yes, $(DYNAMIC_RUBY))
DEFINES += -DDYNAMIC_RUBY -DDYNAMIC_RUBY_DLL=\"mswin32-ruby$(RUBY_VER).dll\"
endif
endif

##############################
# DYNAMIC_TCL=yes and no both work.
##############################
ifdef TCL
DEFINES += -DFEAT_TCL 
TCL_OBJ = $(OUTDIR)/if_tcl.o
ifndef TCL_VER
TCL_VER = 83
endif
INCLUDES += -I$(TCL)/include
ifeq (yes, $(DYNAMIC_TCL))
DEFINES += -DDYNAMIC_TCL -DDYNAMIC_TCL_DLL=\"tcl$(TCL_VER).dll\"
EXTRA_LIBS += $(TCL)/lib/tclstub$(TCL_VER).lib
else
EXTRA_LIBS += $(TCL)/lib/tcl$(TCL_VER).lib
endif
endif

##############################
ifeq (yes, $(GETTEXT))
DEFINES += -DDYNAMIC_GETTEXT
endif

##############################
ifeq (yes, $(ICONV))
DEFINES += -DDYNAMIC_ICONV
endif

##############################
ifeq (yes, $(IME))
DEFINES += -DFEAT_MBYTE_IME -DDYNAMIC_IME
EXTRA_LIBS += -limm32
endif

##############################
ifeq (yes, $(DEBUG))
DEFINES += -DDEBUG
INCLUDES += -g -fstack-check
DEBUG_SUFFIX = d
else
INCLUDES += -s -fomit-frame-pointer -freg-struct-return \
	    -malign-double -finline-functions
endif

##############################
# USEDLL=yes will build a Cygwin32 executable that relies on cygwin1.dll.
# USEDLL=no will build a Mingw32 executable with no extra dll dependencies.
##############################
ifeq (no, $(USEDLL))
INCLUDES += -mno-cygwin
else
DEFINES += -D__CYGWIN__
endif

##############################
ifeq (yes, $(POSTSCRIPT))
DEFINES += -DMSWINPS
endif

##############################
ifeq (yes, $(OLE))
INCLUDES += -DFEAT_OLE
OLE_OBJ = $(OUTDIR)/if_ole.o
EXTRA_LIBS += -loleaut32 -lstdc++
endif

##############################
ifeq ($(GUI),yes)
EXE = gvim$(DEBUG_SUFFIX).exe
OUTDIR = gobj$(DEBUG_SUFFIX)
DEFINES += -DFEAT_GUI_W32 -DFEAT_CLIPBOARD
GUI_OBJ = $(OUTDIR)/gui.o $(OUTDIR)/gui_w32.o $(OUTDIR)/os_w32exe.o $(OUTDIR)/vimrc.o
EXTRA_LIBS += -mwindows -lcomctl32
else
EXE = vim$(DEBUG_SUFFIX).exe
OUTDIR = obj$(DEBUG_SUFFIX)
LIBS += -luser32 -lgdi32 -lcomdlg32
endif

##############################
ifneq (sh.exe, $(SHELL))
DEL = rm
else
DEL = del
endif

#>>>>> end of choices
###########################################################################

INCL = vim.h globals.h option.h keymap.h macros.h ascii.h term.h os_win32.h \
       structs.h version.h
CFLAGS = -O2 -D_MAX_PATH=256 -DWIN32 -DPC -Iproto $(DEFINES) $(INCLUDES)
RCFLAGS = -D_MAX_PATH=256 -DWIN32 -DPC -O coff $(DEFINES)

OBJ = \
	$(OUTDIR)/buffer.o \
	$(OUTDIR)/charset.o \
	$(OUTDIR)/diff.o \
	$(OUTDIR)/digraph.o \
	$(OUTDIR)/edit.o \
	$(OUTDIR)/eval.o \
	$(OUTDIR)/ex_cmds.o \
	$(OUTDIR)/ex_cmds2.o \
	$(OUTDIR)/ex_docmd.o \
	$(OUTDIR)/ex_eval.o \
	$(OUTDIR)/ex_getln.o \
	$(OUTDIR)/fileio.o \
	$(OUTDIR)/fold.o \
	$(OUTDIR)/getchar.o \
	$(OUTDIR)/main.o \
	$(OUTDIR)/mark.o \
	$(OUTDIR)/memfile.o \
	$(OUTDIR)/memline.o \
	$(OUTDIR)/menu.o \
	$(OUTDIR)/message.o \
	$(OUTDIR)/misc1.o \
	$(OUTDIR)/misc2.o \
	$(OUTDIR)/move.o \
	$(OUTDIR)/mbyte.o \
	$(OUTDIR)/normal.o \
	$(OUTDIR)/ops.o \
	$(OUTDIR)/option.o \
	$(OUTDIR)/os_win32.o \
	$(OUTDIR)/os_mswin.o \
	$(OUTDIR)/pathdef.o \
	$(OUTDIR)/quickfix.o \
	$(OUTDIR)/regexp.o \
	$(OUTDIR)/screen.o \
	$(OUTDIR)/search.o \
	$(OUTDIR)/syntax.o \
	$(OUTDIR)/tag.o \
	$(OUTDIR)/term.o \
	$(OUTDIR)/ui.o \
	$(OUTDIR)/undo.o \
	$(OUTDIR)/version.o \
	$(OUTDIR)/window.o \
	$(GUI_OBJ) \
	$(OLE_OBJ) \
	$(PERL_OBJ) \
	$(PYTHON_OBJ) \
	$(RUBY_OBJ) \
	$(TCL_OBJ)

all: $(EXE) xxd/xxd.exe vimrun.exe install.exe uninstal.exe

# According to the Cygwin doc 1.2 FAQ, kernel32 should not be specified for
# linking unless calling ld directly.  
# See /usr/doc/cygwin-doc-1.2/html/faq_toc.html#TOC93 for more information.
$(EXE): $(OUTDIR) $(OBJ)
	$(CC) $(CFLAGS) -s -o $(EXE) $(OBJ) $(LIBS) -luuid -lole32 $(EXTRA_LIBS)

xxd/xxd.exe: xxd/xxd.c
	cd xxd ; $(MAKE) -f Make_cyg.mak USEDLL=$(USEDLL); cd ..

vimrun.exe: vimrun.c
	$(CC) $(CFLAGS) -s -o vimrun.exe vimrun.c  $(LIBS)

install.exe: dosinst.c
	$(CC) $(CFLAGS) -s -o install.exe dosinst.c  $(LIBS) -luuid -lcomctl32 -lole32

uninstal.exe: uninstal.c
	$(CC) $(CFLAGS) -s -o uninstal.exe uninstal.c $(LIBS)

$(OUTDIR):
	mkdir $(OUTDIR)

tags:
	command /c ctags *.c $(INCL)

clean:
ifneq (sh.exe, $(SHELL))
	-$(DEL) $(OUTDIR)/*.o
else
	-$(DEL) $(OUTDIR)\*.o
endif
	-$(DEL) *.o
	-$(DEL) *.exe
	-$(DEL) *.~
	-$(DEL) *~
	-$(DEL) if_perl.c
	-$(DEL) pathdef.c
	cd xxd ; $(MAKE) -f Make_cyg.mak clean; cd ..

###########################################################################

$(OUTDIR)/buffer.o:	buffer.c $(INCL)
	$(CC) -c $(CFLAGS) buffer.c -o $(OUTDIR)/buffer.o

$(OUTDIR)/charset.o:	charset.c $(INCL)
	$(CC) -c $(CFLAGS) charset.c -o $(OUTDIR)/charset.o

$(OUTDIR)/diff.o:	diff.c $(INCL)
	$(CC) -c $(CFLAGS) diff.c -o $(OUTDIR)/diff.o

$(OUTDIR)/digraph.o:	digraph.c $(INCL)
	$(CC) -c $(CFLAGS) digraph.c -o $(OUTDIR)/digraph.o

$(OUTDIR)/edit.o:	edit.c $(INCL)
	$(CC) -c $(CFLAGS) edit.c -o $(OUTDIR)/edit.o

$(OUTDIR)/eval.o:	eval.c $(INCL)
	$(CC) -c $(CFLAGS) eval.c -o $(OUTDIR)/eval.o

$(OUTDIR)/ex_cmds.o:	ex_cmds.c $(INCL)
	$(CC) -c $(CFLAGS) ex_cmds.c -o $(OUTDIR)/ex_cmds.o

$(OUTDIR)/ex_cmds2.o:	ex_cmds2.c $(INCL)
	$(CC) -c $(CFLAGS) ex_cmds2.c -o $(OUTDIR)/ex_cmds2.o

$(OUTDIR)/ex_docmd.o:	ex_docmd.c $(INCL) ex_cmds.h
	$(CC) -c $(CFLAGS) ex_docmd.c -o $(OUTDIR)/ex_docmd.o

$(OUTDIR)/ex_eval.o:	ex_eval.c $(INCL) ex_cmds.h
	$(CC) -c $(CFLAGS) ex_eval.c -o $(OUTDIR)/ex_eval.o

$(OUTDIR)/ex_getln.o:	ex_getln.c $(INCL)
	$(CC) -c $(CFLAGS) ex_getln.c -o $(OUTDIR)/ex_getln.o

$(OUTDIR)/fileio.o:	fileio.c $(INCL)
	$(CC) -c $(CFLAGS) fileio.c -o $(OUTDIR)/fileio.o

$(OUTDIR)/fold.o:	fold.c $(INCL)
	$(CC) -c $(CFLAGS) fold.c -o $(OUTDIR)/fold.o

$(OUTDIR)/getchar.o:	getchar.c $(INCL)
	$(CC) -c $(CFLAGS) getchar.c -o $(OUTDIR)/getchar.o

$(OUTDIR)/gui.o:	gui.c $(INCL)
	$(CC) -c $(CFLAGS) gui.c -o $(OUTDIR)/gui.o

$(OUTDIR)/gui_w32.o:	gui_w32.c $(INCL)
	$(CC) -c $(CFLAGS) gui_w32.c -o $(OUTDIR)/gui_w32.o

$(OUTDIR)/if_ole.o:	if_ole.cpp $(INCL)
	$(CC) -c $(CFLAGS) -D__IID_DEFINED__ if_ole.cpp -o $(OUTDIR)/if_ole.o

if_perl.c: if_perl.xs typemap
	perl $(PERL)/lib/ExtUtils/xsubpp -prototypes -typemap \
	     $(PERL)/lib/ExtUtils/typemap if_perl.xs > $@

$(OUTDIR)/if_perl.o:	if_perl.c $(INCL)
ifeq (no, $(USEDLL))
	$(CC) -c $(CFLAGS) if_perl.c -o $(OUTDIR)/if_perl.o
else
	$(CC) -c $(CFLAGS) -I/usr/include/mingw -D__MINGW32__ if_perl.c -o $(OUTDIR)/if_perl.o
endif

$(OUTDIR)/if_python.o:	if_python.c $(INCL)
	$(CC) -c $(CFLAGS) if_python.c -o $(OUTDIR)/if_python.o

$(OUTDIR)/if_ruby.o:	if_ruby.c $(INCL)
ifeq (no, $(USEDLL))
	$(CC) -c $(CFLAGS) -U_WIN32 if_ruby.c -o $(OUTDIR)/if_ruby.o
else
	$(CC) -c $(CFLAGS) if_ruby.c -o $(OUTDIR)/if_ruby.o
endif

$(OUTDIR)/if_tcl.o:	if_tcl.c $(INCL)
	$(CC) -c $(CFLAGS) if_tcl.c -o $(OUTDIR)/if_tcl.o

$(OUTDIR)/main.o:	main.c $(INCL)
	$(CC) -c $(CFLAGS) main.c -o $(OUTDIR)/main.o

$(OUTDIR)/mark.o:	mark.c $(INCL)
	$(CC) -c $(CFLAGS) mark.c -o $(OUTDIR)/mark.o

$(OUTDIR)/memfile.o:	memfile.c $(INCL)
	$(CC) -c $(CFLAGS) memfile.c -o $(OUTDIR)/memfile.o

$(OUTDIR)/memline.o:	memline.c $(INCL)
	$(CC) -c $(CFLAGS) memline.c -o $(OUTDIR)/memline.o

$(OUTDIR)/menu.o:	menu.c $(INCL)
	$(CC) -c $(CFLAGS) menu.c -o $(OUTDIR)/menu.o

$(OUTDIR)/message.o:	message.c $(INCL)
	$(CC) -c $(CFLAGS) message.c -o $(OUTDIR)/message.o

$(OUTDIR)/misc1.o:	misc1.c $(INCL)
	$(CC) -c $(CFLAGS) misc1.c -o $(OUTDIR)/misc1.o

$(OUTDIR)/misc2.o:	misc2.c $(INCL)
	$(CC) -c $(CFLAGS) misc2.c -o $(OUTDIR)/misc2.o

$(OUTDIR)/move.o:	move.c $(INCL)
	$(CC) -c $(CFLAGS) move.c -o $(OUTDIR)/move.o

$(OUTDIR)/mbyte.o: mbyte.c $(INCL)
	$(CC) -c $(CFLAGS) mbyte.c -o $(OUTDIR)/mbyte.o

$(OUTDIR)/normal.o:	normal.c $(INCL)
	$(CC) -c $(CFLAGS) normal.c -o $(OUTDIR)/normal.o

$(OUTDIR)/ops.o:	ops.c $(INCL)
	$(CC) -c $(CFLAGS) ops.c -o $(OUTDIR)/ops.o

$(OUTDIR)/option.o:	option.c $(INCL)
	$(CC) -c $(CFLAGS) option.c -o $(OUTDIR)/option.o

$(OUTDIR)/os_win32.o:	os_win32.c $(INCL)
	$(CC) -c $(CFLAGS) os_win32.c -o $(OUTDIR)/os_win32.o

$(OUTDIR)/os_w32exe.o: os_w32exe.c $(INCL)
	$(CC) -c $(CFLAGS) os_w32exe.c -o $(OUTDIR)/os_w32exe.o

$(OUTDIR)/os_mswin.o:	os_mswin.c $(INCL)
	$(CC) -c $(CFLAGS) os_mswin.c -o $(OUTDIR)/os_mswin.o

$(OUTDIR)/pathdef.o:	pathdef.c $(INCL)
	$(CC) -c $(CFLAGS) pathdef.c -o $(OUTDIR)/pathdef.o

$(OUTDIR)/quickfix.o:	quickfix.c $(INCL)
	$(CC) -c $(CFLAGS) quickfix.c -o $(OUTDIR)/quickfix.o

$(OUTDIR)/regexp.o:	regexp.c $(INCL)
	$(CC) -c $(CFLAGS) regexp.c -o $(OUTDIR)/regexp.o

$(OUTDIR)/screen.o:	screen.c $(INCL)
	$(CC) -c $(CFLAGS) screen.c -o $(OUTDIR)/screen.o

$(OUTDIR)/search.o:	search.c $(INCL)
	$(CC) -c $(CFLAGS) search.c -o $(OUTDIR)/search.o

$(OUTDIR)/syntax.o:	syntax.c $(INCL)
	$(CC) -c $(CFLAGS) syntax.c -o $(OUTDIR)/syntax.o

$(OUTDIR)/tag.o:	tag.c $(INCL)
	$(CC) -c $(CFLAGS) tag.c -o $(OUTDIR)/tag.o

$(OUTDIR)/term.o:	term.c $(INCL)
	$(CC) -c $(CFLAGS) term.c -o $(OUTDIR)/term.o

$(OUTDIR)/ui.o:	ui.c $(INCL)
	$(CC) -c $(CFLAGS) ui.c -o $(OUTDIR)/ui.o

$(OUTDIR)/undo.o:	undo.c $(INCL)
	$(CC) -c $(CFLAGS) undo.c -o $(OUTDIR)/undo.o

$(OUTDIR)/version.o:	version.c $(INCL)
	$(CC) -c $(CFLAGS) version.c -o $(OUTDIR)/version.o

$(OUTDIR)/vimrc.o:	vim.rc $(INCL)
	$(RC) $(RCFLAGS) vim.rc -o $(OUTDIR)/vimrc.o

$(OUTDIR)/window.o:	window.c $(INCL)
	$(CC) -c $(CFLAGS) window.c -o $(OUTDIR)/window.o

pathdef.c:
	@echo creating pathdef.c
	@echo '/* pathdef.c */' > pathdef.c
	@echo '#include "vim.h"' >> pathdef.c
	@echo 'char_u *default_vim_dir = (char_u *)"$(VIMRCLOC)";' >> pathdef.c
	@echo 'char_u *default_vimruntime_dir = (char_u *)"$(VIMRUNTIMEDIR)";' >> pathdef.c
	@echo 'char_u *all_cflags = (char_u *)"$(CC) $(CFLAGS)";' >> pathdef.c
	@echo 'char_u *all_lflags = (char_u *)"$(CC) -s -o $(EXE) $(LIBS) -luuid -lole32 $(EXTRA_LIBS)";' >> pathdef.c
	@echo 'char_u *compiled_user = (char_u *)"$(USERNAME)";' >> pathdef.c
	@echo 'char_u *compiled_sys = (char_u *)"$(USERDOMAIN)";' >> pathdef.c
