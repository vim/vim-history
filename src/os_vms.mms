#
# Makefile for Vim on OpenVMS
#
# Maintainer:   Zoltan Arpadffy <arpadffy@altavista.net>
# Last change:  2000 Feb 28
# 
# This has been tested on VMS 6.2 to 7.1 on DEC Alpha and VAX.
# The following will be built:
#	vim.exe:	standard
#		  or
#	dvim.exe:	debug
#
# Edit the lines in the Configuration section below to select.
#
# To build: use the following command line:
#
#	mms/descrip=os_vms.mms
#	   or if you use mmk
#	mmk/descrip=os_vms.mms
#

######################################################################
# Configuration section.
######################################################################

# Comment out if you want just the character terminal mode only.
GUI = YES

######################################################################
# Comment out if you want the compiler version with :ver command.
# NOTE: This part can make some complications if you're using some 
# predefined symbols/flags for your compiler. If does, just leave behind
# the comment varialbe CCVER.
######################################################################
CCVER = YES

######################################################################
# Used to fix some dependecies problem during development.
# Normally, it should not be defined. !!!
######################################################################
DEVELOPMENT = YES

######################################################################
# Uncomment if want a debug version. Resulting executable is DVIM.EXE
######################################################################
# DEBUG = YES

######################################################################
# Languages support as Perl, Python, TCL etc.
# If you don't need it really, leave them behind the comment.
# You will need special libs etc.
######################################################################
# VIM_TCL    = YES
# VIM_PERL   = YES
# VIM_SNIFF  = YES
# VIM_PYTHON = YES

.IFDEF GUI
######################################################################
# X/Motif executable  (also works in terminal mode )
######################################################################
DEFS     = "HAVE_CONFIG_H","USE_GUI_MOTIF"
LIBS	 = ,OS_VMS.OPT/OPT
CONFIG_H = gui_vms_conf.h
GUI_SRC  = gui.c gui_motif.c gui_x11.c
GUI_OBJ  = gui.obj gui_motif.obj gui_x11.obj
GUI_INC  = gui.h

# These may need to be defined if things are not in standard locations:

# GUI_INC_DIR = decw$include:
# GUI_LIB_DIR = sys$library:
# X_LIB_DIR   =
# X_LIB       =
# EXTRA_LIB   =

.ELSE
######################################################################
# Character terminal only executable
######################################################################
DEFS     = "HAVE_CONFIG_H"
LIBS	 =
CONFIG_H = os_vms_conf.h
.ENDIF

.IFDEF VIM_PERL
######################################################################
# Perl related setup.
######################################################################
PERL_DEF = ,"HAVE_PERL_INTERP"
PERL_SRC = if_perlsfio.c if_perl.xs
PERL_OBJ = if_perlsfio.obj if_perl.obj
PERL_LIB = 
.ENDIF

.IFDEF VIM_PYTHON
######################################################################
# Python related setup. 
######################################################################
PYTHON_DEF = ,"HAVE_PYTHON"
PYTHON_SRC = if_python.c
PYTHON_OBJ = if_python.obj
PYTHON_LIB =
.ENDIF

.IFDEF VIM_TCL
######################################################################
# TCL related setup. 
######################################################################
TCL_DEF = ,"HAVE_TCL"
TCL_SRC = if_tcl.c
TCL_OBJ = if_tcl.obj
TCL_LIB =
.ENDIF

.IFDEF VIM_SNIFF
######################################################################
# SNIFF related setup. 
######################################################################
SNIFF_DEF = ,"USE_SNIFF"
SNIFF_SRC = if_sniff.c
SNIFF_OBJ = if_sniff.obj
SNIFF_LIB = 
.ENDIF

######################################################################
# End of configuration section.
#
# Please, do not change anything below without programming experience.
######################################################################

CC	= cc/decc
LD	= link

.IFDEF DEBUG
TARGET  = dvim.exe
CFLAGS	= /debug/noopt/prefix=all/include=[.proto]
LDFLAGS	= /debug
.ELSE
TARGET  = vim.exe
CFLAGS	= /opt/prefix=all/include=[.proto]
LDFLAGS	=
.ENDIF


### These go into pathdef.c
VIMLOC  = ""
VIMRUN  = ""
VIMUSER = "''f$extract(f$locate(",",f$user())+1,f$length(f$user())-f$locate(",",f$user())-2,f$user())'"
VIMHOST = "''f$extract(0,f$length(f$trnlnm("sys$node"))-2,f$trnlnm("sys$node"))' (''f$trnlnm("ucx$inet_host")'.''f$trnlnm("ucx$inet_domain")')"

	
###
### GRAPHICAL USER INTERFACE (GUI).

.SUFFIXES : .obj .c .pro .xs

ALL_CFLAGS = /def=($(DEFS) $(PERL_DEF) $(PYTHON_DEF) $(TCL_DEF) $(SNIFF_DEF)) $(CFLAGS)

ALL_LIBS = $(LIBS) $(GUI_LIB_DIR) $(X_LIB_DIR) $(GUI_LIB) $(X_LIB) $(EXTRA_LIB)\
	   $(PERL_LIB) $(PYTHON_LIB) $(TCL_LIB) $(SNIFF_LIB)

INCL =  vim.h globals.h option.h keymap.h macros.h ascii.h term.h os_unix.h \
	unixunix.h structs.h proto.h osdef.h config.h \
	$(GUI_INC)

SRC =	buffer.c charset.c digraph.c edit.c eval.c ex_cmds.c ex_docmd.c \
	ex_getln.c fileio.c getchar.c main.c mark.c menu.c multbyte.c \
	memfile.c memline.c message.c misc1.c misc2.c normal.c ops.c option.c \
	pty.c quickfix.c regexp.c search.c syntax.c tag.c term.c termlib.c \
	ui.c undo.c version.c screen.c window.c os_vms.c pathdef.c \
	$(GUI_SRC) $(PERL_SRC) $(PYTHON_SRC) $(TCL_SRC) $(SNIFF_SRC)

OBJ =	buffer.obj charset.obj digraph.obj edit.obj eval.obj ex_cmds.obj \
	ex_docmd.obj ex_getln.obj fileio.obj getchar.obj \
	main.obj mark.obj menu.obj memfile.obj memline.obj message.obj misc1.obj \
	misc2.obj multbyte.obj normal.obj ops.obj option.obj pty.obj quickfix.obj \
	regexp.obj search.obj syntax.obj tag.obj term.obj termlib.obj ui.obj \
	undo.obj screen.obj window.obj os_vms.obj pathdef.obj \
	$(GUI_OBJ) $(PERL_OBJ) $(PYTHON_OBJ) $(TCL_OBJ) $(SNIFF_OBJ)

# Default target is making the executable
all : $(TARGET) mms_vim
	! $@

.IFDEF DEVELOPMENT
config.h : $(CONFIG_H)
	copy $(CONFIG_H) config.h
.ELSE
config.h : os_vms.mms $(CONFIG_H)
	copy $(CONFIG_H) config.h
.ENDIF

osdef.h : os_vms_osdef.h
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


.IFDEF CCVER
#########################################################################
# This part can make come complications if you're using some predefined
# symbols/flags for your compiler. If does, just comment out _CCVER_ variable
##########################################################################

pathdef.c : os_vms.mms $(CONFIG_H)
	-@ define sys$output cc_ver.tmp
	-@ cc/version
	-@ deass sys$output
	-@ open/read file cc_ver.tmp
	-@ read file CC_VERSION
	-@ close file
	-@ del cc_ver.tmp.*
	-@ write sys$output "creating pathdef.c (CCVER = YES)"
	-@ open/write pd pathdef.c
	-@ write pd "/* pathdef.c -- DO NOT EDIT! */"
	-@ write pd "/* This file is automatically created by os_vms.mms"
	-@ write pd " * Change the file os_vms.mms only. */"
	-@ write pd "#include ""vim.h"""
	-@ write pd "char_u *default_vim_dir = (char_u *)"$(VIMLOC)";"
	-@ write pd "char_u *default_vimruntime_dir = (char_u *)"$(VIMRUN)";"
	-@ write pd "char_u *all_cflags = (char_u *)""$(CC) /incl=[] $(ALL_CFLAGS)\nCompiler: ''CC_VERSION'"";"
	-@ write pd "char_u *all_lflags = (char_u *)""$(LD) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)"";"
	-@ write pd "char_u *compiled_user = (char_u *) "$(VIMUSER)";" 
	-@ write pd "char_u *compiled_sys = (char_u *) "$(VIMHOST)";"
	-@ close pd
.ELSE
pathdef.c : os_vms.mms
	-@ write sys$output "creating pathdef.c"
	-@ open/write pd pathdef.c
	-@ write pd "/* pathdef.c -- DO NOT EDIT! */"
	-@ write pd "/* This file is automatically created by os_vms.mms"
	-@ write pd " * Change the file os_vms.mms only. */"
	-@ write pd "#include ""vim.h"""
	-@ write pd "char_u *default_vim_dir = (char_u *)"$(VIMLOC)";"
	-@ write pd "char_u *default_vimruntime_dir = (char_u *)"$(VIMRUN)";"
	-@ write pd "char_u *all_cflags = (char_u *)""$(CC) /incl=[] $(ALL_CFLAGS)"";"
	-@ write pd "char_u *all_lflags = (char_u *)""$(LD) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)"";"
	-@ write pd "char_u *compiled_user = (char_u *) "$(VIMUSER)";"
	-@ write pd "char_u *compiled_sys = (char_u *) "$(VIMHOST)";"
	-@ close pd
.ENDIF

os_vms.mms :
	@ write sys$output "The name of the makefile MUST be <OS_VMS.MMS> !!!!"

cmdtab.h : cmdtab.tab mkcmdtab.exe
	mkcmdtab :== $sys$disk:[]mkcmdtab
	mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab.exe : mkcmdtab.obj
	$(LD) $(LDFLAGS) mkcmdtab
mkcmdtab.obj : mkcmdtab.c
	$(CC) $(ALL_CFLAGS) mkcmdtab.c

###############################################################################
ui.obj : ui.c vim.h
	$(CC) $(ALL_CFLAGS) $<
syntax.obj : syntax.c vim.h
	$(CC) $(ALL_CFLAGS) $<
misc1.obj : misc1.c vim.h
	$(CC) $(ALL_CFLAGS) $<
misc2.obj : misc2.c vim.h
	$(CC) $(ALL_CFLAGS) $<
ex_getln.obj : ex_getln.c vim.h
	$(CC) $(ALL_CFLAGS) $<
ex_docmd.obj : ex_docmd.c vim.h ex_cmds.h
	$(CC) $(ALL_CFLAGS) $<
ex_cmds.obj : ex_cmds.c vim.h
	$(CC) $(ALL_CFLAGS) $<
alloc.obj : alloc.c vim.h config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
buffer.obj : buffer.c vim.h config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
charset.obj : charset.c vim.h config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
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
edit.obj : edit.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
	keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
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
multbyte.obj : multbyte.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
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
if_perl.obj : if_perl.xs vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC) $(ALL_CFLAGS) $<
if_perlsfio.obj : if_perlsfio.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC) $(ALL_CFLAGS) $<
if_python.obj : if_python.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \ 
	term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC) $(ALL_CFLAGS) $<
if_tcl.obj : if_tcl.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC) $(ALL_CFLAGS) $<
if_sniff.obj : if_sniff.c if_sniff.h vim.h config.h feature.h os_unix.h osdef.h ascii.h \
	keymap.h term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h \
	ex_cmds.h proto.h
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
termlib.obj : termlib.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h option.h proto.h regexp.h [.proto]termlib.pro
	$(CC) $(ALL_CFLAGS) $<
undo.obj : undo.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
os_vms.obj : os_vms.c vim.h globals.h option.h proto.h
	$(CC) $(ALL_CFLAGS) $<
window.obj : window.c vim.h config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC) $(ALL_CFLAGS) $<
version.obj : version.c version.h vim.h config.h feature.h os_unix.h \
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
pty.obj : pty.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
	keymap.h term.h macros.h structs.h gui.h proto.h regexp.h
	$(CC) $(ALL_CFLAGS) $<
mms_vim :	mms_vim.exe
	!
mms_vim.exe :	mms_vim.obj
	$(LD) $(LDFLAGS) mms_vim.obj
mms_vim.obj :	os_vms_mms.c
	$(CC) $(CFLAGS) os_vms_mms.c
	copy os_vms_mms.obj mms_vim.obj
