#
# Makefile for VIM, using SAS/Lattice C 5.10
#

INCL = vim.h globals.h param.h keymap.h macros.h ascii.h term.h amiga.h
CFLAGS = -c -DAMIGA -DWILD_CARDS -DAUX

OBJ =	alloc.o amiga.o buffers.o charset.o cmdline.o cmdsearch.o edit.o \
	fileio.o help.o linefunc.o main.o mark.o message.o misccmds.o \
	normal.o ops.o param.o quickfix.o regexp.o regsub.o screen.o \
	script.o search.o storage.o tag.o undo.o version.o

Vim: $(OBJ)
	cc $(CFLAGS) version.c
	blink FROM lib:c.o $(OBJ) TO Vim LIBRARY lib:lc.lib lib:amiga.lib lib:arp.lib VERBOSE NODEBUG SMALLCODE SMALLDATA map Vim.map

debug: $(OBJ)
	cc $(CFLAGS) version.c
	blink FROM lib:c.o $(OBJ) TO Vim LIBRARY lib:lc.lib lib:amiga.lib lib:arp.lib VERBOSE DEBUG SMALLCODE SMALLDATA map Vim.map

tags:	$(OBJ)
	csh -c ctags *.c *.h

alloc.o:	alloc.c  $(INCL)
	cc $(CFLAGS) alloc.c

amiga.o:	amiga.c  $(INCL) amiga.h
	cc $(CFLAGS) amiga.c

buffers.o:	buffers.c  $(INCL)
	cc $(CFLAGS) buffers.c

charset.o:	charset.c  $(INCL)
	cc $(CFLAGS) charset.c

cmdline.o:	cmdline.c  $(INCL) cmdtab.h
	cc $(CFLAGS) cmdline.c

cmdsearch.o:	cmdsearch.c  $(INCL)
	cc $(CFLAGS) cmdsearch.c

edit.o:	edit.c  $(INCL)
	cc $(CFLAGS) edit.c

fileio.o:	fileio.c  $(INCL)
	cc $(CFLAGS) fileio.c

help.o:	help.c  $(INCL)
	cc $(CFLAGS) help.c

linefunc.o:	linefunc.c  $(INCL)
	cc $(CFLAGS) linefunc.c

main.o:	main.c  $(INCL)
	cc $(CFLAGS) main.c

mark.o:	mark.c  $(INCL) mark.h
	cc $(CFLAGS) mark.c

message.o:	message.c  $(INCL)
	cc $(CFLAGS) message.c

misccmds.o:	misccmds.c  $(INCL)
	cc $(CFLAGS) misccmds.c

normal.o:	normal.c  $(INCL) ops.h
	cc $(CFLAGS) normal.c

ops.o:	ops.c  $(INCL) ops.h
	cc $(CFLAGS) ops.c

param.o:	param.c  $(INCL)
	cc $(CFLAGS) param.c

quickfix.o:	quickfix.c  $(INCL)
	cc $(CFLAGS) quickfix.c

regexp.o:	regexp.c  $(INCL)
	cc $(CFLAGS) regexp.c

regsub.o:	regsub.c  $(INCL)
	cc $(CFLAGS) regsub.c

screen.o:	screen.c  $(INCL)
	cc $(CFLAGS) screen.c

script.o:	script.c  $(INCL)
	cc $(CFLAGS) script.c

search.o:	search.c  $(INCL)
	cc $(CFLAGS) search.c

storage.o:	storage.c  $(INCL)
	cc $(CFLAGS) storage.c

tag.o:	tag.c  $(INCL) mark.h
	cc $(CFLAGS) tag.c

undo.o:	undo.c  $(INCL)
	cc $(CFLAGS) undo.c

cmdtab.h: cmdtab.tab makecmdtab
	makecmdtab < cmdtab.tab > cmdtab.h

makecmdtab: makecmdtab.o
	cc -o makecmdtab makecmdtab.o
