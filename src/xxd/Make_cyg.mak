# The most simplistic Makefile, for Cygnus gcc on MS-DOS

ifndef USEDLL
USEDLL = no
endif

ifeq (no, $(USEDLL))
DEFINES = -mno-cygwin
LIBS    =
else
DEFINES =
LIBS    = -lc
endif

CFLAGS = -O2 -Wall $(DEFINES)

ifneq (sh.exe, $(SHELL))
DEL = rm
else
DEL = del
endif

xxd.exe: xxd.c
	gcc $(CFLAGS) -s -o xxd.exe xxd.c $(LIBS)
	
clean:
	-$(DEL) xxd.exe
