#
# Makefile for VIM on the Amiga, using Aztec/Manx C 5.0 or later
#
# Note: not all dependencies are included. This was done to avoid having
#       to compile everything when a global variable or function is added
#	

#>>>>> choose options:
### -DDIGRAPHS		digraph support (at the cost of 1.6 Kbyte code)
### -DNO_ARP		do not use arp.library, DOS 2.0 required
### -DCOMPATIBLE	start in vi-compatible mode
### -DNOBACKUP		default is no backup file
### -DDEBUG		output a lot of debugging garbage
### -DTERMCAP		include termcap file support
### -DNO_BUILTIN_TCAPS	do not include builtin termcap entries
###				(use only with -DTERMCAP)
### -DSOME_BUILTIN_TCAPS include most useful builtin termcap entries
###				(use only without -DNO_BUILTIN_TCAPS)
### -DALL_BUILTIN_TCAPS	include all builtin termcap entries
###				(use only without -DNO_BUILTIN_TCAPS)
### -Dconst		for compilers that don't have type const
### -DVIMRC_FILE	name of the .vimrc file in current dir
### -DEXRC_FILE		name of the .exrc file in current dir
### -DSYSVIMRC_FILE	name of the global .vimrc file
### -DSYSEXRC_FILE	name of the global .exrc file
### -DDEFVIMRC_FILE	name of the system-wide .vimrc file
### -DVIM_HLP		name of the help file
### -DNOTITLE		'title' option off by default
### -DWEBB_COMPLETE	include Webb's code for command line completion
### -DWEBB_KEYWORD_COMPL include Webb's code for keyword completion

DEFINES = -DDIGRAPHS -DSOME_BUILTIN_TCAPS -DVIM_ISSPACE \
		-DWEBB_COMPLETE -DWEBB_KEYWORD_COMPL

#>>>>> if TERMCAP is defined obj/termlib.o has to be used
#TERMLIB = obj/termlib.o
TERMLIB = 

#>>>>> choose between debugging (-bs) or optimizing (-so)
OPTIONS = -so
#OPTIONS = -bs

#>>>>> end of choices
###########################################################################

CFLAGS = $(OPTIONS) -wapruq -ps -qf $(DEFINES) -DAMIGA

LIBS = -lc16
SYMS = vim.syms

OBJ =	obj/alloc.o obj/amiga.o obj/buffer.o obj/charset.o obj/cmdcmds.o obj/cmdline.o \
	obj/csearch.o obj/digraph.o obj/edit.o obj/fileio.o obj/getchar.o obj/help.o \
	obj/linefunc.o obj/main.o obj/mark.o obj/memfile.o obj/memline.o obj/message.o obj/misccmds.o \
	obj/normal.o obj/ops.o obj/param.o obj/quickfix.o obj/regexp.o \
	obj/regsub.o obj/screen.o obj/search.o \
	obj/tag.o obj/term.o obj/undo.o obj/window.o $(TERMLIB)

PRO =	proto/alloc.pro proto/buffer.pro proto/charset.pro proto/cmdcmds.pro proto/cmdline.pro \
	proto/csearch.pro proto/digraph.pro proto/edit.pro proto/fileio.pro \
	proto/getchar.pro proto/help.pro proto/linefunc.pro proto/main.pro proto/mark.pro \
	proto/memfile.pro proto/memline.pro proto/message.pro proto/misccmds.pro proto/normal.pro proto/ops.pro \
	proto/param.pro proto/quickfix.pro proto/regexp.pro proto/regsub.pro \
	proto/screen.pro proto/search.pro \
	proto/tag.pro proto/term.pro proto/termlib.pro \
	proto/undo.pro proto/window.pro proto/amiga.pro

/Vim: $(OBJ) version.c
	cc $(CFLAGS) version.c -o obj/version.o
	ln +q -m -o /Vim $(OBJ) obj/version.o $(LIBS)

debug: $(OBJ) version.c
	cc $(CFLAGS) version.c -o obj/version.o
	ln +q -m -g -o /Vim $(OBJ) obj/version.o $(LIBS)

proto: $(SYMS) $(PRO)

ctags:
	csh -c ctags *.c *.h

# can't use delete here, too many file names
clean:
	csh -c rm -f $(OBJ) obj/version.o obj/mkcmdtab.o obj/termlib.o /Vim $(SYMS) mkcmdtab cmdtab.h

$(SYMS)  : vim.h keymap.h macros.h ascii.h term.h structs.h amiga.h
	cc $(CFLAGS) -ho$(SYMS) vim.h

###########################################################################

obj/alloc.o:	alloc.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) alloc.c -o obj/alloc.o

proto/alloc.pro:	alloc.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) alloc.c
	csh -c mv -f alloc.pro proto

obj/amiga.o:	amiga.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) amiga.c -o obj/amiga.o

proto/amiga.pro:	amiga.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) amiga.c
	csh -c mv -f amiga.pro proto

obj/buffer.o:	buffer.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) buffer.c -o obj/buffer.o

proto/buffer.pro:	buffer.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) buffer.c
	csh -c mv -f buffer.pro proto

obj/charset.o:	charset.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) charset.c -o obj/charset.o

proto/charset.pro:	charset.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) charset.c
	csh -c mv -f charset.pro proto

obj/cmdcmds.o:	cmdcmds.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) cmdcmds.c -o obj/cmdcmds.o

proto/cmdcmds.pro:	cmdcmds.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) cmdcmds.c
	csh -c mv -f cmdcmds.pro proto

obj/cmdline.o:	cmdline.c  $(SYMS) cmdtab.h
	cc $(CFLAGS) -hi$(SYMS) cmdline.c -o obj/cmdline.o

proto/cmdline.pro:	cmdline.c $(SYMS) cmdtab.h
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) cmdline.c
	csh -c mv -f cmdline.pro proto

obj/csearch.o:	csearch.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) csearch.c -o obj/csearch.o

proto/csearch.pro:	csearch.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) csearch.c
	csh -c mv -f csearch.pro proto

obj/digraph.o:	digraph.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) digraph.c -o obj/digraph.o

proto/digraph.pro:	digraph.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) digraph.c
	csh -c mv -f digraph.pro proto

obj/edit.o:	edit.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) edit.c -o obj/edit.o

proto/edit.pro:	edit.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) edit.c
	csh -c mv -f edit.pro proto

obj/fileio.o:	fileio.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) fileio.c -o obj/fileio.o

proto/fileio.pro:	fileio.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) fileio.c
	csh -c mv -f fileio.pro proto

obj/getchar.o:	getchar.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) getchar.c -o obj/getchar.o

proto/getchar.pro:	getchar.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) getchar.c
	csh -c mv -f getchar.pro proto

obj/help.o:	help.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) help.c -o obj/help.o

proto/help.pro:	help.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) help.c
	csh -c mv -f help.pro proto

obj/linefunc.o:	linefunc.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) linefunc.c -o obj/linefunc.o

proto/linefunc.pro:	linefunc.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) linefunc.c
	csh -c mv -f linefunc.pro proto

obj/main.o:	main.c $(SYMS) param.h globals.h
	cc $(CFLAGS) -hi$(SYMS) main.c -o obj/main.o

proto/main.pro:	main.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) main.c
	csh -c mv -f main.pro proto

obj/mark.o:	mark.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) mark.c -o obj/mark.o

proto/mark.pro:	mark.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) mark.c
	csh -c mv -f mark.pro proto

obj/memfile.o:	memfile.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) memfile.c -o obj/memfile.o

proto/memfile.pro:	memfile.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) memfile.c
	csh -c mv -f memfile.pro proto

obj/memline.o:	memline.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) memline.c -o obj/memline.o

proto/memline.pro:	memline.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) memline.c
	csh -c mv -f memline.pro proto

obj/message.o:	message.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) message.c -o obj/message.o

proto/message.pro:	message.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) message.c
	csh -c mv -f message.pro proto

obj/misccmds.o:	misccmds.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) misccmds.c -o obj/misccmds.o

proto/misccmds.pro:	misccmds.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) misccmds.c
	csh -c mv -f misccmds.pro proto

obj/normal.o:	normal.c  $(SYMS) ops.h
	cc $(CFLAGS) -hi$(SYMS) normal.c -o obj/normal.o

proto/normal.pro:	normal.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) normal.c
	csh -c mv -f normal.pro proto

obj/ops.o:	ops.c  $(SYMS) ops.h
	cc $(CFLAGS) -hi$(SYMS) ops.c -o obj/ops.o

proto/ops.pro:	ops.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) ops.c
	csh -c mv -f ops.pro proto

obj/param.o:	param.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) param.c -o obj/param.o

proto/param.pro:	param.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) param.c
	csh -c mv -f param.pro proto

obj/quickfix.o:	quickfix.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) quickfix.c -o obj/quickfix.o

proto/quickfix.pro:	quickfix.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) quickfix.c
	csh -c mv -f quickfix.pro proto

obj/regexp.o:	regexp.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) regexp.c -o obj/regexp.o

proto/regexp.pro:	regexp.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) regexp.c
	csh -c mv -f regexp.pro proto

obj/regsub.o:	regsub.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) regsub.c -o obj/regsub.o

proto/regsub.pro:	regsub.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) regsub.c
	csh -c mv -f regsub.pro proto

obj/screen.o:	screen.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) screen.c -o obj/screen.o

proto/screen.pro:	screen.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) screen.c
	csh -c mv -f screen.pro proto

obj/search.o:	search.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) search.c -o obj/search.o

proto/search.pro:	search.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) search.c
	csh -c mv -f search.pro proto

obj/tag.o:	tag.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) tag.c -o obj/tag.o

proto/tag.pro:	tag.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) tag.c
	csh -c mv -f tag.pro proto

obj/term.o:	term.c  $(SYMS) term.h
	cc $(CFLAGS) -hi$(SYMS) term.c -o obj/term.o

proto/term.pro:	term.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) term.c
	csh -c mv -f term.pro proto

obj/termlib.o:	termlib.c  $(SYMS)
	cc $(CFLAGS) termlib.c -o obj/termlib.o

proto/termlib.pro:	termlib.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO termlib.c
#the manx compiler generates a wrong prototype for tputs
	vim -s termlib.fix termlib.pro
	csh -c mv -f termlib.pro proto

obj/undo.o:	undo.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) undo.c -o obj/undo.o

proto/undo.pro:	undo.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) undo.c
	csh -c mv -f undo.pro proto

obj/window.o:	window.c  $(SYMS)
	cc $(CFLAGS) -hi$(SYMS) window.c -o obj/window.o

proto/window.pro:	window.c $(SYMS)
	cc -qa -qp $(CFLAGS) -DPROTO -hi$(SYMS) window.c
	csh -c mv -f window.pro proto

cmdtab.h: cmdtab.tab mkcmdtab
	mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab: obj/mkcmdtab.o
	ln +q -o mkcmdtab obj/mkcmdtab.o -lc16

obj/mkcmdtab.o: mkcmdtab.c
	cc $(CFLAGS) mkcmdtab.c -o obj/mkcmdtab.o
