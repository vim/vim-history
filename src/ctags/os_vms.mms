# $Id$
#
# Make file for building CTAGS
#
# Maintained by by Zoltan Arpadffy <arpadffy@altavista.net>
#
# Edit the lines in the Configuration section below to select.
#
# To build: use the following command line:
#
#       mms/descrip=os_vms.mms
#          or if you use mmk
#       mmk/descrip=os_vms.mms
#

######################################################################
# Configuration section.
######################################################################
# Compiler selection.
# Comment out if you use the VAXC compiler
######################################################################
DECC = YES

#####################################################################
# Uncomment if want a debug version. Resulting executable is DVIM.EXE
######################################################################
# DEBUG = YES

######################################################################
# End of configuration section.
#
# Please, do not change anything below without programming experience.
######################################################################

CC      = cc

.IFDEF DECC
CC_DEF  = $(CC)/decc
PREFIX  = /prefix=all
.ELSE
CC_DEF  = $(CC)
PREFIX  =
.ENDIF

LD_DEF  = link

.IFDEF DEBUG
TARGET  = dctags.exe
CFLAGS  = /debug/noopt$(PREFIX)/cross_reference/include=[]
LDFLAGS = /debug
.ELSE
TARGET  = ctags.exe
CFLAGS  = /opt$(PREFIX)/include=[]
LDFLAGS =
.ENDIF

.SUFFIXES : .obj .c

SOURCES = \
	args.c asm.c awk.c eiffel.c beta.c c.c cobol.c entry.c fortran.c \
	get.c keyword.c main.c lisp.c options.c parse.c perl.c python.c \
	read.c sh.c sort.c strlist.c tcl.c vim.c vstring.c

OBJS = \
	args.obj asm.obj awk.obj eiffel.obj beta.obj c.obj cobol.obj \
	entry.obj fortran.obj get.obj keyword.obj lisp.obj main.obj \
	options.obj parse.obj perl.obj python.obj read.obj sh.obj \
	sort.obj strlist.obj tcl.obj vim.obj vstring.obj

all : $(TARGET)
        ! $@

.obj.c :  
        $(CC_DEF) $(CFLAGS) $<

$(TARGET) :  $(OBJS) 
        $(LD_DEF) $(LDFLAGS) /exe=$(TARGET) $+

clean :
       -@ if "''F$SEARCH("*.obj")'" .NES. ""  then del *.obj.*
       -@ if "''F$SEARCH("*.exe")'" .NES. "" then del *.exe.*
       -@ if "''F$SEARCH("config.h")'" .NES. "" then del config.h.*
