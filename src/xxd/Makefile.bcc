# Simple makefile for Borland C++ 4.0
# 3.1 can NOT be used, it has problems with the fileno() define.

# Adjust the paths to your location of the borland C files
BCCLOC  = e:\bc4
CC	=   $(BCCLOC)\bin\bcc
INC	= -I$(BCCLOC)\include
LIB	= -L$(BCCLOC)\lib

# uncomment the next line for including debugging messages
#DEBUG	= -DDEBUG

# The following compile options can be changed for better machines.
#	replace -1- with -2 to produce code for a 80286 or higher
#	replace -1- with -3 to produce code for a 80386 or higher
#	add -v for source debugging
OPTIMIZE= -1- -Ox

CFLAGS	= -A -mc -DMSDOS $(DEBUG) $(OPTIMIZE) $(INC) $(LIB)

xxd.exe: xxd.c
	$(CC) $(CFLAGS) xxd.c
