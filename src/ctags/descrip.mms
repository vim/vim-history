# VMS MM[KS] makefile for ctags
# tested with MMK 3.4, DEC C 5.7 under VMS 6.2
#
# Maintained by Martin Vorlaender <martin@radiogaga.harz.de>

.ifdef DEBUG
CDEBFLAGS = /debug/nooptimize/list
DEBDEF = ,DEBUG
LDEBFLAGS = /debug/map=$(MMS$TARGET_NAME)/cross_reference
.endif

CC	= cc/decc
LD	= link
CFLAGS	= $(CDEBFLAGS)/prefix=all/define=(HAVE_CONFIG_H$(DEBDEF))/include=[]
LDFLAGS	= $(LDEBFLAGS)/exec=$(MMS$TARGET)
LIBS	=

SOURCES	= args.c eiffel.c entry.c fortran.c get.c keyword.c \
	main.c options.c parse.c read.c sort.c strlist.c vstring.c
OBJ	= args.obj eiffel.obj entry.obj fortran.obj get.obj keyword.obj \
	main.obj options.obj parse.obj read.obj sort.obj strlist.obj vstring.obj

.first
	! For use with DEC C under VMS versions prior 7.0
	DEFINE DECC$CRTLMAP SYS$LIBRARY:DECC$CRTL.EXE
	DEFINE LNK$LIBRARY  SYS$LIBRARY:DECC$CRTL.OLB

.c.obj :
	$(CC) $(CFLAGS) $(MMS$SOURCE)

.ifdef DEBUG
dctags.exe : $(OBJ) debug.obj
.else
ctags.exe : $(OBJ)
.endif
	$(LD) $(LDFLAGS) $(MMS$SOURCE_LIST)
