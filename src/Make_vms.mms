#
# Makefile for Vim on OpenVMS
#
# Maintainer:   Zoltan Arpadffy <arpadffy@altavista.net>
# Last change:  20-NOV-2000
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
#	mms/descrip=Make_vms.mms
#	   or if you use mmk
#	mmk/descrip=Make_vms.mms
#

######################################################################
# Configuration section.
######################################################################

# GUI or terminal mode executable.
# Comment out if you want just the character terminal mode only.
GUI = YES

# Compiler selection.
# Comment out if you use the VAXC compiler
DECC = YES

# Comment out if you want the compiler version with :ver command.
# NOTE: This part can make some complications if you're using some
# predefined symbols/flags for your compiler. If does, just leave behind
# the comment varialbe CCVER.
CCVER = YES

# Used to fix some dependecies problem during development.
# Development purpose only! Normally, it should not be defined. !!!
# DEVELOPMENT = YES

# Uncomment if want a debug version. Resulting executable is DVIM.EXE
# Development purpose only! Normally, it should not be defined. !!!
# DEBUG = YES

# Languages support for Perl, Python, TCL etc.
# If you don't need it really, leave them behind the comment.
# You will need special libs etc.
# VIM_TCL    = YES
# VIM_PERL   = YES
# VIM_SNIFF  = YES
# VIM_PYTHON = YES
# VIM_RUBY   = YES

# X Input Method.  For entering special languages like chinese and
# Japanese. Please define just one: VIM_XIM or VIM_HANGULIN
# If you don't need it really, leave it behind the comment.
# VIM_XIM = YES

# Internal Hangul input method. GUI only. 
# If you don't need it really, leave it behind the comment.
# VIM_HANGULIN = YES

######################################################################
# Library and include files configuration section.
# Normally you need not to change anything below. !
# These may need to be defined if things are not in standard locations
######################################################################

.IFDEF GUI
# X/Motif executable  (also works in terminal mode )
DEFS	 = "HAVE_CONFIG_H","FEAT_GUI_MOTIF"
LIBS	 = ,OS_VMS.OPT/OPT
CONFIG_H = gui_vms_conf.h
GUI_SRC  = gui.c gui_motif.c gui_x11.c
GUI_OBJ  = gui.obj gui_motif.obj gui_x11.obj
GUI_INC  = gui.h

# GUI_INC_DIR = decw$include:
# GUI_LIB_DIR = sys$library:
# X_LIB_DIR   =
# X_LIB       =
# EXTRA_LIB   =

.ELSE
# Character terminal only executable
DEFS	 = "HAVE_CONFIG_H"
LIBS	 =
CONFIG_H = os_vms_conf.h
.ENDIF

.IFDEF VIM_PERL
# Perl related setup.
PERL_DEF = ,"FEAT_PERL"
PERL_SRC = if_perlsfio.c if_perl.xs
PERL_OBJ = if_perlsfio.obj if_perl.obj
PERL_LIB =
.ENDIF

.IFDEF VIM_PYTHON
# Python related setup.
PYTHON_DEF = ,"FEAT_PYTHON"
PYTHON_SRC = if_python.c
PYTHON_OBJ = if_python.obj
PYTHON_LIB =
.ENDIF

.IFDEF VIM_TCL
# TCL related setup.
TCL_DEF = ,"FEAT_TCL"
TCL_SRC = if_tcl.c
TCL_OBJ = if_tcl.obj
TCL_LIB =
.ENDIF

.IFDEF VIM_SNIFF
# SNIFF related setup.
SNIFF_DEF = ,"FEAT_SNIFF"
SNIFF_SRC = if_sniff.c
SNIFF_OBJ = if_sniff.obj
SNIFF_LIB =
.ENDIF

.IFDEF VIM_RUBY
# RUBY related setup.
RUBY_DEF = ,"FEAT_RUBY"
RUBY_SRC = if_ruby.c
RUBY_OBJ = if_ruby.obj
RUBY_LIB =
.ENDIF

.IFDEF VIM_XIM
# XIM related setup.
.IFDEF GUI 
XIM_DEF = ,"FEAT_XIM"
.ENDIF
.ENDIF

.IFDEF VIM_HANGULIN
# HANGULIN related setup.
.IFDEF GUI
HANGULIN_DEF = ,"FEAT_HANGULIN"
HANGULIN_SRC = hangulin.c
HANGULIN_OBJ = hangulin.obj
.ENDIF
.ENDIF

######################################################################
# End of configuration section.
# Please, do not change anything below without programming experience.
######################################################################

CC      = cc

.IFDEF DECC
CC_DEF	= $(CC)/decc
PREFIX  = /prefix=all
.ELSE
CC_DEF  = $(CC)
PREFIX  =
CCVER   =
.ENDIF

LD_DEF	= link

.IFDEF DEBUG
TARGET  = dvim.exe
CFLAGS	= /debug/noopt$(PREFIX)/include=[.proto]
LDFLAGS	= /debug
.ELSE
TARGET  = vim.exe
CFLAGS	= /opt$(PREFIX)/include=[.proto]
LDFLAGS	=
.ENDIF

# These go into pathdef.c
VIMLOC  = ""
VIMRUN  = ""
VIMUSER = "''f$extract(f$locate(",",f$user())+1,f$length(f$user())-f$locate(",",f$user())-2,f$user())'"
VIMHOST = "''f$extract(0,f$length(f$trnlnm("sys$node"))-2,f$trnlnm("sys$node"))' (''f$trnlnm("ucx$inet_host")'.''f$trnlnm("ucx$inet_domain")')"

.SUFFIXES : .obj .c .pro .xs

ALL_CFLAGS = /def=($(DEFS) $(PERL_DEF) $(PYTHON_DEF) $(TCL_DEF) $(SNIFF_DEF) $(RUBY_DEF) $(XIM_DEF) $(HANGULIN_DEF)) $(CFLAGS)

ALL_LIBS = $(LIBS) $(GUI_LIB_DIR) $(X_LIB_DIR) $(GUI_LIB) $(X_LIB) $(EXTRA_LIB)\
	   $(PERL_LIB) $(PYTHON_LIB) $(TCL_LIB) $(SNIFF_LIB) $(RUBY_LIB) 

INCL =  vim.h globals.h option.h keymap.h macros.h ascii.h term.h os_unix.h \
	unixunix.h structs.h proto.h [.auto]osdef.h [.auto]config.h \
	$(GUI_INC)

SRC =	buffer.c charset.c digraph.c edit.c eval.c ex_cmds.c ex_cmds2.c ex_docmd.c \
	ex_getln.c fileio.c fold.c getchar.c main.c mark.c menu.c multibyte.c \
	memfile.c memline.c message.c misc1.c misc2.c move.c normal.c ops.c option.c \
	pty.c quickfix.c regexp.c search.c syntax.c tag.c term.c termlib.c \
	ui.c undo.c version.c screen.c window.c os_vms.c pathdef.c \
	$(GUI_SRC) $(PERL_SRC) $(PYTHON_SRC) $(TCL_SRC) $(SNIFF_SRC) $(RUBY_SRC) $(HANGULIN_SRC)

OBJ =	buffer.obj charset.obj digraph.obj edit.obj eval.obj ex_cmds.obj ex_cmds2.obj \
	ex_docmd.obj ex_getln.obj fileio.obj fold.obj getchar.obj \
	main.obj mark.obj menu.obj memfile.obj memline.obj message.obj misc1.obj \
	misc2.obj move.obj multibyte.obj normal.obj ops.obj option.obj pty.obj quickfix.obj \
	regexp.obj search.obj syntax.obj tag.obj term.obj termlib.obj ui.obj \
	undo.obj screen.obj window.obj os_vms.obj pathdef.obj \
	$(GUI_OBJ) $(PERL_OBJ) $(PYTHON_OBJ) $(TCL_OBJ) $(SNIFF_OBJ) $(RUBY_OBJ) $(HANGULIN_OBJ)

# Default target is making the executable
all : $(TARGET) mms_vim
	! $@

.IFDEF DEVELOPMENT
[.auto]config.h : $(CONFIG_H)
	copy $(CONFIG_H) [.auto]config.h
.ELSE
[.auto]config.h : make_vms.mms $(CONFIG_H)
	copy $(CONFIG_H) [.auto]config.h
.ENDIF

osdef.h : os_vms_osdef.h
	copy os_vms_osdef.h osdef.h

# Re-execute this make_vms.mms to include the new [.auto]config.mk produced by
# [.auto]configure Only used when typing "make" with a fresh [.auto]config.mk.
myself :
	mms /descrip=Make_vms.mms all

clean : 
        -@ if "''F$SEARCH("*.exe")'" .NES. "" then delete/noconfirm/nolog *.exe;*
	-@ if "''F$SEARCH("*.obj")'" .NES. "" then delete/noconfirm/nolog *.obj;*
	-@ if "''F$SEARCH("[.auto]config.h")'" .NES. "" then delete/noconfirm/nolog [.auto]config.h;*
	-@ if "''F$SEARCH("pathdef.c")'" .NES. "" then delete/noconfirm/nolog pathdef.c;*
        -@ if "''F$SEARCH("osdef.h")'" .NES. "" then delete/noconfirm/nolog osdef.h;*

# Link the target
$(TARGET) : $(OBJ) version.obj
	$(CC_DEF) $(ALL_CFLAGS) version.c
	$(LD_DEF) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)

FILES = *.c *.h make_vms.mms *.in makefile.* *.sh cmdtab.tab tags [.auto]configure

# Used when .obj files are in src directory
.c.obj :
	$(CC_DEF) $(ALL_CFLAGS) $<

pathdef.c : check_ccver $(CONFIG_H)
	-@ write sys$output "creating pathdef.c"
	-@ open/write pd pathdef.c
	-@ write pd "/* pathdef.c -- DO NOT EDIT! */"
	-@ write pd "/* This file is automatically created by Make_vms.mms"
	-@ write pd " * Change the file Make_vms.mms only. */"
	-@ write pd "typedef unsigned char   char_u;"
	-@ write pd "char_u *default_vim_dir = (char_u *)"$(VIMLOC)";"
	-@ write pd "char_u *default_vimruntime_dir = (char_u *)"$(VIMRUN)";"
	-@ write pd "char_u *all_cflags = (char_u *)""$(CC_DEF) /incl=[] $(ALL_CFLAGS)"";"
	-@ write pd "char_u *all_lflags = (char_u *)""$(LD_DEF) $(LDFLAGS) /exe=$(TARGET) *.obj $(ALL_LIBS)"";"
	-@ write pd "char_u *compiler_version = (char_u *) ""''CC_VER'"";"
	-@ write pd "char_u *compiled_user = (char_u *) "$(VIMUSER)";"
	-@ write pd "char_u *compiled_sys = (char_u *) "$(VIMHOST)";"
	-@ close pd

make_vms.mms :
	@ write sys$output "The name of the makefile MUST be <MAKE_VMS.MMS> !!!!"

cmdtab.h : cmdtab.tab mkcmdtab.exe
	mkcmdtab :== $sys$disk:[]mkcmdtab
	mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab.exe : mkcmdtab.obj
	$(LD_DEF) $(LDFLAGS) mkcmdtab
mkcmdtab.obj : mkcmdtab.c
	$(CC_DEF) $(ALL_CFLAGS) mkcmdtab.c

ui.obj : ui.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
syntax.obj : syntax.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
misc1.obj : misc1.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
misc2.obj : misc2.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
move.obj : move.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
ex_getln.obj : ex_getln.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
ex_docmd.obj : ex_docmd.c vim.h ex_cmds.h
	$(CC_DEF) $(ALL_CFLAGS) $<
ex_cmds.obj : ex_cmds.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
ex_cmds2.obj : ex_cmds2.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
alloc.obj : alloc.c vim.h [.auto]config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h
	$(CC_DEF) $(ALL_CFLAGS) $<
buffer.obj : buffer.c vim.h [.auto]config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
charset.obj : charset.c vim.h [.auto]config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
cmdcmds.obj : cmdcmds.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
cmdline.obj : cmdline.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h cmdtab.h
	$(CC_DEF) $(ALL_CFLAGS) $<
csearch.obj : csearch.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
digraph.obj : digraph.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
edit.obj : edit.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h \
	keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
fileio.obj : fileio.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
fold.obj : fold.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
getchar.obj : getchar.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
help.obj : help.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
hangulin.obj : hangulin.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
        macros.h structs.h gui.h globals.h proto.h regexp.h option.h
        $(CC_DEF) $(ALL_CFLAGS) $<
linefunc.obj : linefunc.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h
	$(CC_DEF) $(ALL_CFLAGS) $<
main.obj : main.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
mark.obj : mark.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
menu.obj : menu.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
memfile.obj : memfile.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
memline.obj : memline.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
message.obj : message.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
misccmds.obj : misccmds.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
multibyte.obj : multibyte.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
normal.obj : normal.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
ops.obj : ops.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
option.obj : option.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
pathdef.obj : pathdef.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h
	$(CC_DEF) $(ALL_CFLAGS) $<
eval.obj : eval.c vim.h
	$(CC_DEF) $(ALL_CFLAGS) $<
if_perl.obj : if_perl.xs vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
if_perlsfio.obj : if_perlsfio.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
if_python.obj : if_python.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
if_tcl.obj : if_tcl.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h regexp.h structs.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
if_sniff.obj : if_sniff.c if_sniff.h vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h \
	keymap.h term.h macros.h regexp.h structs.h gui.h globals.h farsi.h option.h \
	ex_cmds.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
quickfix.obj : quickfix.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
regexp.obj : regexp.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
regsub.obj : regsub.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h
	$(CC_DEF) $(ALL_CFLAGS) $<
ruby.obj : ruby.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
        term.h macros.h structs.h gui.h globals.h proto.h regexp.h
        $(CC_DEF) $(ALL_CFLAGS) $<
screen.obj : screen.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
search.obj : search.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
tables.obj : tables.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
tag.obj : tag.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
term.obj : term.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h option.h proto.h regexp.h
	$(CC_DEF) $(ALL_CFLAGS) $<
termlib.obj : termlib.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h option.h proto.h regexp.h [.proto]termlib.pro
	$(CC_DEF) $(ALL_CFLAGS) $<
undo.obj : undo.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
os_vms.obj : os_vms.c vim.h globals.h option.h proto.h
	$(CC_DEF) $(ALL_CFLAGS) $<
window.obj : window.c vim.h [.auto]config.h feature.h os_unix.h osdef.h \
	ascii.h keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
version.obj : version.c version.h vim.h [.auto]config.h feature.h os_unix.h \
	osdef.h ascii.h keymap.h term.h macros.h structs.h gui.h
	$(CC_DEF) $(ALL_CFLAGS) $<
gui.obj : gui.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h term.h \
	macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
gui_motif.obj : gui_motif.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
gui_x11.obj : gui_x11.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
	term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
	$(CC_DEF) $(ALL_CFLAGS) $<
pty.obj : pty.c vim.h [.auto]config.h feature.h os_unix.h osdef.h ascii.h \
	keymap.h term.h macros.h structs.h gui.h proto.h regexp.h
	$(CC_DEF) $(ALL_CFLAGS) $<

mms_vim :	mms_vim.exe
	!
mms_vim.exe :	mms_vim.obj
	$(LD_DEF) $(LDFLAGS) mms_vim.obj
mms_vim.obj :	os_vms_mms.c
	$(CC_DEF) $(CFLAGS) os_vms_mms.c
	copy os_vms_mms.obj mms_vim.obj

.IFDEF CCVER
# This part can make some complications if you're using some predefined
# symbols/flags for your compiler. If does, just comment out CCVER variable
check_ccver :
	-@ define sys$output cc_ver.tmp
	-@ $(CC)/version
	-@ deassign sys$output
	-@ open/read file cc_ver.tmp
	-@ read file CC_VER
	-@ close file
	-@ delete/noconfirm/nolog cc_ver.tmp.*
.ELSE
check_ccver :
	-@ !
.ENDIF
