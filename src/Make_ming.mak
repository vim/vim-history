# Makefile for VIM on Win32, using 'EGCS/mingw32 1.1.2'.
# Info at http://www.mingw.org
# Also requires 'GNU make 3.77', which you can get through a link
# to 'JanJaap's page from the above page.
# Get missing libraries from http://gnuwin32.sf.net.
#
# Tested on Win32 NT 4 and Win95.
#
# To make everything, just 'make -f Make_ming.mak'
# To make just e.g. gvim.exe, 'make -f Make_ming.mak gvim.exe'
# After a run, you can 'make -f Make_ming.mak clean' to clean up
#
# NOTE: Sometimes 'GNU Make' will stop after building vimrun.exe -- I think
# it's just run out of memory or something.  Run again, and it will continue
# with 'xxd'.
#
# "make upx" makes *compressed* versions of the GUI and console EXEs, using the
# excellent UPX compressor:
#     http://upx.sourceforge.net/
#
# Maintained by Ron Aaron <ron@mossbayeng.com>
# updated 2003 Jan 20

#>>>>> choose options:
# set to yes for a debug build
DEBUG=no
# set to 1 for size, 0 for speed
OPTSIZE=0
# set to yes to make gvim, no for vim
GUI=yes
# FEATURES=[TINY | SMALL  | NORMAL | BIG | HUGE]
# set to TINY to make minimal version (few features)
FEATURES=BIG
# set to one of i386, i486, i586, i686 as the *target* processor
CPUNR=i686
# set to same choices as 'CPUNR', but will prevent running on 'lower' cpus:
ARCH=i386
# set to yes to cross-compile from unix; no=native Windows
CROSS=no
# set to path to iconv.h and libiconv.a to enable using 'iconv.dll'
#ICONV="."
# set to yes to include IME support
IME=no
# set to yes to enable writing a postscript file with :hardcopy
POSTSCRIPT=no
# set to yes to enable OLE support
OLE=no
#set to yes to enable Cscope support 
CSCOPE=yes

# Added by E.F. Amatria <eferna1@platea.ptic.mec.es> 2001 Feb 23
# Uncomment the first line and one of the following three if you want Native Language
# Support.  You'll need gnu_gettext.win32, a MINGW32 Windows PORT of gettext by
# Franco Bez <franco.bez@gmx.de>.  It may be found at
# http://home.a-city.de/franco.bez/gettext/gettext_win32_en.html
# Tested with mingw32 with GCC-2.95.2 on Win98
# Updated 2001 Jun 9
#GETTEXT=c:/gettext.win32.msvcrt
#STATIC_GETTEXT=USE_STATIC_GETTEXT
#DYNAMIC_GETTEXT=USE_GETTEXT_DLL
#DYNAMIC_GETTEXT=USE_SAFE_GETTEXT_DLL
SAFE_GETTEXT_DLL_OBJ = $(GETTEXT)/src/safe_gettext_dll/safe_gettext_dll.o
# Alternatively, if you uncomment the two following lines, you get a "safe" version
# without linking the safe_gettext_dll.o object file.
#DYNAMIC_GETTEXT=DYNAMIC_GETTEXT
#GETTEXT_DYNAMIC=gnu_gettext.dll
INTLPATH=$(GETTEXT)/lib/mingw32
INTLLIB=gnu_gettext

# If you are using gettext-0.10.35 from http://sourceforge.net/projects/gettext
# or gettext-0.10.37 from http://sourceforge.net/projects/mingwrep/
# uncomment the following, but I can't build a static versión with them, ?-(|
#GETTEXT=c:/gettext-0.10.37-20010430
#STATIC_GETTEXT=USE_STATIC_GETTEXT
#DYNAMIC_GETTEXT=DYNAMIC_GETTEXT
#INTLPATH=$(GETTEXT)/lib
#INTLLIB=intl

# uncomment 'PERL' if you want a perl-enabled version
#PERL=C:/perl
ifdef PERL
ifndef PERL_VER
PERL_VER=56
endif
ifndef DYNAMIC_PERL
DYNAMIC_PERL=yes
endif
# on Linux, for cross-compile, it's here:
#PERLLIB=/home/ron/ActivePerl/lib
# on NT, it's here:
PERLLIB=$(PERL)/lib
PERLLIBS=$(PERLLIB)/Core
endif

# Python support -- works with the ActiveState python 2.0 release (and others
# too, probably)
#
# uncomment 'PYTHON' to make python-enabled version
# Put the path to the python distro here.  If cross compiling from Linux, you
# will also need to convert the header files to unix instead of dos format:
#   for fil in *.h ; do vim -e -c 'set ff=unix|w|q' $fil
# and also, you will need to make a mingw32 'libpython20.a' to link with:
#   cd $PYTHON/libs
#   pexports python20.dll > python20.def
#   dlltool -d python20.def -l libpython20.a
# on my Linux box, I put the Python stuff here:
#PYTHON=/home/ron/ActivePython-2.0.0-202/src/Core
# on my NT box, it's here:
#PYTHON=c:/python20

ifdef PYTHON
ifndef DYNAMIC_PYTHON
DYNAMIC_PYTHON=yes
endif

ifndef PYTHON_VER
PYTHON_VER=20
endif

ifeq (no,$(DYNAMIC_PYTHON))
PYTHONLIB=-L$(PYTHON)/libs -lpython$(PYTHON_VER)
endif
# my include files are in 'win32inc' on Linux, and 'include' in the standard
# NT distro (ActiveState)
ifeq ($(CROSS),no)
PYTHONINC=-I $(PYTHON)/include
else
PYTHONINC=-I $(PYTHON)/win32inc
endif
endif

#	TCL interface:
#	  TCL=[Path to TCL directory]
#	  DYNAMIC_TCL=yes (to load the TCL DLL dynamically)
#	  TCL_VER=[TCL version, eg 83, 84] (default is 83)
#TCL=c:/tcl
ifdef TCL
ifndef DYNAMIC_TCL
DYNAMIC_TCL=yes
endif
ifndef TCL_VER
TCL_VER = 83
endif
TCLINC += -I$(TCL)/include
endif


#	Ruby interface:
#	  RUBY=[Path to Ruby directory]
#	  DYNAMIC_RUBY=yes (to load the Ruby DLL dynamically)
#	  RUBY_VER=[Ruby version, eg 16, 17] (default is 16)
#	  RUBY_VER_LONG=[Ruby version, eg 1.6, 1.7] (default is 1.6)
#	    You must set RUBY_VER_LONG when change RUBY_VER.
#RUBY=c:/ruby
ifdef RUBY
ifndef DYNAMIC_RUBY
DYNAMIC_RUBY=yes
endif
#  Set default value
ifndef RUBY_VER
RUBY_VER = 16
endif
ifndef RUBY_VER_LONG
RUBY_VER_LONG = 1.6
endif
ifndef RUBY_PLATFORM
RUBY_PLATFORM = i586-mswin32
endif
RUBY_INSTALL_NAME = mswin32-ruby$(RUBY_VER)

RUBYINC =-I $(RUBY)/lib/ruby/$(RUBY_VER_LONG)/$(RUBY_PLATFORM)
ifeq (no, $(DYNAMIC_RUBY))
RUBYLIB = -L$(RUBY)/lib -l$(RUBY_INSTALL_NAME)
endif

endif # RUBY

# See feature.h for a list of options.
# Any other defines can be included here.
DEF_GUI=-DFEAT_GUI_W32 -DFEAT_CLIPBOARD -DFEAT_BIG
DEF_MIN=-DFEAT_SMALL
DEFINES=-DWIN32 -DPC
ifeq ($(CROSS),yes)
# cross-compiler:
CC = i586-pc-mingw32msvc-gcc
DEL = rm
WINDRES = i586-pc-mingw32msvc-windres
else
# normal (Windows) compilation:
CC = gcc
ifneq (sh.exe, $(SHELL))
DEL = rm
else
DEL = del
endif
WINDRES = windres
endif

#>>>>> end of choices
###########################################################################

CFLAGS = -Iproto $(DEFINES) -pipe -malign-double -w
CFLAGS += -march=$(ARCH) -mcpu=$(CPUNR) -Wall -DFEAT_$(FEATURES)

ifdef GETTEXT
DEFINES +=-DHAVE_GETTEXT -DHAVE_LOCALE_H
GETTEXTINCLUDE = $(GETTEXT)/include
GETTEXTLIB = $(INTLPATH)
ifdef DYNAMIC_GETTEXT
DEFINES +=-D$(DYNAMIC_GETTEXT)
ifdef GETTEXT_DYNAMIC
DEFINES += -DGETTEXT_DYNAMIC -DGETTEXT_DLL=\"$(GETTEXT_DYNAMIC)\"
endif
endif
endif

ifdef PERL
CFLAGS += -I$(PERLLIBS) -DFEAT_PERL -L$(PERLLIBS)
ifeq (yes, $(DYNAMIC_PERL))
CFLAGS += -DDYNAMIC_PERL -DDYNAMIC_PERL_DLL=\"perl$(PERL_VER).dll\"
endif
endif

ifdef RUBY
CFLAGS += -DFEAT_RUBY $(RUBYINC)
ifeq (yes, $(DYNAMIC_RUBY))
CFLAGS += -DDYNAMIC_RUBY -DDYNAMIC_RUBY_DLL=\"$(RUBY_INSTALL_NAME).dll\"
endif
endif

ifdef PYTHON
CFLAGS += -DFEAT_PYTHON $(PYTHONINC)
ifeq (yes, $(DYNAMIC_PYTHON))
CFLAGS += -DDYNAMIC_PYTHON -DDYNAMIC_PYTHON_DLL=\"python$(PYTHON_VER).dll\"
endif
endif

ifdef TCL
CFLAGS += -DFEAT_TCL $(TCLINC)
ifeq (yes, $(DYNAMIC_TCL))
CFLAGS += -DDYNAMIC_TCL -DDYNAMIC_TCL_DLL=\"tcl$(TCL_VER).dll\"
endif
endif

ifeq ($(POSTSCRIPT),yes)
CFLAGS += -DMSWINPS
endif

ifeq (yes, $(OLE))
CFLAGS += -DFEAT_OLE
endif

ifeq ($(CSCOPE),yes)
CFLAGS += -DFEAT_CSCOPE
endif

ifeq ($(DEBUG),yes)
CFLAGS += -g -fstack-check
else
CFLAGS += -s
CFLAGS += -fomit-frame-pointer -freg-struct-return
CFLAGS += -malign-double -finline-functions
ifeq ($(OPTSIZE),1)
CFLAGS += -Os
else
CFLAGS += -O3
endif
endif
ifeq ($(GUI),yes)
TARGET := gvim.exe
else
TARGET := vim.exe
endif
ifeq ($(MIN),yes)
DEFINES += $(DEF_MIN)
endif

GUISRC =  vimres.c gui.c gui_w32.c
SRC    =  os_w32exe.c buffer.c charset.c diff.c digraph.c edit.c eval.c ex_cmds.c \
	  ex_cmds2.c ex_docmd.c ex_eval.c ex_getln.c fileio.c fold.c getchar.c main.c \
	  mark.c memfile.c memline.c menu.c message.c misc1.c misc2.c move.c \
	  mbyte.c normal.c ops.c option.c os_win32.c os_mswin.c \
	  quickfix.c regexp.c screen.c search.c syntax.c tag.c term.c ui.c \
	  undo.c window.c version.c
ifdef PERL
SRC += if_perl.c
endif
ifdef PYTHON
SRC += if_python.c
endif
ifdef RUBY
SRC += if_ruby.c
endif
ifdef TCL
SRC += if_tcl.c
endif
ifeq ($(CSCOPE),yes)
SRC += if_cscope.c
endif


GUIOBJ = $(GUISRC:.c=.o)
OBJ    = $(SRC:.c=.o)
LIB = -lkernel32 -luser32 -lgdi32 -ladvapi32 -lcomdlg32 -lcomctl32 -lole32 -luuid

ifdef GETTEXT
CFLAGS += -I$(GETTEXTINCLUDE)
ifndef STATIC_GETTEXT
LIB += -L$(GETTEXTLIB) -l$(INTLLIB)
ifeq (USE_SAFE_GETTEXT_DLL, $(DYNAMIC_GETTEXT))
OBJ+=$(SAFE_GETTEXT_DLL_OBJ)
endif
else
LIB += -L$(GETTEXTLIB) -lintl
endif
endif

ifdef PERL
ifeq (no, $(DYNAMIC_PERL))
LIB += -lperl$(PERL_VER)
endif
endif

ifdef TCL
LIB += -L$(TCL)/lib
ifeq (yes, $(DYNAMIC_TCL))
LIB += -ltclstub$(TCL_VER)
else
LIB += -ltcl$(TCL_VER)
endif
endif

ifeq (yes, $(OLE))
LIB += -loleaut32 -lstdc++
OBJ += if_ole.o
endif

ifeq (yes, $(IME))
DEFINES += -DFEAT_MBYTE_IME -DDYNAMIC_IME
LIB += -limm32
endif

ifdef ICONV
LIB += -L$(ICONV)
DEFINES+=-DDYNAMIC_ICONV
CFLAGS += -I$(ICONV)
endif

all: $(TARGET) vimrun.exe xxd/xxd.exe install.exe uninstal.exe GvimExt/gvimext.dll

vimrun.exe: vimrun.c
	$(CC) $(CFLAGS) -s -o vimrun.exe vimrun.c $(LIB)

install.exe: dosinst.c
	$(CC) $(CFLAGS) -s -o install.exe dosinst.c $(LIB) -lole32 -luuid

uninstal.exe: uninstal.c
	$(CC) $(CFLAGS) -s -o uninstal.exe uninstal.c $(LIB)

vim.exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

gvim.exe: DEFINES+=$(DEF_GUI)
gvim.exe: $(OBJ) $(GUIOBJ)
	$(CC) $(DEF_GUI) $(CFLAGS) -o $@ $^ -mwindows $(LIB) $(PYTHONLIB) $(RUBYLIB)

exes:
	-$(DEL) *.o
	$(MAKE) -f Make_ming.mak gvim.exe
	-$(DEL) *.o
	$(MAKE) -f Make_ming.mak vim.exe
	-$(DEL) *.o

upx: exes
	upx gvim.exe
	upx vim.exe

xxd/xxd.exe: xxd/xxd.c
	$(CC) $(CFLAGS) -o xxd/xxd.exe -s -DWIN32 xxd/xxd.c $(LIB)

GvimExt/gvimext.dll: GvimExt/gvimext.cpp GvimExt/gvimext.rc GvimExt/gvimext.h
	cd GvimExt; $(MAKE) -f Make_ming.mak; cd ..

clean:
	-$(DEL) *.o
	-$(DEL) *.res
	-$(DEL) *.exe
ifneq (sh.exe, $(SHELL))
	-$(DEL) xxd/*.exe
else
	-$(DEL) xxd\*.exe
endif
ifdef PERL
	-$(DEL) if_perl.c
endif

###########################################################################
vimres.res: vim.rc
	$(WINDRES) --define MING --define FEAT_GUI_W32 vim.rc vimres.res

vimres.o: vimres.res
	$(WINDRES) vimres.res vimres.o

INCL = vim.h feature.h os_win32.h os_dos.h ascii.h keymap.h term.h macros.h \
	structs.h regexp.h option.h ex_cmds.h proto.h globals.h farsi.h \
	gui.h

$(OBJ) $(GUIOBJ): $(INCL)

if_ole.o: if_ole.cpp
	$(CC) $(CFLAGS) -D__IID_DEFINED__ -c -o if_ole.o if_ole.cpp

if_ruby.o: if_ruby.c
	$(CC) $(CFLAGS) -U_WIN32 -c -o if_ruby.o if_ruby.c

if_perl.c: if_perl.xs typemap
	perl $(PERLLIB)/ExtUtils/xsubpp -prototypes -typemap $(PERLLIB)/ExtUtils/typemap if_perl.xs > $@
