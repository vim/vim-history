#
# Makefile for VIM, using Manx C 5.0d
#

INCL = vim.syms
#use the next line for debugging
#CFLAGS = -bs -wa -wc -ps -qf -DAMIGA -DWILD_CARDS -DAUX
CFLAGS = -so -wa -wc -ps -qf -DAMIGA -DWILD_CARDS -DAUX
LIBS = -lc16

OBJ =	alloc.o amiga.o buffers.o charset.o cmdline.o cmdsearch.o edit.o \
	fileio.o help.o linefunc.o main.o mark.o message.o misccmds.o \
	normal.o ops.o param.o quickfix.o regexp.o regsub.o screen.o \
	script.o search.o storage.o tag.o undo.o version.o

/Vim: $(OBJ)
	cc $(CFLAGS) version.c
	ln +q -m -o /Vim $(OBJ) $(LIBS)

debug: $(OBJ)
	cc $(CFLAGS) version.c
	ln +q -m -g -o /Vim $(OBJ) $(LIBS)

tags:	$(OBJ)
	csh -c ctags *.c *.h

$(INCL)  : vim.h globals.h param.h keymap.h macros.h ascii.h term.h amiga.h
	cc $(CFLAGS) -ho$(INCL) vim.h

alloc.o:	alloc.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) alloc.c

amiga.o:	amiga.c  $(INCL) amiga.h
	cc $(CFLAGS) -hi$(INCL) amiga.c

buffers.o:	buffers.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) buffers.c

charset.o:	charset.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) charset.c

cmdline.o:	cmdline.c  $(INCL) cmdtab.h
	cc $(CFLAGS) -hi$(INCL) cmdline.c

cmdsearch.o:	cmdsearch.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) cmdsearch.c

edit.o:	edit.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) edit.c

fileio.o:	fileio.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) fileio.c

help.o:	help.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) help.c

linefunc.o:	linefunc.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) linefunc.c

main.o:	main.c  $(INCL)
	cc $(CFLAGS) main.c

mark.o:	mark.c  $(INCL) mark.h
	cc $(CFLAGS) -hi$(INCL) mark.c

message.o:	message.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) message.c

misccmds.o:	misccmds.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) misccmds.c

normal.o:	normal.c  $(INCL) ops.h
	cc $(CFLAGS) -hi$(INCL) normal.c

ops.o:	ops.c  $(INCL) ops.h
	cc $(CFLAGS) -hi$(INCL) ops.c

param.o:	param.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) param.c

quickfix.o:	quickfix.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) quickfix.c

regexp.o:	regexp.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) regexp.c

regsub.o:	regsub.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) regsub.c

screen.o:	screen.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) screen.c

script.o:	script.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) script.c

search.o:	search.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) search.c

storage.o:	storage.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) storage.c

tag.o:	tag.c  $(INCL) mark.h
	cc $(CFLAGS) -hi$(INCL) tag.c

undo.o:	undo.c  $(INCL)
	cc $(CFLAGS) -hi$(INCL) undo.c

cmdtab.h: cmdtab.tab makecmdtab
	makecmdtab <cmdtab.tab >cmdtab.h

makecmdtab: makecmdtab.o
	ln +q -o makecmdtab makecmdtab.o -lc16
