# The most simplistic Makefile for VMS
CC	= cc/decc
LD	= link
CFLAGS	= /opt/prefix=all
LDFLAGS	= 
LIBS	=

SOURCES	= entry.c get.c main.c options.c parse.c read.c sort.c
OBJ = entry.obj get.obj main.obj options.obj parse.obj read.obj sort.obj

ctags.exe : $(OBJ)
	$(LD) $(LDFLAGS) /exe=ctags.exe $+ 

entry.obj : entry.c
	$(CC) $(CFLAGS) $<
get.obj : get.c
	$(CC) $(CFLAGS) $<
main.obj : main.c
	$(CC) $(CFLAGS) $<
options.obj : options.c
	$(CC) $(CFLAGS) $<
parse.obj : parse.c
	$(CC) $(CFLAGS) $<
read.obj : read.c
	$(CC) $(CFLAGS) $<
sort.obj : sort.c
	$(CC) $(CFLAGS) $<
