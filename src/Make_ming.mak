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
# set to '1' for a debug build
DEBUG=0
# set to 1 for size, 0 for speed
OPTSIZE=0
# set to 1 to make gvim, 0 for vim
GUI=1
# FEATURES=[TINY | SMALL  | NORMAL | BIG | HUGE]
# set to TINY to make minimal version (few features)
FEATURES=BIG
# set to one of i386, i486, i586, i686 as the *target* processor
CPUNR=i686
# set to same choices as 'CPUNR', but will prevent running on 'lower' cpus:
ARCH=i386
# set to '1' to cross-compile from unix; 0=native Windows
CROSS=0
# set to path to iconv.h and libiconv.a to enable using 'iconv.dll'
#ICONV="."
# set to 1 to enable writing a postscript file with :hardcopy
POSTSCRIPT=0

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
#PERL=perl
ifdef PERL
ifndef PERL_VER
PERL_VER=56
endif
endif
DYNAMIC_PERL=perl$(PERL_VER).dll
# on Linux, for cross-compile, it's here:
#PERLLIB=/home/ron/ActivePerl/lib
# on NT, it's here:
PERLLIB=c:/perl/lib
PERLLIBS=$(PERLLIB)/Core

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

# to make a vim version that autoloads python if available.  You do want this!
PYTHON_VER=20
DYNAMIC_PYTHON=python$(PYTHON_VER).dll

ifdef PYTHON
ifndef DYNAMIC_PYTHON
PYTHONLIB=-L$(PYTHON)/libs -lpython$(PYTHON_VER)
endif
# my include files are in 'win32inc' on Linux, and 'include' in the standard
# NT distro (ActiveState)
ifeq ($(CROSS),0)
PYTHONINC=-I $(PYTHON)/include
else
PYTHONINC=-I $(PYTHON)/win32inc
endif
endif


#	Ruby interface:
#	  RUBY=[Path to Ruby directory]
#	  DYNAMIC_RUBY=yes (to load the Ruby DLL dynamically)
#	  RUBY_VER=[Ruby version, eg 16, 17] (default is 16)
#	  RUBY_VER_LONG=[Ruby version, eg 1.6, 1.7] (default is 1.6)
#	    You must set RUBY_VER_LONG when change RUBY_VER.
#RUBY=c:/ruby
DYNAMIC_RUBY=yes
#
# Support Ruby interface
#
ifdef RUBY
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

#RUBY_OBJ = $(OUTDIR)\if_ruby.obj
RUBYINC =-I $(RUBY)/lib/ruby/$(RUBY_VER_LONG)/$(RUBY_PLATFORM)
ifndef DYNAMIC_RUBY
RUBYLIB = -L$(RUBY)/lib -l$(RUBY_INSTALL_NAME)
endif

endif # RUBY

# See feature.h for a list of options.
# Any other defines can be included here.
DEF_GUI=-DFEAT_GUI_W32 -DFEAT_CLIPBOARD -DFEAT_BIG
DEF_MIN=-DFEAT_SMALL
DEFINES=-DWIN32 -DPC
ifeq ($(CROSS),1)
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
DEFINES += -DGETTEXT_DYNAMIC
endif
endif
endif

ifdef PERL
CFLAGS += -I$(PERLLIBS) -DFEAT_PERL -L$(PERLLIBS)
ifdef DYNAMIC_PERL
CFLAGS += -DDYNAMIC_PERL
endif
endif

ifdef RUBY
CFLAGS += -DFEAT_RUBY $(RUBYINC)
ifdef DYNAMIC_RUBY
CFLAGS += -DDYNAMIC_RUBY
DYNAMIC_RUBY_DLL = "$(RUBY_INSTALL_NAME).dll"
endif
endif

ifdef PYTHON
CFLAGS += -DFEAT_PYTHON $(PYTHONINC)
ifdef DYNAMIC_PYTHON
CFLAGS += -DDYNAMIC_PYTHON
endif
endif

ifeq ($(POSTSCRIPT),1)
CFLAGS += -DMSWINPS
endif

ifeq ($(DEBUG),1)
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
ifeq ($(GUI),1)
TARGET := gvim.exe
else
TARGET := vim.exe
endif
ifeq ($(MIN),1)
DEFINES += $(DEF_MIN)
endif

GUISRC =  vimres.c gui.c gui_w32.c
SRC    =  os_w32exe.c buffer.c charset.c diff.c digraph.c edit.c eval.c ex_cmds.c \
	  ex_cmds2.c ex_docmd.c ex_getln.c fileio.c fold.c getchar.c main.c \
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
ifndef DYNAMIC_PERL
LIB += -lperl
endif
endif

ifdef ICONV
LIB += -L$(ICONV)
DEFINES+=-DDYNAMIC_ICONV
CFLAGS += -I$(ICONV)
endif

all: $(TARGET) vimrun.exe xxd/xxd.exe install.exe uninstal.exe

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

clean:
	-$(DEL) *.o
	-$(DEL) *.res
	-$(DEL) *.exe
ifneq (sh.exe, $(SHELL))
	-$(DEL) xxd/*.exe
else
	-$(DEL) xxd\*.exe
endif
	-$(DEL) dyn-ming.h

###########################################################################
vimres.res: vim.rc
	$(WINDRES) --define MING --define FEAT_GUI_W32 vim.rc vimres.res

vimres.o: vimres.res
	$(WINDRES) vimres.res vimres.o

INCL = vim.h feature.h os_win32.h os_dos.h ascii.h keymap.h term.h macros.h \
	structs.h regexp.h option.h ex_cmds.h proto.h globals.h farsi.h \
	gui.h

$(OBJ) $(GUIOBJ): $(INCL)

if_ruby.c: dyn-ming.h

if_python.c: dyn-ming.h

if_perl.c: dyn-ming.h if_perl.xs typemap
	$(PERL) $(PERLLIB)/ExtUtils/xsubpp -prototypes -typemap $(PERLLIB)/ExtUtils/typemap if_perl.xs > $@

os_win32.c: dyn-ming.h

# $(SHELL) is set to sh.exe by default, it is reset to the ABSOLUT path if a
# sh.exe is found; therefore ifeq ("sh.exe", $(SHELL)) means no sh was found
# and the shell used is the dos shell
dyn-ming.h:
ifneq (sh.exe, $(SHELL))
	@echo \/\* created by make \*\/ > dyn-ming.h
else
	@echo /* created by make */ > dyn-ming.h
endif
ifdef DYNAMIC_PERL
ifneq (sh.exe, $(SHELL))
	@echo \#define DYNAMIC_PERL_DLL \"$(DYNAMIC_PERL)\" >> dyn-ming.h
else
	@echo #define DYNAMIC_PERL_DLL "$(DYNAMIC_PERL)" >> dyn-ming.h
endif
endif
ifdef DYNAMIC_RUBY
ifneq (sh.exe, $(SHELL))
	@echo \#define DYNAMIC_RUBY_DLL \"$(DYNAMIC_RUBY_DLL)\" >> dyn-ming.h
else
	@echo #define DYNAMIC_RUBY_DLL "$(DYNAMIC_RUBY_DLL)" >> dyn-ming.h
endif
endif
ifdef DYNAMIC_PYTHON
ifneq (sh.exe, $(SHELL))
	@echo \#define DYNAMIC_PYTHON_DLL \"$(DYNAMIC_PYTHON)\" >> dyn-ming.h
else
	@echo #define DYNAMIC_PYTHON_DLL "$(DYNAMIC_PYTHON)" >> dyn-ming.h
endif
endif
ifdef GETTEXT_DYNAMIC
ifneq (sh.exe, $(SHELL))
	@echo \#define GETTEXT_DLL \"$(GETTEXT_DYNAMIC)\" >> dyn-ming.h
else
	@echo #define GETTEXT_DLL "$(GETTEXT_DYNAMIC)" >> dyn-ming.h
endif
endif
