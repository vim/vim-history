#
# Makefile for VIM on Win32, using Cygnus gcc
#
# This compiles Vim as a Windows application.  If you want Vim to run as a
# Cygwin application use the Makefile (just like on Unix).
#
# Last updated by Dan Sharp.  Last Change: 2001 Sep 20
#

#>>>>> choose options:

### See feature.h for a list of optionals.
### Any other defines can be included here.

DEFINES = -D__CYGWIN__
INCLUDES = -I/usr/lib/include/mingw

#>>>>> name of the compiler and linker, name of lib directory
CC = gcc
RC = windres
EXE = vim.exe
OUTDIR = obj

#>>>>> uncomment this block to build a GUI version
#OUTDIR = gobj
#DEFINES += -DFEAT_GUI_W32
#GUI_OBJ = $(OUTDIR)/gui.o $(OUTDIR)/gui_w32.o $(OUTDIR)/os_w32exe.o $(OUTDIR)/vimrc.o
#GUI_LIBS = -lcomctl32
#EXE = gvim.exe

#>>>>> end of choices
###########################################################################

INCL = vim.h globals.h option.h keymap.h macros.h ascii.h term.h os_win32.h structs.h
CFLAGS = -O2 -D_MAX_PATH=256 -DWIN32 -DPC -Iproto $(DEFINES) $(INCLUDES)
RCFLAGS = -D_MAX_PATH=256 -DWIN32 -DPC -O coff $(DEFINES)

OBJ = \
	$(OUTDIR)/buffer.o \
	$(OUTDIR)/charset.o \
	$(OUTDIR)/diff.o \
	$(OUTDIR)/digraph.o \
	$(OUTDIR)/edit.o \
	$(OUTDIR)/eval.o \
	$(OUTDIR)/ex_cmds.o \
	$(OUTDIR)/ex_cmds2.o \
	$(OUTDIR)/ex_docmd.o \
	$(OUTDIR)/ex_getln.o \
	$(OUTDIR)/fileio.o \
	$(OUTDIR)/fold.o \
	$(OUTDIR)/getchar.o \
	$(OUTDIR)/main.o \
	$(OUTDIR)/mark.o \
	$(OUTDIR)/memfile.o \
	$(OUTDIR)/memline.o \
	$(OUTDIR)/menu.o \
	$(OUTDIR)/message.o \
	$(OUTDIR)/misc1.o \
	$(OUTDIR)/misc2.o \
	$(OUTDIR)/move.o \
	$(OUTDIR)/mbyte.o \
	$(OUTDIR)/normal.o \
	$(OUTDIR)/ops.o \
	$(OUTDIR)/option.o \
	$(OUTDIR)/os_win32.o \
	$(OUTDIR)/os_mswin.o \
	$(OUTDIR)/quickfix.o \
	$(OUTDIR)/regexp.o \
	$(OUTDIR)/screen.o \
	$(OUTDIR)/search.o \
	$(OUTDIR)/syntax.o \
	$(OUTDIR)/tag.o \
	$(OUTDIR)/term.o \
	$(OUTDIR)/ui.o \
	$(OUTDIR)/undo.o \
	$(OUTDIR)/window.o \
	$(GUI_OBJ) \
	$(TERMLIB)

all: $(EXE) xxd/xxd.exe vimrun.exe install.exe uninstal.exe

$(EXE): $(OUTDIR) $(OBJ) version.c version.h
	$(CC) $(CFLAGS) -s -o $(EXE) version.c $(OBJ) -lkernel32 -luser32 -lgdi32 -ladvapi32 -luuid -lole32 -lcomdlg32 $(GUI_LIBS)

xxd/xxd.exe: xxd/xxd.c
	cd xxd ; $(MAKE) -f Make_cyg.mak ; cd ..

vimrun.exe: vimrun.c
	$(CC) $(CFLAGS) -s -o vimrun.exe vimrun.c  -lkernel32 -luser32 -lgdi32 -ladvapi32

install.exe: dosinst.c
	$(CC) $(CFLAGS) -s -o install.exe dosinst.c  -lkernel32 -luser32 -lgdi32 -ladvapi32 -luuid -lcomctl32 -lole32

uninstal.exe: uninstal.c
	$(CC) $(CFLAGS) -s -o uninstal.exe uninstal.c  -lkernel32 -luser32 -lgdi32 -ladvapi32

$(OUTDIR):
	mkdir $(OUTDIR)

tags:
	command /c ctags *.c $(INCL)

clean:
	-del $(OUTDIR)\*.o
	-del *.o
	-del *.exe
	-del *.~
	-del *~

###########################################################################

$(OUTDIR)/buffer.o:	buffer.c $(INCL)
	$(CC) -c $(CFLAGS) buffer.c -o $(OUTDIR)/buffer.o

$(OUTDIR)/charset.o:	charset.c $(INCL)
	$(CC) -c $(CFLAGS) charset.c -o $(OUTDIR)/charset.o

$(OUTDIR)/diff.o:	diff.c $(INCL)
	$(CC) -c $(CFLAGS) diff.c -o $(OUTDIR)/diff.o

$(OUTDIR)/digraph.o:	digraph.c $(INCL)
	$(CC) -c $(CFLAGS) digraph.c -o $(OUTDIR)/digraph.o

$(OUTDIR)/edit.o:	edit.c $(INCL)
	$(CC) -c $(CFLAGS) edit.c -o $(OUTDIR)/edit.o

$(OUTDIR)/eval.o:	eval.c $(INCL)
	$(CC) -c $(CFLAGS) eval.c -o $(OUTDIR)/eval.o

$(OUTDIR)/ex_cmds.o:	ex_cmds.c $(INCL)
	$(CC) -c $(CFLAGS) ex_cmds.c -o $(OUTDIR)/ex_cmds.o

$(OUTDIR)/ex_cmds2.o:	ex_cmds2.c $(INCL)
	$(CC) -c $(CFLAGS) ex_cmds2.c -o $(OUTDIR)/ex_cmds2.o

$(OUTDIR)/ex_docmd.o:	ex_docmd.c $(INCL) ex_cmds.h
	$(CC) -c $(CFLAGS) ex_docmd.c -o $(OUTDIR)/ex_docmd.o

$(OUTDIR)/ex_getln.o:	ex_getln.c $(INCL)
	$(CC) -c $(CFLAGS) ex_getln.c -o $(OUTDIR)/ex_getln.o

$(OUTDIR)/fileio.o:	fileio.c $(INCL)
	$(CC) -c $(CFLAGS) fileio.c -o $(OUTDIR)/fileio.o

$(OUTDIR)/fold.o:	fold.c $(INCL)
	$(CC) -c $(CFLAGS) fold.c -o $(OUTDIR)/fold.o

$(OUTDIR)/getchar.o:	getchar.c $(INCL)
	$(CC) -c $(CFLAGS) getchar.c -o $(OUTDIR)/getchar.o

$(OUTDIR)/gui.o:	gui.c $(INCL)
	$(CC) -c $(CFLAGS) gui.c -o $(OUTDIR)/gui.o

$(OUTDIR)/gui_w32.o:	gui_w32.c $(INCL)
	$(CC) -c $(CFLAGS) gui_w32.c -o $(OUTDIR)/gui_w32.o

$(OUTDIR)/main.o:	main.c $(INCL)
	$(CC) -c $(CFLAGS) main.c -o $(OUTDIR)/main.o

$(OUTDIR)/mark.o:	mark.c $(INCL)
	$(CC) -c $(CFLAGS) mark.c -o $(OUTDIR)/mark.o

$(OUTDIR)/memfile.o:	memfile.c $(INCL)
	$(CC) -c $(CFLAGS) memfile.c -o $(OUTDIR)/memfile.o

$(OUTDIR)/memline.o:	memline.c $(INCL)
	$(CC) -c $(CFLAGS) memline.c -o $(OUTDIR)/memline.o

$(OUTDIR)/menu.o:	menu.c $(INCL)
	$(CC) -c $(CFLAGS) menu.c -o $(OUTDIR)/menu.o

$(OUTDIR)/message.o:	message.c $(INCL)
	$(CC) -c $(CFLAGS) message.c -o $(OUTDIR)/message.o

$(OUTDIR)/misc1.o:	misc1.c $(INCL)
	$(CC) -c $(CFLAGS) misc1.c -o $(OUTDIR)/misc1.o

$(OUTDIR)/misc2.o:	misc2.c $(INCL)
	$(CC) -c $(CFLAGS) misc2.c -o $(OUTDIR)/misc2.o

$(OUTDIR)/move.o:	move.c $(INCL)
	$(CC) -c $(CFLAGS) move.c -o $(OUTDIR)/move.o

$(OUTDIR)/mbyte.o: mbyte.c $(INCL)
	$(CC) -c $(CFLAGS) mbyte.c -o $(OUTDIR)/mbyte.o

$(OUTDIR)/normal.o:	normal.c $(INCL)
	$(CC) -c $(CFLAGS) normal.c -o $(OUTDIR)/normal.o

$(OUTDIR)/ops.o:	ops.c $(INCL)
	$(CC) -c $(CFLAGS) ops.c -o $(OUTDIR)/ops.o

$(OUTDIR)/option.o:	option.c $(INCL)
	$(CC) -c $(CFLAGS) option.c -o $(OUTDIR)/option.o

$(OUTDIR)/os_win32.o:	os_win32.c $(INCL)
	$(CC) -c $(CFLAGS) os_win32.c -o $(OUTDIR)/os_win32.o

$(OUTDIR)/os_w32exe.o: os_w32exe.c $(INCL)
	$(CC) -c $(CFLAGS) os_w32exe.c -o $(OUTDIR)/os_w32exe.o

$(OUTDIR)/os_mswin.o:	os_mswin.c $(INCL)
	$(CC) -c $(CFLAGS) os_mswin.c -o $(OUTDIR)/os_mswin.o

$(OUTDIR)/quickfix.o:	quickfix.c $(INCL)
	$(CC) -c $(CFLAGS) quickfix.c -o $(OUTDIR)/quickfix.o

$(OUTDIR)/regexp.o:	regexp.c $(INCL)
	$(CC) -c $(CFLAGS) regexp.c -o $(OUTDIR)/regexp.o

$(OUTDIR)/screen.o:	screen.c $(INCL)
	$(CC) -c $(CFLAGS) screen.c -o $(OUTDIR)/screen.o

$(OUTDIR)/search.o:	search.c $(INCL)
	$(CC) -c $(CFLAGS) search.c -o $(OUTDIR)/search.o

$(OUTDIR)/syntax.o:	syntax.c $(INCL)
	$(CC) -c $(CFLAGS) syntax.c -o $(OUTDIR)/syntax.o

$(OUTDIR)/tag.o:	tag.c $(INCL)
	$(CC) -c $(CFLAGS) tag.c -o $(OUTDIR)/tag.o

$(OUTDIR)/term.o:	term.c $(INCL)
	$(CC) -c $(CFLAGS) term.c -o $(OUTDIR)/term.o

$(OUTDIR)/ui.o:	ui.c $(INCL)
	$(CC) -c $(CFLAGS) ui.c -o $(OUTDIR)/ui.o

$(OUTDIR)/undo.o:	undo.c $(INCL)
	$(CC) -c $(CFLAGS) undo.c -o $(OUTDIR)/undo.o

$(OUTDIR)/vimrc.o:	vim.rc $(INCL)
	$(RC) $(RCFLAGS) vim.rc -o $(OUTDIR)/vimrc.o

$(OUTDIR)/window.o:	window.c $(INCL)
	$(CC) -c $(CFLAGS) window.c -o $(OUTDIR)/window.o
