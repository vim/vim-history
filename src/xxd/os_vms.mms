# The most simplistic Makefile for VMS
CC	= cc/decc
LD	= link
CFLAGS	= /opt/prefix=all
LDFLAGS	=
LIBS	=

SOURCES	= xxd.c
OBJ = xxd.obj

xxd.exe : $(OBJ)
	$(LD) $(LDFLAGS) /exe=xxd.exe $+

xxd.obj : xxd.c
	$(CC) $(CFLAGS) $<

# Clean
clean :
	del *.obj;*
	del *.exe;*
