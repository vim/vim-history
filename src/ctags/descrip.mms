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
#       mms/descrip=descrip.mms
#          or if you use mmk
#       mmk/descrip=descrip.mms
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
CC_DEF  = $(CC)/decc/prefix=all
EXTRA_OBJS =
.ELSE
CC_DEF  = $(CC)
EXTRA_OBJS = argproc.obj
.ENDIF

LD_DEF  = link

.IFDEF DEBUG
TARGET  = dctags.exe
CFLAGS  = /debug/noopt/cross_reference/include=[]
LDFLAGS = /debug
.ELSE
TARGET  = ctags.exe
CFLAGS  = /opt/include=[]
LDFLAGS =
.ENDIF

.SUFFIXES : .obj .c

.INCLUDE source.mak

all : $(TARGET)
        ! $@

.obj.c :  
        $(CC_DEF) $(CFLAGS) $<

$(TARGET) :  $(OBJS) $(EXTRA_OBJS)
        $(LD_DEF) $(LDFLAGS) /exe=$(TARGET) $+

clean :
       -@ if "''F$SEARCH("*.obj")'" .NES. ""  then del *.obj.*
       -@ if "''F$SEARCH("*.exe")'" .NES. "" then del *.exe.*
       -@ if "''F$SEARCH("config.h")'" .NES. "" then del config.h.*
