#
# Makefile for Vim on MiNT				vim:ts=8:sw=8:tw=78
#
# This is a modified version of the original vim 3.29 Makefile. It is
# configured to accompany the MiNT distribution of Vim.
# The GUI, Win32, Amiga, MS-Dos, and cproto stuff has been deleted because 
# the relevant files are not included in the MiNT distribution.
# You might as well use the modified Unix Makefile (read the comments at the
# beginning of Makefile how to configure it).
#
# This Makefile is loosely based on the GNU Makefile conventions found in
# standards.info.

# Compiling Vim, summary:
#
#	3. make
#	4. make install
#
# Compiling Vim, details:
#
# Edit this file for adjusting to your system. You should not need to edit any
# other file for machine specific things!
# The name of this file MUST be Makefile (note the uppercase 'M').
#
# 1. Edit this Makefile
#    The defaults for Vim should work on most machines, but you may want to
#    uncomment some lines or make other changes below to tune it to your
#    system, compiler or preferences.
#    - Uncomment one of CFLAGS and/or CC if you have something very special or
#      want to tune the optimizer.
#    - Search for the name of your system to see if it needs anything special.
#    - A few versions of make use '.include "file"' instead of 'include file'.
#      Adjust the include line below if yours does.
#
# 2. Edit feature.h
#    Only if you do not agree with the default compile features, e.g.:
#       - you want Vim to be as vi compatible as it can be
#       - you want to use Emacs tags files
#       - you want right-to-left editing (Hebrew)
#       - you want 'langmap' support (Greek)
#       - you want to remove features to make Vim smaller
#
# 3. "make"
#    Will first run ./configure with the options in this file. Then it will
#    start make again on this Makefile to do the compiling. You can also do
#    this in two steps with:
#		make config
#		make
#    The configuration phase creates/overwrites config.h and config.mk.
#    Configure is created with autoconf. It can detect different systems and
#    act accordingly. However, it is not correct for all systems.
#    If you changed something do this to run configure again:
#		make distclean
#		make config
#		make
#
#    - If you do not trust the automatic configuration code, then inspect 
#      config.h and config.mk, before starting the actual build phase. If
#      possible edit this Makefile, rather than config.mk -- especially look
#      the definitions of VIM_HLP and SYS_VIMRC_FILE below. Note that the
#      configure phase overwrites config.mk and config.h again.
#    - If you get error messages, find out what is wrong and try to correct it
#      in this Makefile. You may need to do "make distclean" when you
#      change anything that configure uses (e.g. switching from an old C
#      compiler to an ANSI C compiler). Only when ./configure does something
#      wrong you may need to change one of the other files. If you find a
#      clean way to fix the problem, consider sending a note to the author of
#      autoconf (bug-gnu-utils@prep.ai.mit.edu) or Vim (mool@oce.nl). Don't
#      bother to do that when you made a hack solution for a non-standard
#      system.
#
# 4. "make install"
#    If the new Vim seems to be working OK you can install it and the
#    documentation in the appropriate location. The default is "/usr/local".
#    Change "PREFIX" below to change the location. "pathdef.c" will be
#    compiled again after changing this to make the executable know where the
#    help files are located.
#    Note that any existing executable and documentation is removed or
#    overwritten.  If you want to keep it you will have to make a backup copy
#    first.

### This Makefile has been succesfully tested on these systems.
### Check the (*) column for remarks, listed below.
### Later code changes may cause small problems, otherwise Vim is supposed to
### compile and run without problems.

#system:              configurations:    	     version (*) tested by:
#-------------        ------------------------       -------  -  ----------
#MiNT 1.12.5             gcc-2.6.1                      3.28     Jens Felderhoff

#DO NOT CHANGE the next line, we need it for configure to find the compiler
#instead of using the default from the "make" program.
#Use a line further down to change the value for CC.
CC=

######################## config.mk ########################
# At this position config.mk is included. When starting from the
# distribution it is almost empty. After running ./configure it contains
# settings that have been discovered for your system. Settings below this
# include override settings in config.mk!

# (X) How to include config.mk depends on the version of "make" you have, if
#     the current choice doesn't work, try the other one.

include config.mk
#.include "config.mk"

# Include the configuration choices first, so we can override everything
# below. As shipped, this file contains a target that causes to run
# configure. Once configure was run, this file contains a list of 
# make variables with predefined values instead. Thus any second invocation
# of make, will buid Vim.

# You can give a lot of options to configure. 
# Change this to your desire and do 'make config' afterwards

### For creating a plain Vim without any X11 related fancies use this one:
CONF_OPT_X = --without-x

### COMPILER -- the default from configure will mostly be fine, no need to
### change this, just an example. If a compiler is defined here, configure
### will use it rather than probing for one. It is dangerous to change this
### after configure was run. Make will use your choice then -- but beware: 
### Many things may change with another compiler. It is wise to run 'make
### distclean' and start all over again.
#CC = cc
#CC = gcc

###
### COMPILER FLAGS - change as you please. Either before running configure
### or afterwards. For examples see below.
### Linux: When using -g, you will probably get a statically linked
### executable!!
### When not defined, configure will try to use -O2 -g for gcc and -O for cc.
#CFLAGS = -g
#CFLAGS = -O
#CFLAGS = -g -O2 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes

### Often used for GCC: optimizing, lot of optimizing, debugging
#CFLAGS = -O2 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#CFLAGS = -O6 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#CFLAGS = -g -DDEBUG -Wall -Wshadow -Wmissing-prototypes

### (M)  MiNT with gcc 2.6.1 and gdb 3.5
CC = gcc -mint
CFLAGS = -g -O
EXTRA_DEFS = -DMINT
POSTPROCESS = fixstk 20k $(TARGET)
DBGLDFLAGS = -B/gnu/bin/sym-
DBGTARGET = $(TARGET).sym

#####################################################
###  Specific systems, check if yours is listed!  ###
#####################################################

### If you want to use ncurses library instead of the automatically found one
### after changing this, you need to do "make distclean; make".
#TERM_LIB = -lncurses

##################### end of system specific lines ###################

### Name of target(s)
TARGET = vim
GTARGET = gvim

### Installation directory. Default from configure is "/usr/local".
### EXEC_PREFIX is for the executable, PREFIX is for other files (they may be
### different when using different architectures for the executable and a
### common directory for the other files).
###
### Uncomment the next two lines to install in your home directory.
#PREFIX      = $(HOME)
#EXEC_PREFIX = $(HOME)
### Uncomment the next line only if the value of PREFIX is different from
### EXEC_PREFIX
#PREFIX_MKDIR = $(PREFIX)

### Location of executable
BINLOC = $(EXEC_PREFIX)/bin

### Location of man page
MANLOC = $(PREFIX)/man
MANDIR = /man1

### Location of help files and system global vimrc
### The default for HELPDIR comes from configure: "/share" or "/lib"
#HELPDIR = /lib
HELPSUBDIR = /vim
HELPLOC = $(PREFIX)$(HELPDIR)
HELPSUBLOC = $(HELPLOC)$(HELPSUBDIR)

### These go into pathdef.c
VIM_HLP = $(HELPSUBLOC)/vim_help.txt
SYS_VIMRC_FILE  = $(HELPSUBLOC)/vimrc

### Program to run on installed binary
STRIP = strip

### Permissions for vim binary
BINMOD = 755

### Permissions for man page
MANMOD = 644

### Permissions for help files
HELPMOD = 644

### Permissions for bin/man/help directories that are created
DIRMOD = 755

# Where to copy the man and help files from
HELPSOURCE = ../doc

### our grand parent directory should know who we are...
### only used for "make tar"
VIMVERSION = `eval "basename \`cd ../../; pwd\`"`

### Command to create dependencies based on #include "..."
### prototype headers are ignored due to -DPROTO, system 
### headers #include <...> are ignored if we use the -MM option, as
### e.g. provided by gcc-cpp.
### Include USE_GUI to get gependency on gui.h
CPP_DEPEND = $(CC) -M$(CPP_MM) -DPROTO -DUSE_GUI $(DEFS) $(GUI_DEFS) $(EXTRA_DEFS) $(GUI_IPATH) $(X_CFLAGS)

# flags for cproto
#     __inline and __attribute__ are not recognized by cproto
#     maybe the "/usr/bin/cc -E" has to be adjusted for some systems

NO_ATTR = -D__inline= -D"__attribute__\\(x\\)="

# This is for cproto 3.5 patchlevel 3:
# PROTO_FLAGS = -f4 -m__PARMS -d -E"$(CPP)" $(NO_ATTR)
#
# Use this for cproto 3 patchlevel 6 or below (use "cproto -V" to check):
# PROTO_FLAGS = -f4 -m__PARMS -d -E"$(CPP)" $(NO_ATTR)
# 
# Use this for cproto 3 patchlevel 7 or above (use "cproto -V" to check):
PROTO_FLAGS = -m -M__PARMS -d -E"$(CPP)" $(NO_ATTR)


################################################
##   no changes required below this line      ##
################################################

SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o .pro

ALL_CFLAGS = $(DEFS) $(GUI_DEFS) $(EXTRA_DEFS) $(GUI_IPATH) $(CFLAGS) $(X_CFLAGS)

ALL_LIBS = $(GUI_LIBS_DIR) $(X_LIBS_DIR) $(GUI_LIBS) $(X_PRE_LIBS) $(GUI_X_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(LIBS) $(EXTRA_LIBS)

PFLAGS = $(PROTO_FLAGS) -DPROTO $(DEFS) $(GUI_DEFS) $(EXTRA_DEFS) $(GUI_IPATH) $(X_CFLAGS)

#     BASIC_INCL and BASIC_SRC: files that are always used
#         GUI_INCL and GUI_SRC: extra GUI files for current configuration
# ALL_GUI_INCL and ALL_GUI_SRC: all GUI files
#
#                 INCL and SRC: files used for current configuration
#                     ALL_INCL: files used for make depend and ctags
#                      ALL_SRC: files used for make depend

BASIC_INCL = vim.h globals.h option.h keymap.h macros.h ascii.h term.h unix.h \
	unixunix.h structs.h proto.h osdef.h config.h

INCL =  $(BASIC_INCL) $(GUI_INCL)

ALL_INCL = $(BASIC_INCL) $(ALL_GUI_INCL)

BASIC_SRC =	alloc.c buffer.c charset.c \
	cmdcmds.c cmdline.c csearch.c \
	digraph.c edit.c fileio.c getchar.c \
	help.c linefunc.c main.c mark.c \
	memfile.c memline.c message.c \
	misccmds.c normal.c ops.c option.c \
	pathdef.c quickfix.c regexp.c regsub.c screen.c \
	search.c tables.c tag.c term.c \
	undo.c unix.c version.c window.c

SRC =	$(BASIC_SRC) $(GUI_SRC)

ALL_SRC = $(BASIC_SRC) $(ALL_GUI_SRC)

OBJ =	alloc.o unix.o buffer.o charset.o \
	cmdcmds.o cmdline.o csearch.o \
	digraph.o edit.o fileio.o getchar.o \
	help.o linefunc.o main.o mark.o \
	memfile.o memline.o message.o \
	misccmds.o normal.o ops.o option.o \
	pathdef.o quickfix.o regexp.o regsub.o screen.o \
	search.o tables.o tag.o term.o \
	undo.o window.o $(GUI_OBJ)

PRO_AUTO = unix.pro $(ALL_GUI_PRO) \
	alloc.pro buffer.pro charset.pro cmdcmds.pro \
	cmdline.pro csearch.pro digraph.pro edit.pro \
	fileio.pro getchar.pro help.pro linefunc.pro \
	main.pro mark.pro memfile.pro memline.pro \
	message.pro misccmds.pro normal.pro ops.pro \
	option.pro quickfix.pro regexp.pro regsub.pro \
	screen.pro search.pro tables.pro tag.pro \
	term.pro termlib.pro undo.pro version.pro window.pro

PRO_MANUAL = amiga.pro msdos.pro win32.pro

# Default target is making the executable
all: $(TARGET)

# Run ./configure with all the setting above.
config: configure
	GUI_INC_LOC="$(GUI_INC_LOC)" GUI_LIB_LOC="$(GUI_LIB_LOC)" \
		CC="$(CC)" CFLAGS="$(CFLAGS)" TERM_LIB="$(TERM_LIB)" \
	$(CONF_SHELL) ./configure $(CONF_OPT_GUI) $(CONF_OPT_X)

# When configure.in has changed, run autoconf to produce configure
# If you don't have autoconf, use the configure that's there
configure: configure.in
	autoconf
# Fix a bug in autoconf 2.8 and 2.9 (not needed for 2.10, which we use now):
#	mv -f configure configure.wrong
#	sed -e 's+$$ac_cv_lib_dnet+$$ac_cv_lib_$$ac_lib_var+' configure.wrong > configure
#	rm -f configure.wrong
	chmod 755 configure

# Re-execute this Makefile to include the new config.mk produced by configure
# Only used when typing "make" with a fresh config.mk.
myself:
	$(MAKE) -f Makefile all


# Link the target for normal use
$(TARGET): $(OBJ) version.c
	$(CC) -c $(ALL_CFLAGS) version.c
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) version.o $(ALL_LIBS)
	$(POSTPROCESS)

# Link the target for debugging
debug: $(OBJ) version.c
	$(CC) -c $(ALL_CFLAGS) version.c
	$(CC) $(LDFLAGS) $(DBGLDFLAGS) -o $(DBGTARGET) -g $(OBJ) version.o $(ALL_LIBS) 

# Generate function prototypes.  This is not needed to compile vim, but if
# you want to use it, cproto is out there on the net somewhere -- Webb
#
# When generating amiga.pro, msdos.pro and win32.pro there will be a few
# include files that can not be found, that's OK.

proto: $(PRO_AUTO) $(PRO_MANUAL)

### Would be nice if this would work for "normal" make.
### Currently it only works for (Free)BSD make.
#$(PRO_AUTO): $$(*F).c
#	cproto $(PFLAGS) -DUSE_GUI $(*F).c > $@

# Always define USE_GUI.  This will generate a few warnings if it's also
# defined in config.h, you can ignore that.
.c.pro:
	cproto $(PFLAGS) -DUSE_GUI $< > proto/$@

amiga.pro: amiga.c
	cproto $(PFLAGS) -DAMIGA -UHAVE_CONFIG_H -DBPTR=char* amiga.c > proto/amiga.pro

msdos.pro: msdos.c
	cproto $(PFLAGS) -DMSDOS -UHAVE_CONFIG_H msdos.c > proto/msdos.pro

win32.pro: win32.c
	cproto $(PFLAGS) -DWIN32 -UHAVE_CONFIG_H win32.c > proto/win32.pro

notags:
	rm -f tags

# Note: tags is made for the currently configured version, can't include both
# Motif and Athena GUI
ctags tags TAGS: notags
	$(CTAGS) $(SRC) $(ALL_INCL)

#
# Avoid overwriting an existing executable, somebody might be running it and
# overwriting it could cause it to crash.  Deleting it is OK, it won't be
# really deleted until all running processes for it have exited.  It is
# renamed first, in case the deleting doesn't work.
#
# If you want to keep an older version, rename it before running "make
# install".
#
install: $(TARGET) $(EXEC_PREFIX) $(BINLOC) $(PREFIX) $(MANLOC) $(MANLOC)$(MANDIR) $(HELPLOC) $(HELPSUBLOC)
	-mv -f $(BINLOC)/$(TARGET) $(BINLOC)/$(TARGET).old
	-rm -f $(BINLOC)/$(TARGET).old
	cp $(TARGET) $(BINLOC)
	$(STRIP) $(BINLOC)/$(TARGET)
	chmod $(BINMOD) $(BINLOC)/$(TARGET)
	-$(LINKIT)
	-$(LINKGVIM)
	sed -e s+/usr/local/lib/vim+$(HELPSUBLOC)+ $(HELPSOURCE)/vim.1 > $(MANLOC)$(MANDIR)/vim.1
	chmod $(MANMOD) $(MANLOC)$(MANDIR)/vim.1
	cp $(HELPSOURCE)/vim_*.txt $(HELPSUBLOC)
	cp $(HELPSOURCE)/vim_tags $(HELPSUBLOC)
	chmod $(HELPMOD) $(HELPSUBLOC)/vim_*.txt $(HELPSUBLOC)/vim_tags

$(EXEC_PREFIX) $(BINLOC) $(PREFIX_MKDIR) $(MANLOC) $(MANLOC)$(MANDIR) $(HELPLOC) $(HELPSUBLOC):
	-mkdir $@
	-chmod $(DIRMOD) $@

uninstall:
	rm -f $(BINLOC)/$(TARGET) $(MANLOC)$(MANDIR)/vim.1 
	rm -f $(HELPSUBLOC)/vim_*.txt $(HELPSUBLOC)/vim_tags

# Clean up all the files that have been produced, except configure's
# We support common typing mistakes for Juergen! :-)
clean celan:
	-rm -f *.o core vim.core $(TARGET) 
	-rm -f osdef.h pathdef.c mkcmdtab cmdtab.h conftest* *~

shadow:	doc
	mkdir shadow
	cd shadow; ln -s ../*.[ch] ../*.in ../*.sh ../toolcheck ../proto ../*.tab ../configure .
	cd shadow; rm -f osdef.h config.h cmdtab.h pathdef.c
	cp Makefile shadow
	echo "the first targets to make vim are: scratch config myself" > shadow/config.mk

doc:
	ln -s ../doc .

# Start configure from scratch
scrub scratch:
	rm -f config.status config.cache config.h config.log

distclean: scratch clean
	echo "the first targets to make vim are: scratch config myself" > config.mk

tar: clean
	echo packing $(VIMVERSION) ...
	VIMVERSION=$(VIMVERSION); cd ../../..; set -x; \
	  tar cvf $$VIMVERSION.tar $$VIMVERSION; \
	  gzip -nf $$VIMVERSION.tar || gzip -f $$VIMVERSION.tar

dist: distclean tar

check:

mdepend:
	-@rm -f Makefile~
	cp Makefile Makefile~
	sed -e '/\#\#\# Dependencies/q' < Makefile > tmp_make
	@for i in $(ALL_SRC) ; do \
	  echo "$$i" ; \
	  echo `echo "$$i" | sed -e 's/.c$$/.o/'`": $$i" `\
            $(CPP) $$i |\
            grep '^# .*"\./.*\.h"' |\
            sort -t'"' -u +1 -2 |\
            sed -e 's/.*"\.\/\(.*\)".*/\1/'\
          ` >> tmp_make ; \
	done
	mv tmp_make Makefile

depend:
	-@rm -f Makefile~
	cp Makefile Makefile~
	sed -e '/\#\#\# Dependencies/q' < Makefile > tmp_make
	-for i in $(ALL_SRC); do echo $$i; \
			$(CPP_DEPEND) $$i >> tmp_make; done
	mv tmp_make Makefile

# use this in case the files have been transported via an MSDOS system

FILES = *.c *.h Makefile *.in makefile.* *.sh cmdtab.tab proto/*.pro tags configure

dos2unix:
	-mv arp_prot.h arp_proto.h
	-mv makefile.arc makefile.arch
	-mv makefile.dic makefile.dice
	-mv makefile.man makefile.manx
	-mv makefile Makefile
	-mv config_h.in config.h.in || mv configxh.in config.h.in || mv configh.in config.h.in
	-mv config_m.in config.mk.in || mv configxm.in config.mk.in || mv configmk.in config.mk.in
	-mv osdef1_h.in osdef1.h.in || mv osdef1xh.in osdef1.h.in || mv osdef1h.in osdef1.h.in
	-mv osdef2_h.in osdef2.h.in || mv osdef2xh.in osdef2.h.in || mv osdef2h.in osdef2.h.in
	-mv configur.in configure.in
	-mv configur configure
	-mv gui_moti.c gui_motif.c
	-mv gui_athe.c gui_athena.c
	-mv gui_at_s.c gui_at_sb.c
	-mv gui_at_s.h gui_at_sb.h
	for i in $(FILES); do tr -d '\r\032' < $$i > ~tmp~; mv ~tmp~ $$i; echo $$i; done

###########################################################################

# Used when .o files are in src directory
.c.o:
	$(CC) -c -I$(srcdir) $(ALL_CFLAGS) $<

# Used when .o files are in src/objects directory
#$(OBJ): $$(*F).c
#	$(CC) -c -I$(srcdir) $(ALL_CFLAGS) $(*F).c -o $@

osdef.h: osdef.sh config.h osdef1.h.in osdef2.h.in
	CC="$(CC) $(ALL_CFLAGS)" srcdir=${srcdir} sh $(srcdir)/osdef.sh

pathdef.c: Makefile config.mk
	-@echo creating pathdef.c
	-@echo '/* pathdef.c -- DO NOT EDIT! */' > pathdef.c
	-@echo '/* This file is automatically created by Makefile' >> pathdef.c
	-@echo ' * Change the Makefile only. */' >> pathdef.c
	-@echo '#include "vim.h"' >> pathdef.c
	-@echo '#ifndef SYS_VIMRC_FILE' >> pathdef.c
	-@echo 'char_u *sys_vimrc_fname = (char_u *)"$(SYS_VIMRC_FILE)";' >> pathdef.c
	-@echo '#else' >> pathdef.c
	-@echo 'char_u *sys_vimrc_fname = (char_u *)SYS_VIMRC_FILE;' >> pathdef.c
	-@echo '#endif' >> pathdef.c
	-@echo '#ifndef SYS_GVIMRC_FILE' >> pathdef.c
	-@echo 'char_u *sys_gvimrc_fname = (char_u *)"$(SYS_GVIMRC_FILE)";' >> pathdef.c
	-@echo '#else' >> pathdef.c
	-@echo 'char_u *sys_gvimrc_fname = (char_u *)SYS_GVIMRC_FILE;' >> pathdef.c
	-@echo '#endif' >> pathdef.c
	-@echo '#ifndef VIM_HLP' >> pathdef.c
	-@echo 'char_u *help_fname = (char_u *)"$(VIM_HLP)";' >> pathdef.c
	-@echo '#else' >> pathdef.c
	-@echo 'char_u *help_fname = (char_u *)VIM_HLP;' >> pathdef.c
	-@echo '#endif' >> pathdef.c
	-@echo 'char_u *all_cflags = (char_u *)"$(CC) -c -I$(srcdir) $(ALL_CFLAGS)";' >> pathdef.c

Makefile:
	@echo The name of the makefile MUST be "Makefile" (with capital M)!!!!

cmdtab.h: cmdtab.tab mkcmdtab
	./mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab: mkcmdtab.c osdef.h
	$(CC) -I$(srcdir) $(ALL_CFLAGS) -o mkcmdtab mkcmdtab.c

###############################################################################
### (automatically generated by 'make depend')
### Dependencies:
alloc.o: alloc.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h
buffer.o: buffer.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
charset.o: charset.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
cmdcmds.o: cmdcmds.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
cmdline.o: cmdline.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h cmdtab.h \
  ops.h
csearch.o: csearch.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
digraph.o: digraph.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
edit.o: edit.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
fileio.o: fileio.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
getchar.o: getchar.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
help.o: help.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
linefunc.o: linefunc.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h
main.o: main.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
mark.o: mark.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
memfile.o: memfile.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
memline.o: memline.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
message.o: message.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
misccmds.o: misccmds.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
normal.o: normal.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
ops.o: ops.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
option.o: option.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
pathdef.o: pathdef.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h
quickfix.o: quickfix.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h
regexp.o: regexp.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
regsub.o: regsub.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h
screen.o: screen.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
search.o: search.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
tables.o: tables.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
tag.o: tag.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
term.o: term.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h option.h proto.h regexp.h
undo.o: undo.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
unix.o: unix.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h option.h proto.h regexp.h unixunix.h
version.o: version.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h
window.o: window.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
gui.o: gui.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h term.h \
  macros.h structs.h gui.h globals.h proto.h regexp.h option.h
gui_motif.o: gui_motif.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
gui_x11.o: gui_x11.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h ops.h
gui_athena.o: gui_athena.c vim.h config.h feature.h unix.h osdef.h ascii.h \
  keymap.h term.h macros.h structs.h gui.h globals.h proto.h regexp.h option.h \
  ops.h gui_at_sb.h
gui_at_sb.o: gui_at_sb.c vim.h config.h feature.h unix.h osdef.h ascii.h keymap.h \
  term.h macros.h structs.h gui.h gui_at_sb.h

