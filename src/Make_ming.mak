# Makefile for VIM on Win32, using 'EGCS/mingw32 1.1.2'.
# Info at http://www.mingw.org
# Also requires 'GNU make 3.77', which you can get through a link
# to 'JanJaap's page from the above page.
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
# Maintained by Ron Aaron <ron@mossbayeng.com>
# updated 2000 Oct 16

#>>>>> choose options:
# set to '1' for a debug build
DEBUG=0
# set to 1 for size, 0 for speed
OPTSIZE=0
# set to 1 to make gvim, 0 for vim
GUI=1
# set to 1 to make minimal version (few features)
MIN=0
# set to one of i386, i486, i586, i686 as the *target* processor
CPU=i686
# set to same choices as 'CPU', but will prevent running on 'lower' cpus:
ARCH=i386
# set to '1' to cross-compile from unix; 0=native Windows
CROSS=0

# uncomment 'PERL' if you want a perl-enabled version
#PERL=perl
ACTIVEPERL=perl56.dll
# on Linux, for cross-compile, it's here:
#PERLLIB=/home/ron/ActivePerl/lib/
# on NT, it's here:
PERLLIB=c:/perl/lib/
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
DEL = del
WINDRES = windres
endif

#>>>>> end of choices
###########################################################################

CFLAGS = -Iproto $(DEFINES) -pipe -malign-double -mwide-multiply -w
CFLAGS += -march=$(ARCH) -mcpu=$(CPU) -Wall

ifdef PERL
CFLAGS += -I$(PERLLIBS) -DFEAT_PERL -L$(PERLLIBS)
ifdef ACTIVEPERL
CFLAGS += -DACTIVE_PERL 
endif
endif

ifdef PYTHON
CFLAGS += -DFEAT_PYTHON $(PYTHONINC)
ifdef DYNAMIC_PYTHON
CFLAGS += -DDYNAMIC_PYTHON 
endif
endif

ifeq ($(DEBUG),1)
CFLAGS += -g -fstack-check
else
CFLAGS += -s
CFLAGS += -fomit-frame-pointer -freg-struct-return
CFLAGS += -malign-double -mwide-multiply -finline-functions
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
SRC    =  os_w32exe.c buffer.c charset.c digraph.c edit.c eval.c ex_cmds.c \
	  ex_docmd.c ex_getln.c fileio.c fold.c getchar.c main.c mark.c \
	  memfile.c memline.c menu.c message.c misc1.c misc2.c multibyte.c \
	  normal.c ops.c option.c os_win32.c quickfix.c regexp.c screen.c \
	  search.c syntax.c tag.c term.c ui.c undo.c window.c version.c
ifdef PERL
SRC += if_perl.c
endif
ifdef PYTHON
SRC += if_python.c
endif

GUIOBJ = $(GUISRC:.c=.o)
OBJ    = $(SRC:.c=.o)
LIB = -lkernel32 -luser32 -lgdi32 -ladvapi32 -lcomdlg32 -lcomctl32
ifdef PERL
ifndef ACTIVEPERL
LIB += -lperl
endif
endif
all: $(TARGET) vimrun.exe xxd/xxd.exe

vimrun.exe: vimrun.c
	$(CC) $(CFLAGS) -s -o vimrun.exe vimrun.c

vim.exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lkernel32 -luser32 -lgdi32 -ladvapi32

gvim.exe: DEFINES+=$(DEF_GUI)
gvim.exe: $(OBJ) $(GUIOBJ)
	$(CC) $(DEF_GUI) $(CFLAGS) -o $@ $^ -mwindows $(LIB) $(PYTHONLIB)

exes:
	@$(DEL) *.o
	$(MAKE) -f Make_ming.mak gvim.exe
	@$(DEL) *.o
	$(MAKE) -f Make_ming.mak vim.exe
	@$(DEL) *.o

xxd/xxd.exe: xxd/xxd.c
	cd xxd && $(CC) $(CFLAGS) -o xxd.exe -s -DWIN32 xxd.c && cd ..

clean:
	-$(DEL) *.o
	-$(DEL) *.exe
	cd xxd && $(DEL) *.exe && cd ..

###########################################################################
vimres.res: vim.rc
	$(WINDRES) --define MING --define FEAT_GUI_W32 vim.rc vimres.res

vimres.o: vimres.res
	$(WINDRES) vimres.res vimres.o

if_python.c: dyn-ming.h

if_perl.c: dyn-ming.h if_perl.xs typemap
	$(PERL) $(PERLLIB)/ExtUtils/xsubpp -prototypes -typemap $(PERLLIB)/ExtUtils/typemap if_perl.xs > $@

dyn-ming.h:
ifeq ($(CROSS),1)
	@echo \/\* created by make \*\/ > dyn-ming.h
else
	@echo /* created by make */ > dyn-ming.h
endif
ifdef ACTIVEPERL
ifeq ($(CROSS),1)
	@echo \#define ACTIVE_PERL_W32 \"$(ACTIVEPERL)\" >> dyn-ming.h
else
	@echo #define ACTIVE_PERL_W32 "$(ACTIVEPERL)" >> dyn-ming.h
endif
endif

ifdef DYNAMIC_PYTHON
ifeq ($(CROSS),1)
	@echo \#define DYNAMIC_PYTHON_DLL \"$(DYNAMIC_PYTHON)\" >> dyn-ming.h
else
	@echo #define DYNAMIC_PYTHON_DLL "$(DYNAMIC_PYTHON)" >> dyn-ming.h
endif
endif
