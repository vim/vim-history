# $Id$
#
# Makefile for SAS/C Amiga Compiler
# Submitted by Stefan Haubenthal <polluks@freeshell.org>

CFLAGS= def AMIGA opt parm r sint
OBJECTS= \
	args.o asm.o awk.o eiffel.o beta.o c.o cobol.o entry.o fortran.o \
	get.o keyword.o lisp.o main.o options.o parse.o perl.o python.o \
	read.o scheme.o sh.o sort.o strlist.o tcl.o vim.o vstring.o

ctags: $(OBJECTS)
	sc link to $@ $(OBJECTS) math s sint

.c.o:
	$(CC) $(CFLAGS) -o $*.o $*.c

clean:
	-delete $(OBJECTS) ctags.lnk

archive: clean
	@-delete force RAM:ctags.lha
	lha -r a RAM:ctags // ctags
