#
# Makefile for Vim on OpenVMS
#
# Maintainer:   Zoltan Arpadffy <arpadffy@altavista.net>
# Last change:  2001 Jul 28
#
# This has script been tested on VMS 6.2 to 7.2 on DEC Alpha and VAX
# with MMS and MMK
#
# The following could be built:
#	vim.exe:	standard
#	dvim.exe:	debug
#
# Edit the lines in the Configuration section below for fine tuning.
#
# To build:    mms/descrip=Make_vms.mms
# To clean up: mms/descrip=Make_vms.mms clean
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
# Directory, library and include files configuration section.
# Normally you need not to change anything below. !
# These may need to be defined if things are not in standard locations
######################################################################

# Predefined VIM directories
# Please, use $VIM and $VIMRUNTIME logicals instead
VIMLOC  = ""
VIMRUN  = ""

CONFIG_H = os_vms_conf.h

.IFDEF GUI
# X/Motif executable  (also works in terminal mode )
DEFS	 = "HAVE_CONFIG_H","FEAT_GUI_MOTIF"
LIBS	 = ,OS_VMS.OPT/OPT
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
DEBUG_DEF = ,"DEBUG"
TARGET    = dvim.exe
CFLAGS	  = /debug/noopt$(PREFIX)/include=[.proto]
LDFLAGS	  = /debug
.ELSE
TARGET    = vim.exe
CFLAGS	  = /opt$(PREFIX)/include=[.proto]
LDFLAGS	  =
.ENDIF

# These go into pathdef.c
VIMUSER = "''f$extract(f$locate(",",f$user())+1,f$length(f$user())-f$locate(",",f$user())-2,f$user())'"
VIMHOST = "''f$extract(0,f$length(f$trnlnm("sys$node"))-2,f$trnlnm("sys$node"))' (''f$trnlnm("ucx$inet_host")'.''f$trnlnm("ucx$inet_domain")')"

.SUFFIXES : .obj .c

ALL_CFLAGS = /def=($(DEFS) $(DEBUG_DEF) $(PERL_DEF) $(PYTHON_DEF) $(TCL_DEF) $(SNIFF_DEF) $(RUBY_DEF) $(XIM_DEF) $(HANGULIN_DEF)) $(CFLAGS)

ALL_LIBS = $(LIBS) $(GUI_LIB_DIR) $(X_LIB_DIR) $(GUI_LIB) $(X_LIB) $(EXTRA_LIB)\
	   $(PERL_LIB) $(PYTHON_LIB) $(TCL_LIB) $(SNIFF_LIB) $(RUBY_LIB)

SRC =	buffer.c charset.c diff.c digraph.c edit.c eval.c ex_cmds.c ex_cmds2.c ex_docmd.c \
	ex_getln.c if_xcmdsrv.c fileio.c fold.c getchar.c main.c mark.c menu.c mbyte.c \
	memfile.c memline.c message.c misc1.c misc2.c move.c normal.c ops.c option.c \
	pty.c quickfix.c regexp.c search.c syntax.c tag.c term.c termlib.c \
	ui.c undo.c version.c screen.c window.c os_unix.c os_vms.c pathdef.c \
	$(GUI_SRC) $(PERL_SRC) $(PYTHON_SRC) $(TCL_SRC) $(SNIFF_SRC) $(RUBY_SRC) $(HANGULIN_SRC)

OBJ =	buffer.obj charset.obj diff.obj digraph.obj edit.obj eval.obj ex_cmds.obj ex_cmds2.obj \
	ex_docmd.obj ex_getln.obj if_xcmdsrv.obj fileio.obj fold.obj getchar.obj \
	main.obj mark.obj menu.obj memfile.obj memline.obj message.obj misc1.obj \
	misc2.obj move.obj mbyte.obj normal.obj ops.obj option.obj pty.obj quickfix.obj \
	regexp.obj search.obj syntax.obj tag.obj term.obj termlib.obj ui.obj \
	undo.obj screen.obj version.obj window.obj os_unix.obj os_vms.obj pathdef.obj \
	$(GUI_OBJ) $(PERL_OBJ) $(PYTHON_OBJ) $(TCL_OBJ) $(SNIFF_OBJ) $(RUBY_OBJ) $(HANGULIN_OBJ)

# Default target is making the executable
all : [.auto]config.h $(TARGET)
	! $@

[.auto]config.h : $(CONFIG_H)
	copy/nolog $(CONFIG_H) [.auto]config.h

clean :
        -@ if "''F$SEARCH("*.exe")'" .NES. "" then delete/noconfirm/nolog *.exe;*
	-@ if "''F$SEARCH("*.obj")'" .NES. "" then delete/noconfirm/nolog *.obj;*
	-@ if "''F$SEARCH("[.auto]config.h")'" .NES. "" then delete/noconfirm/nolog [.auto]config.h;*
	-@ if "''F$SEARCH("pathdef.c")'" .NES. "" then delete/noconfirm/nolog pathdef.c;*

# Link the target
$(TARGET) : $(OBJ)
	$(LD_DEF) $(LDFLAGS) /exe=$(TARGET) $+ $(ALL_LIBS)

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
	@ write sys$output "The name of the makefile MUST be <MAKE_VMS.MMS> !!!"

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
