#
# Makefile for Vim on VMS
#
# This has only been tested on VMS 7.1 on DEC Alpha hardware.
# The following will be built:
#	vim.exe:	standard
#		or
#	dvim.exe:	debug
#
# Edit the lines in the Configuration section below to select.
#
# To build: use the following command line:
#
#   mmk/descrip=os_vms.mms
#		or
#	mms/descrip=os_vms.mms
#

#######################################################################
# Configuration section.
#######################################################################

# Comment out if you want just the character terminal mode only.
GUI = YES

# Uncomment if want a debug version.
# DEBUG = YES


.IFDEF GUI
#######################################################################
# X/Motif executable  (also works in terminal mode )
#######################################################################
DEFS	= /def=("HAVE_CONFIG_H","USE_GUI_MOTIF")
LIBS	= ,OS_VMS.OPT/OPT
CONFIG_H = gui_vms_conf.h
GUI_INC_LOC = decw$include:
GUI_LIB_LOC = sys$library:
GUI_SRC = gui.c gui_motif.c gui_x11.c
GUI_OBJ = gui.obj gui_motif.obj gui_x11.obj

# These may need to be defined if things are not in standard locations:
X_LIBS_DIR =
X_PRE_LIBS =
X_EXTRA_LIBS =
X_LIBS =
#######################################################################
.ELSE
#######################################################################
# Character terminal only executable
#######################################################################
DEFS	= /def=("HAVE_CONFIG_H")
LIBS	=
CONFIG_H = os_vms_conf.h
#######################################################################
# End of configuration section
#######################################################################
.ENDIF
#######################################################################

CC	= cc/decc
LD	= link

.IFDEF DEBUG
TARGET = dvim.exe
CFLAGS	= /debug/noopt/prefix=all/include=[.proto]
LDFLAGS	=  /debug
.ELSE
TARGET = vim.exe
CFLAGS	= /opt/prefix=all/include=[.proto]
LDFLAGS	=
.ENDIF


### These go into pathdef.c
VIMLOC = $VIM
VIM_HLP = $VIM_DOC:help.txt


###
### GRAPHICAL USER INTERFACE (GUI).


.SUFFIXES :
.SUFFIXES : .c .obj .pro

ALL_CFLAGS = $(DEFS) $(GUI_DEFS) $(GUI_IPATH) $(CFLAGS)

ALL_LIBS = $(GUI_LIBS_DIR) $(X_LIBS_DIR) $(X_PRE_LIBS) $(GUI_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(LIBS) $(EXTRA_LIBS)

INCL = vim.h globals.h option.h keymap.h macros.h ascii.h term.h os_unix.h \
	unixunix.h structs.h proto.h osdef.h config.h \
	$(GUI_INCL)

SRC =	buffer.c charset.c digraph.c edit.c eval.c ex_cmds.c ex_docmd.c \
	ex_getln.c fileio.c getchar.c \
	main.c mark.c menu.c\
	memfile.c memline.c message.c misc1.c misc2.c normal.c ops.c option.c \
	pty.c quickfix.c regexp.c search.c syntax.c tag.c term.c termlib.c \
	ui.c undo.c version.c screen.c window.c os_vms.c pathdef.c \
	$(GUI_SRC)

OBJ =	buffer.obj charset.obj digraph.obj edit.obj eval.obj ex_cmds.obj \
	ex_docmd.obj ex_getln.obj fileio.obj getchar.obj \
	main.obj mark.obj menu.obj memfile.obj memline.obj message.obj misc1.obj \
	misc2.obj normal.obj ops.obj option.obj pty.obj quickfix.obj \
	regexp.obj search.obj syntax.obj tag.obj term.obj termlib.obj ui.obj \
	undo.obj screen.obj window.obj os_vms.obj pathdef.obj \
	$(GUI_OBJ)

# Default target is making the executable
all : $(TARGET) mms_vim
	! $@

config.h :	$(CONFIG_H)
	copy $(CONFIG_H) config.h

osdef.h :	os_vms_osdef.h
	copy os_vms_osdef.h osdef.h

# Re-execute this os_vms.mms to include the new config.mk produced by
# configure Only used when typing "make" with a fresh config.mk.
myself :
	mms /descrip=os_vms.mms all

clean :
	del *.obj;*
	del config.h;*

# Link the target
$(TARGET) : $(OBJ) version.obj
	$(CC) $(ALL_CFLAGS) version.c
	$(LD) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)

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
	-@ write pd " * Change the file os_vms.mms only. */"
	-@ write pd "#include ""vim.h"""
	-@ write pd "char_u *default_vim_dir = (char_u *)""$(VIMLOC)"";"
	-@ write pd "char_u *default_vimruntime_dir = (char_u *)"""";"
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
menu.obj : menu.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
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
