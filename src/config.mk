#
# config.mk.in -- autoconf template for Vim on Unix		vim:ts=8:sw=8:
#
# DO NOT EDIT config.mk!! Do not edit config.mk.in!! Edit Makefile and run
# configure again. config.mk will be overwritten by configure. 
#
# Configure does not edit the makefile directly. This method is not the 
# standard use of GNU autoconf, but it has two advantages:
#   a) The user can override every choice made by configure.
#   b) Modifications to the makefile are not lost when configure is run.
#
# I hope this is worth being nonstandard. jw.


CC = gcc
DEFS = -DHAVE_CONFIG_H
CFLAGS = -O2 -fno-strength-reduce -Wall -Wshadow
srcdir = .
LDFLAGS = 
LIBS = -ltermcap 
CTAGS = ctags -i+t

CPP = gcc -E
CPP_MM = M
X_CFLAGS = -I/usr/X11R6/include 
X_LIBS_DIR =  -L/usr/X11R6/lib
X_PRE_LIBS =  -lSM -lICE
X_EXTRA_LIBS = 
X_LIBS = -lX11

### Prefix for location of files
PREFIX = /usr/local
# to fix a bug in autoconf, also do this:
prefix = /usr/local

### Location of binary
EXEC_PREFIX = ${prefix}

### Location for help files
HELPDIR = /share

### Do we have a GUI
GUI_INC_LOC = /usr/local/include
GUI_LIB_LOC = /usr/local/lib
MOTIF_COMMENT = 
ATHENA_COMMENT = ZZZ
NARROW_PROTO = -DFUNCPROTO=15 -DNARROWPROTO
GUI_X_LIBS = -lXext
