# $Id$
#
# Shared macros

HEADERS = \
	args.h ctags.h debug.h entry.h general.h get.h keyword.h \
	main.h options.h parse.h read.h sort.h strlist.h vstring.h

SOURCES = \
	args.c asm.c awk.c eiffel.c beta.c c.c cobol.c entry.c fortran.c \
	get.c keyword.c lisp.c main.c options.c parse.c perl.c python.c \
	read.c scheme.c sh.c sort.c strlist.c tcl.c vim.c vstring.c

OBJECTS = \
	args.o asm.o awk.o eiffel.o beta.o c.o cobol.o entry.o fortran.o \
	get.o keyword.o lisp.o main.o options.o parse.o perl.o python.o \
	read.o scheme.o sh.o sort.o strlist.o tcl.o vim.o vstring.o

OBJS = \
	args.obj asm.obj awk.obj eiffel.obj beta.obj c.obj cobol.obj \
	entry.obj fortran.obj get.obj keyword.obj lisp.obj main.obj \
	options.obj parse.obj perl.obj python.obj read.obj scheme.obj \
	sh.obj sort.obj strlist.obj tcl.obj vim.obj vstring.obj

