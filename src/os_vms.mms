CC	= cc/decc
LD	= link
DEFS	= /def=("HAVE_CONFIG_H")
CFLAGS	= /debug/noopt/prefix=all
LDFLAGS	= 
LIBS	=

X_LIBS_DIR =
X_PRE_LIBS =
X_EXTRA_LIBS = 
X_LIBS =

### Location of binary
EXEC_PREFIX = []

### Location for help files
HELPDIR = vim_hlp:

### Do we have a GUI
#GUI_INC_LOC = xxx:[xxx.xxx]
#GUI_LIB_LOC = xxx:[xxx.xxx]

TARGET = vim.exe

### Installation directory. Default from configure is "/usr/local".
### EXEC_PREFIX is for the executable, PREFIX is for other files (they may be
### different when using different architectures for the executable and a
### common directory for the other files).
###
### Uncomment the next two lines to install in your home directory.

### Location of executable
BINLOC = $(EXEC_PREFIX)/bin

### Location of man page
MANLOC = vim_man:
MANDIR = vim_man:

### Location of help files and system global vimrc
### The default for HELPDIR comes from configure: "/share" or "/lib"
HELPSUBDIR = vim_hlp:
HELPLOC = vim_hlp:
HELPSUBLOC = vim_hlp:

### These go into pathdef.c
VIMLOC = vim:
VIM_HLP = $(HELPSUBLOC)vim_help.txt
SYS_VIMRC_FILE  = $(HELPSUBLOC).vimrc
SYS_GVIMRC_FILE = $(HELPSUBLOC).gvimrc

### Program to run on installed binary
STRIP = strip

### Permissions for vim binary
BINMOD = 755

### Permissions for man page
MANMOD = 644

### Permissions for help files
HELPMOD = 644

# Where to copy the man and help files from
HELPSOURCE = [-.doc]

###
### GRAPHICAL USER INTERFACE (GUI).
### 'configure --enable-gui' can enable one of these for you if you did set
### a corresponding CONF_OPT_GUI above and have X11.
### Override configures choice by uncommenting one of the following lines (The
### first is an explicit enable, the second is an explicit disable):

.SUFFIXES :
.SUFFIXES : .c .obj .pro

ALL_CFLAGS = $(DEFS) $(GUI_DEFS) $(GUI_IPATH) $(CFLAGS)

ALL_LIBS = $(GUI_LIBS_DIR) $(X_LIBS_DIR) $(X_PRE_LIBS) $(GUI_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(LIBS) $(EXTRA_LIBS)

INCL = vim.h globals.h option.h keymap.h macros.h ascii.h term.h os_unix.h \
	unixunix.h structs.h proto.h osdef.h config.h \
	$(GUI_INCL)

SRC =	buffer.c charset.c digraph.c edit.c eval.c ex_cmds.c ex_docmd.c \
	ex_getln.c fileio.c getchar.c \
	main.c mark.c \
	memfile.c memline.c message.c misc1.c misc2.c normal.c ops.c option.c \
	pty.c quickfix.c regexp.c search.c syntax.c tag.c term.c termlib.c \
	ui.c undo.c version.c screen.c window.c os_vms.c pathdef.c

OBJ =	buffer.obj charset.obj digraph.obj edit.obj eval.obj ex_cmds.obj \
	ex_docmd.obj ex_getln.obj fileio.obj getchar.obj \
	main.obj mark.obj memfile.obj memline.obj message.obj misc1.obj \
	misc2.obj normal.obj ops.obj option.obj pty.obj quickfix.obj \
	regexp.obj search.obj syntax.obj tag.obj term.obj termlib.obj ui.obj \
	undo.obj screen.obj window.obj os_vms.obj pathdef.obj

# Default target is making the executable
all : $(TARGET) mms_vim
	! $@
# Run ./configure with all the setting above.
config : configure
	GUI_INC_LOC="$(GUI_INC_LOC)" GUI_LIB_LOC="$(GUI_LIB_LOC)" \
		CC="$(CC)" CFLAGS="$(CFLAGS)" TERM_LIB="$(TERM_LIB)" \
		NO_XMKMF="$(NO_XMKMF)" \
	./configure $(CONF_OPT_GUI) $(CONF_OPT_X)

config.h :	os_vms_conf.h
	copy os_vms_conf.h config.h

osdef.h :	os_vms_osdef.h
	copy os_vms_osdef.h osdef.h

# When configure.in has changed, run autoconf to produce configure
# If you don't have autoconf, use the configure that's there
configure : configure.in
	autoconf
# Fix a bug in autoconf 2.8 and 2.9:
	mv -f configure configure.wrong
	sed -e 's+$$ac_cv_lib_dnet+$$ac_cv_lib_$$ac_lib_var+' configure.wrong > configure
	rm -f configure.wrong
	chmod 755 configure

# Re-execute this os_vms.mms to include the new config.mk produced by configure
# Only used when typing "make" with a fresh config.mk.
myself :
	mms /descrip=os_vms.mms all


# Link the target for normal use
$(TARGET) : $(OBJ) version.obj
	$(LD) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)
	$(LD) $(LDFLAGS)/debug /exe=dvim.exe $+ $(ALL_LIBS)

FILES = *.c *.h os_vms.mms *.in makefile.* *.sh cmdtab.tab tags configure

###########################################################################

# Used when .obj files are in src directory
.c.obj :
	$(CC) $(ALL_CFLAGS) $<

pathdef.c : os_vms.mms
	-@ write sys$output "creating pathdef.c"
	-@ open/write pd pathdef.c
	-@ write pd "/* pathdef.c -- DO NOT EDIT! */"
	-@ write pd "/* This file is automatically created by os_vms.mms"
	-@ write pd " * Change the os_vms.mms only. */"
	-@ write pd "#include ""vim.h"""
	-@ write pd "char_u *default_vim_dir = (char_u *)""$(VIMLOC)"";"

	-@ write pd "#ifndef SYS_VIMRC_FILE"
	-@ write pd "char_u *sys_vimrc_fname = (char_u *)""$(SYS_VIMRC_FILE)"";"
	-@ write pd "#else"
	-@ write pd "char_u *sys_vimrc_fname = (char_u *)SYS_VIMRC_FILE;"
	-@ write pd "#endif"
	-@ write pd "#ifndef SYS_GVIMRC_FILE"
	-@ write pd "char_u *sys_gvimrc_fname = (char_u *)""$(SYS_GVIMRC_FILE)"";"
	-@ write pd "#else"
	-@ write pd "char_u *sys_gvimrc_fname = (char_u *)SYS_GVIMRC_FILE;"
	-@ write pd "#endif"
	-@ write pd "#ifndef VIM_HLP"
	-@ write pd "char_u *help_fname = (char_u *)""$(VIM_HLP)"";"
	-@ write pd "#else"
	-@ write pd "char_u *help_fname = (char_u *)VIM_HLP;"
	-@ write pd "#endif"
	-@ write pd "char_u *all_cflags = (char_u *)""$(CC) /incl=[] $(ALL_CFLAGS)"";"
	-@ write pd "char_u *all_lflags = (char_u *)""$(LD) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)"";"
	-@ close pd

os_vms.mms :
	@ write sys$output "The name of the makefile MUST be <os_vms.mms> !!!!"

cmdtab.h : cmdtab.tab mkcmdtab.exe
	mkcmdtab :== $sys$disk:[]mkcmdtab
	mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab.exe : mkcmdtab.obj
	$(LD) $(LDFLAGS) mkcmdtab
mkcmdtab.obj : mkcmdtab.c
	$(CC) $(ALL_CFLAGS) mkcmdtab.c

###############################################################################
ui.obj :	ui.c vim.h
	$(CC) $(ALL_CFLAGS) $<
syntax.obj :	syntax.c vim.h
	$(CC) $(ALL_CFLAGS) $<
misc1.obj :	misc1.c vim.h
	$(CC) $(ALL_CFLAGS) $<
misc2.obj :	misc2.c vim.h
	$(CC) $(ALL_CFLAGS) $<
ex_getln.obj :	ex_getln.c vim.h
	$(CC) $(ALL_CFLAGS) $<
ex_docmd.obj :	ex_docmd.c vim.h ex_cmds.h
	$(CC) $(ALL_CFLAGS) $<
ex_cmds.obj :	ex_cmds.c vim.h
	$(CC) $(ALL_CFLAGS) $<
alloc.obj :	alloc.c vim.h config.h feature.h os_unix.h osdef.h \
		ascii.h keymap.h term.h macros.h structs.h gui.h \
		globals.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
buffer.obj :	buffer.c vim.h config.h feature.h os_unix.h osdef.h \
		ascii.h keymap.h term.h macros.h structs.h gui.h \
		globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
charset.obj :	charset.c vim.h config.h feature.h os_unix.h osdef.h \
		ascii.h keymap.h term.h macros.h structs.h gui.h \
		globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
cmdcmds.obj : cmdcmds.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
cmdline.obj : cmdline.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h cmdtab.h

	$(CC) $(ALL_CFLAGS) $<
csearch.obj : csearch.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
digraph.obj : digraph.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
edit.obj :	edit.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
		keymap.h term.h macros.h structs.h gui.h globals.h proto.h \
		regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
fileio.obj : fileio.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
getchar.obj : getchar.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
help.obj : help.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
linefunc.obj : linefunc.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
main.obj : main.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
mark.obj : mark.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
memfile.obj : memfile.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
memline.obj : memline.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
message.obj : message.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
misccmds.obj : misccmds.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
normal.obj : normal.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
ops.obj : ops.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
option.obj : option.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
pathdef.obj : pathdef.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h
	$(CC) $(ALL_CFLAGS) $<
eval.obj : eval.c vim.h
	$(CC) $(ALL_CFLAGS) $<
quickfix.obj : quickfix.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
regexp.obj : regexp.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	     term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
regsub.obj : regsub.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	     term.h macros.h structs.h gui.h globals.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
screen.obj : screen.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
search.obj : search.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
tables.obj : tables.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
tag.obj : tag.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
term.obj : term.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	   term.h macros.h structs.h gui.h globals.h option.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
termlib.obj : termlib.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
	      keymap.h term.h macros.h structs.h gui.h globals.h option.h \
	      proto.h regexp.h [.proto]termlib.pro
	$(CC) $(ALL_CFLAGS) $<
undo.obj :	undo.c vim.h config.h feature.h os_unix.h osdef.h \
		ascii.h keymap.h term.h macros.h structs.h gui.h \
		globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
os_vms.obj :	os_vms.c vim.h globals.h option.h proto.h
	$(CC) $(ALL_CFLAGS) $<
window.obj :	window.c vim.h config.h feature.h os_unix.h osdef.h \
		ascii.h keymap.h term.h macros.h structs.h gui.h \
		globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
version.obj :	version.c version.h vim.h config.h feature.h os_unix.h \
		osdef.h ascii.h keymap.h term.h macros.h structs.h gui.h
	$(CC) $(ALL_CFLAGS) $<
gui.obj : gui.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
gui_motif.obj : gui_motif.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
gui_x11.obj : gui_x11.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
pty.obj :	pty.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
			keymap.h term.h macros.h structs.h gui.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
mms_vim :	mms_vim.exe
	!
mms_vim.exe :	mms_vim.obj
	$(LD) $(LDFLAGS) mms_vim.obj
mms_vim.obj :	os_vms_mms.c
	$(CC) $(CFLAGS) os_vms_mms.c
	copy os_vms_mms.obj mms_vim.obj
